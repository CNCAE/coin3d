/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2003 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org> for  more information.
 *
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no>.
 *
\**************************************************************************/

/* Here's a posting I made on Usenet 2003-02-19 with queries about
   increasing the robustness of the symbol binding process on MSWin
   DLLs. I'm keeping it here for reference, just so we remember that
   there are potential problems:
  
   ------8<------[snip]------8<------[snip]------8<------[snip]----
      Newsgroups: comp.os.ms-windows.programmer.win32
      Subject: Possible to do calling convention query?
      Gcc: nnfolder+archive:out-news
      From: Morten Eriksen <mortene@sim.no>
      Organization: The Underpant Gnomes
      --text follows this line--
      Hi,
      
      I have a delicate problem with robustness which I would like to try to
      solve. The outline of my problem is as follows:
      
        * I'm using LoadLibrary() to bind to the symbols of a DLL at
          run-time.
      
        * This DLL was generated outside of my control.
      
        * I bind to function pointer symbols of the DLL with something like
          the following snippet of code:
      
            typedef void (__stdcall * nurbsProperty_t)(void *, int, float);
            nurbsProperty_t nurbsProperty =
                 (nurbsProperty_t)GetProcAddress(<dllhnd>, "nurbsProperty");
      
            // Now the function can be used like any other, e.g. like this:
            //
            // nurbsProperty(NULL, 0, 0.0f);
      
        * Now, notice the "__stdcall" part of the function signature
          typedef. As you all probably know, this specifies the _assumed_
          calling convention of the function, i.e. how arguments are pushed
          on the stack, and if it's the caller's or the callee's
          responsibility to clean up the stack afterwards.
      
      Now here's the problem I would like to solve: if the "nurbsProperty()"
      function of the DLL was built with a different calling convention than
      __stdcall (say, __cdecl), my subsequent invocation of the method will
      lead to either a corrupted set of input arguments, a corrupted stack
      upon return, or both. Therefore, I would like to *detect* whether the
      DLL function symbols actually matches the calling convention I expect.
      
      Can this be done through the Win32 API somehow? I.e. getting to know
      what calling convention an exported function in a DLL was built with?
      
      (I guess it can be done by loading the DLL file into memory "raw", and
      parsing it's structures of the DLL / PE file, but that is _really_ the
      last resort.)
      
      Regards,
      Morten
      -- 
      Ees a sad an' beautiful world
   ------8<------[snip]------8<------[snip]------8<------[snip]----
  
   20030219 mortene.
  
   UPDATE 20030221 mortene: from the replies, it looks like this is
   impossible to do in a "clean" way (i.e. through the Win32
   API). Only suggestion that could be implemented so far is to check
   the value of the stack pointer before and after calling a function
   from the DLL.
  
   Additional suggestion from Lars: we could let the user control
   calling convention from the client side by setting an environment
   variable. That would be really helpful for remote debugging of
   problems suspected to be caused by mismatches.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_DLFCN_H
#include <dlfcn.h> /* Programming interface to libdl. */
#endif /* HAVE_DLFCN_H */

#if HAVE_WINDOWS_H
#include <windows.h>
#endif /* HAVE_WINDOWS_H */

#ifdef HAVE_MACH_O_DYLD_H
#include <mach-o/dyld.h>
#include <mach-o/ldsyms.h>
#endif /* HAVE_MACH_O_DYLD_H */

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/C/glue/dl.h>
#include <Inventor/C/tidbits.h>
#include <assert.h>
#include <stddef.h> /* NULL definition. */
#include <stdlib.h> /* atoi() */
#include <string.h> /* strlen(), strcpy() */
#include <stdio.h>  /* snprintf() */
#ifdef HAVE_LIBGEN_H
#include <libgen.h> /* dirname() */
#endif /* HAVE_LIBGEN_H */
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h> /* PATH_MAX */
#endif /* HAVE_SYS_PARAM_H */
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>  /* stat() */
#endif /* HAVE_SYS_STAT_H */

