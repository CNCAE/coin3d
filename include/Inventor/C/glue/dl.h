#ifndef COIN_GLUE_DL_H
#define COIN_GLUE_DL_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2001 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  version 2 as published by the Free Software Foundation.  See the
 *  file LICENSE.GPL at the root directory of this source distribution
 *  for more details.
 *
 *  If you desire to use Coin with software that is incompatible
 *  licensewise with the GPL, and / or you would like to take
 *  advantage of the additional benefits with regard to our support
 *  services, please contact Systems in Motion about acquiring a Coin
 *  Professional Edition License.  See <URL:http://www.coin3d.org> for
 *  more information.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  <URL:http://www.sim.no>, <mailto:support@sim.no>
 *
\**************************************************************************/

/* This is a cross-platform interface abstraction against the various
   methods on different operating systems for doing dynamic, run-time
   linking of symbols. */

#include <Inventor/C/basic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if 0 /* to get proper auto-indentation in emacs */
}
#endif /* emacs indentation */

typedef struct cc_libhandle_struct * cc_libhandle;

COIN_DLL_API cc_libhandle cc_dl_open(const char * filename);
COIN_DLL_API void * cc_dl_sym(cc_libhandle handle, const char * symbolname);
COIN_DLL_API void cc_dl_close(cc_libhandle handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !COIN_GLUE_DL_H */