struct cc_libhandle_struct {
  void * nativehnd;
  cc_string libname;
};

/* Return value of COIN_DEBUG_DL environment variable. */
static int
cc_dl_debugging(void)
{
  static int d = -1;
  if (d == -1) {
    const char * val = coin_getenv("COIN_DEBUG_DL");
    d = val ? atoi(val) : 0;
  }
  return (d > 0) ? 1 : 0;
}

#ifdef HAVE_WINDLL_RUNTIME_BINDING
/* Returns the string and error code describing the cause of an
   internal Win32 API error. */
static void
cc_dl_get_win32_err(DWORD * lasterr, cc_string * str)
{
  LPTSTR buffer;
  BOOL result;

  *lasterr = GetLastError();
  result = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                         FORMAT_MESSAGE_FROM_SYSTEM |
                         FORMAT_MESSAGE_IGNORE_INSERTS,
                         NULL,
                         *lasterr,
                         0,
                         (LPTSTR)&buffer,
                         0,
                         NULL);

  if (!result) {
    cc_debugerror_post("cc_dl_get_Win32_err", "FormatMessage() failed!?");
  }
  else {
    cc_string_set_text(str, buffer);
    (void)LocalFree(buffer);
  }
}
#endif /* HAVE_WINDLL_RUNTIME_BINDING */


#if defined (HAVE_DYLD_RUNTIME_BINDING)

/* Returns a string containing the search directories for
   dynamic libraries, separated by ':'. Needed since Mac OS X
   wants to have a full path to the library when loading it. */

static const char * 
cc_build_search_list(const char * libname)
{
  int image_count = _dyld_image_count();
  int i;
  size_t length;
  char * res_path = NULL;
  char * p = NULL;
  char * path, * framework_path, * dyld_path, * default_path;

  
  /* First, let's see if we have this library as Framework in
     /Library/Frameworks/$libname.framework/$libname. 

     Note that this will only work if the framework is installed in
     /Library/Frameworks.  The more correct thing to do would maybe be
     to search within the DYLD_FRAMEWORK_PATH, but then again, loading
     frameworks at runtime is not something you are supposed to do in
     general, even though it is technically possible. (The "Foo" file
     in Foo.framework/Foo is nothing but a plain MH_DYLIB file, so we
     can open it as if it was a *.dylib)

     Actually, this is a hack created for OpenAL, since the source
     distribution is major PITA to set up (at least for $Mac_user),
     and the binary installer by Creative installs OpenAL as
     framework. 

     kyrah 20030311 */

  const char * framework_prefix = "/Library/Frameworks/";
  const char * framework_ext = ".framework";
  length = strlen(framework_prefix) + strlen(framework_ext) +
    strlen(libname) + 1;
  framework_path = malloc(length);
  snprintf(framework_path, length, "%s%s%s", framework_prefix,
           libname, framework_ext);
  

  /* We also want to search in the default locations (specified by
     DYLD_LIBRARY_PATH, and the system's library path). */

  dyld_path = getenv("DYLD_LIBRARY_PATH"); 
  if (!dyld_path) dyld_path = "";
  
  default_path = getenv("DYLD_FALLBACK_LIBRARY_PATH");
  if (!default_path) default_path = "/usr/local/lib:/lib:/usr/lib";


  /* If we cannot find the library on the system, we might have a
     fallback shipped with Coin / the Inventor.framework.  Get the
     file system path to the actually loaded Inventor.framework, and
     look for the library in its Resources folder. */

  for (i = 0; i < image_count; i++) {
    if (_dyld_get_image_header(i) == &_mh_dylib_header) {
      p = _dyld_get_image_name(i);
      if (strstr(p, "Inventor.framework")) {
        /* We get /path/to/Foo.framework/Versions/A/Libraries/foo.dylib
           but want /path/to/Foo.framework/Versions/A/Resources */
        char * path_to_version_dir = dirname(dirname(p));
        size_t l = strlen(path_to_version_dir) + strlen("/Resources") + 1;
        res_path = malloc(l);
        snprintf(res_path, l, "%s%s", path_to_version_dir, "/Resources");
      }
      break;
    }
  }
  
  length = strlen(framework_path) + strlen(dyld_path) + strlen(default_path) + 
           (res_path ? strlen(res_path) : 0) + 4;

  path = malloc(length);
  snprintf(path, length, "%s%s%s%s%s%s%s",
           framework_path, ":",
           dyld_path, dyld_path[0] ? ":" : "", default_path, 
           res_path ? ":" : "" , res_path ? res_path : "");

  free(framework_path);
  free(res_path);
  return path;
}

/* Get the full path for the ith entry in the search list. */

static const char * 
cc_get_full_path(int i, const char * file)
{
  /* FIXME: how many entries should we support? 
     64 is a random value. kyrah 20030306 */
  #define MAX_NR_PATH_ENTRIES 64

  static char fullpath[PATH_MAX];
  static const char * list = 0;
  static const char * path[MAX_NR_PATH_ENTRIES] = { 0 };
  static int end_reached = 0;
  
  /* Create list the first time around. */
  if (!list && !end_reached) list = cc_build_search_list(file);
  
  while (!path[i] && !end_reached) {
    path[i] = strsep((char **) &list, ":");
    if (path[i][0] == 0) path[i] = 0;
    end_reached = (list == 0);
  }

  if (path[i]) {
    snprintf(fullpath, PATH_MAX, "%s/%s", path[i], file);
    return fullpath;
  }

  return NULL;
}

/* Try to determine full path for file. */

static const struct stat *
cc_find_file(const char * file, const char ** fullpath)
{
  int i = 0;
  static struct stat sbuf;

  *fullpath = file;

  while ((*fullpath = cc_get_full_path(i++, file))) {
    if (stat(*fullpath, &sbuf) == 0) {
      return &sbuf;
    }   
  }
  
  return 0;
}

#endif /* HAVE_DYLD_RUNTIME_BINDING */


cc_libhandle
cc_dl_open(const char * filename)
{

  cc_libhandle h = (cc_libhandle) malloc(sizeof(struct cc_libhandle_struct));
  /* if (!h), FIXME: exception handling. 20020906 mortene. */
  h->nativehnd = NULL;

  /* FIXME: support HP-UX shn_load()? (dlopen() is missing on older
     HP-UX versions.)

     Some versions of HP-UX has dlopen() (from version 11 and
     onwards?). Although according to a discussion on the libtool
     mailinglist it has been buggy in an official release, needing a
     patch to function properly. This is of course a good reason to
     try to use shn_load() *first*, then dlopen() on HP-UX.

     Note also that it looks like dlopen() might reside in a library
     "svld" instead of "dl".

     20010626 mortene. */

#ifdef HAVE_DL_LIB

  h->nativehnd = dlopen(filename, RTLD_LAZY);
  /*
    If dlopen() fails for any reason than not being able to find the
    dynamic link-library given by "filename" on disk, we should really
    detect it and report an error, whether we're running in debug mode
    or release mode.

    The libdl interface doesn't provide any means to do that, though,
    so we'll just /assume/ that a NULL return means the library
    couldn't be found.

    But if a special debugging environment variable is found, we'll
    spit out the error message, which could prove useful for remote
    debugging:
  */

  if (cc_dl_debugging() && (h->nativehnd == NULL)) {
    const char * e = dlerror();
    if (e) {
      cc_debugerror_post("cc_dl_open", "dlopen(\"%s\") failed with: '%s'", 
                         filename, e);
    }
  }

#elif defined (HAVE_DYLD_RUNTIME_BINDING) 

  NSLinkEditErrors c;
  int e;
  const char * file;
  const char * errstr;

  if (filename != NULL) {

    /* Note that we must use NSAddImage, since we want to load a
       shared library, instead of NSCreateObjectFileImageFromFile()
       and NSLinkModule(), which work only with loadable
       modules/bundles. See man 3 NSModule, man 3 NSObjectFileImage
       and http://fink.sourceforge.net/doc/porting/shared.php for
       details. */
 
    const struct stat * filestat;
    const char * fullpath;
    if (!(filestat = cc_find_file(filename, &fullpath))) return NULL;

    if (cc_dl_debugging()) {
      cc_debugerror_postinfo("cc_dlopen", "opening: %s", fullpath);
    }

    h->nativehnd = (void *) NSAddImage(fullpath, 
                                       NSADDIMAGE_OPTION_RETURN_ON_ERROR);

    if (cc_dl_debugging() && !h->nativehnd) {
      NSLinkEditError(&c, &e, &file, &errstr);
      cc_debugerror_post("cc_dlopen", "%s", errstr);
    }

  } else {

    h->nativehnd = NULL;
    return h;

  }

#elif defined (HAVE_WINDLL_RUNTIME_BINDING)

  /* FIXME: if filename==NULL, could we use Module32First() and
     Module32Next() to cycle through the loaded modules, to "fake"
     what happens on dlopen(NULL) on UNIX-systems?

     That would still not work on NT4, which is missing the Tool Help
     API. Check in a Win32-API related Usenet group if there is any
     other way to resolve symbols in the current process
     image. 20021015 mortene. */


  /* Don't use GetModuleHandle(): LoadLibrary() will *not* load a new
     image if the module is already loaded, it will only inc the
     reference count.

     Also, GetModuleHandle() doesn't inc the reference count, so it is
     dangerous in the sense that the module could be free'd from
     somewhere else between us opening it, and until it is used for
     resolving symbols.
  */

  if (filename != NULL)
    h->nativehnd = LoadLibrary(filename);
  else
    return NULL;

  /*
    We don't want to call LoadLibrary(NULL) because this causes a 
    crash on some Windows platforms (Crashes on Windows2000 has 
    been reported). 20021101 thammer
   */

  if (cc_dl_debugging() && (h->nativehnd == NULL)) {
    DWORD lasterr;
    cc_string errstr;

    cc_string_construct(&errstr);
    cc_dl_get_win32_err(&lasterr, &errstr);
    cc_debugerror_post("cc_dl_open", "LoadLibrary(\"%s\") failed with: '%s'",
                       filename ? filename : "(null)",
                       cc_string_get_text(&errstr));
    cc_string_clean(&errstr);
  }

#endif

  if (h->nativehnd == NULL) {
    free(h);
    h = NULL;
  }
  else {
    cc_string_construct(&h->libname);
    cc_string_set_text(&h->libname, filename ? filename : "(null)");

    if (cc_dl_debugging()) {
#ifdef HAVE_WINDLL_RUNTIME_BINDING
      char libpath[512];
      DWORD retval = GetModuleFileName(h->nativehnd, libpath, sizeof(libpath));
      assert(retval > 0 && "GetModuleFileName() failed");
      libpath[sizeof(libpath) - 1] = 0;
      cc_debugerror_postinfo("cc_dl_open", "Opened library '%s'", libpath);
#elif defined (HAVE_DL_LIB)
      cc_debugerror_postinfo("cc_dl_open", "Opening library '%s'", cc_string_get_text(&h->libname));
#endif
    }
  }

  return h;
}

void *
cc_dl_sym(cc_libhandle handle, const char * symbolname)
{
  void * ptr = NULL;  

#ifdef HAVE_DL_LIB

  if ((handle == NULL) || (handle->nativehnd == NULL)) return NULL;
  ptr = dlsym(handle->nativehnd, symbolname);

  if (cc_dl_debugging()) {
    const char * e = dlerror();
    if (e) {
      cc_debugerror_post("cc_dl_sym", "dlsym(\"%s\", \"%s\") failed with: '%s'",
                         cc_string_get_text(&handle->libname), symbolname, e);
    }
  }

#elif defined (HAVE_DYLD_RUNTIME_BINDING) 

  /* Note: The dlopen() version returns NULL here if handle or
     handle->nativehnd are NULL, but we do not need a handle for
     symbol lookup on Mac OS X - if we have one, it makes the lookup
     faster, but that's all, so we can get away with having no valid
     handle. */

  NSSymbol symbol = NULL;
  char * mangledname;
  NSLinkEditErrors c;
  int e;
  const char * file;
  const char * errstr;

  if (cc_dl_debugging()) {
    cc_debugerror_postinfo("cc_dl_sym", "Looking up symbol %s", 
                       symbolname);
  }

  mangledname = malloc(strlen(symbolname) + 2);
  strcpy(mangledname + 1, symbolname);
  mangledname[0] = '_';

  if (handle && handle->nativehnd) {
    if (NSIsSymbolNameDefinedInImage(handle->nativehnd, mangledname))
      symbol = NSLookupSymbolInImage(handle->nativehnd, mangledname,
               NSLOOKUPSYMBOLINIMAGE_OPTION_BIND |
               NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR);
  } 

  /* If we did not specifically load the library ourselves
     (handle->nativehandle being NULL), or if the symbol could not be
     found in the library, let's try if we can find it in any of the
     loaded libs. */

  if (!symbol && NSIsSymbolNameDefined(mangledname)) {
    symbol = NSLookupAndBindSymbol(mangledname);
  } 

  if (cc_dl_debugging()) {
    if (symbol == NULL) {
      NSLinkEditError(&c, &e, &file, &errstr);
      cc_debugerror_post("cc_dl_sym", "symbol %s not found: %s", 
                         symbolname, errstr);
    }
  }

  free (mangledname);
  ptr = symbol ? NSAddressOfSymbol(symbol) : NULL;  
  
#elif defined (HAVE_WINDLL_RUNTIME_BINDING)

  if ((handle == NULL) || (handle->nativehnd == NULL)) return NULL;
  ptr = GetProcAddress(handle->nativehnd, symbolname);

  if (cc_dl_debugging() && (ptr == NULL)) {
    DWORD lasterr;
    cc_string errstr;

    cc_string_construct(&errstr);
    cc_dl_get_win32_err(&lasterr, &errstr);
    cc_debugerror_post("cc_dl_sym",
                       "GetProcAddress(\"%s\", \"%s\") failed with: '%s'",
                       cc_string_get_text(&handle->libname), symbolname,
                       cc_string_get_text(&errstr));
    cc_string_clean(&errstr);
  }

#endif

  return ptr;
}

void
cc_dl_close(cc_libhandle handle)
{
#ifdef HAVE_DL_LIB

  int result = dlclose(handle->nativehnd);

  if (result != 0) {
    const char * e = dlerror();
    if (e) {
      cc_debugerror_post("cc_dl_close", "dlclose(\"%s\") failed with: '%s'",
                         cc_string_get_text(&handle->libname), e);
    }
  }

#elif defined (HAVE_DYLD_RUNTIME_BINDING) 

  /* Do nothing. */

  /* Unlike on ELF systems, you cannot unload unload Mach-O shared
     libraries. See man 3 NSModule, man 3 NSObjectFileImage and
     http://fink.sourceforge.net/doc/porting/shared.php for details. */

#elif defined (HAVE_WINDLL_RUNTIME_BINDING)

  BOOL result = FreeLibrary(handle->nativehnd);

  if (!result) {
    DWORD lasterr;
    cc_string errstr;

    cc_string_construct(&errstr);
    cc_dl_get_win32_err(&lasterr, &errstr);
    cc_debugerror_post("cc_dl_close",
                       "FreeLibrary(\"%s\") failed with: '%s'",
                       cc_string_get_text(&handle->libname),
                       cc_string_get_text(&errstr));
    cc_string_clean(&errstr);
  }

#endif

  cc_string_clean(&handle->libname);
  free(handle);
}
