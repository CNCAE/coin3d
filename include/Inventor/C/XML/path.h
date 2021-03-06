#ifndef COIN_XML_PATH_H
#define COIN_XML_PATH_H

/**************************************************************************\
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\**************************************************************************/

#include <stdarg.h>

#include <Inventor/C/XML/types.h>

/* ********************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

COIN_DLL_API cc_xml_path * cc_xml_path_new(void);
COIN_DLL_API void          cc_xml_path_delete_x(cc_xml_path * path);

COIN_DLL_API void          cc_xml_path_clear_x(cc_xml_path * path);
COIN_DLL_API void          cc_xml_path_set_x(cc_xml_path * path, ...); /* use 'v' in signature? */
COIN_DLL_API void          cc_xml_path_copy_x(cc_xml_path * path, cc_xml_path * path2);
COIN_DLL_API void          cc_xml_path_reverse_x(cc_xml_path * path);

COIN_DLL_API int           cc_xml_path_get_length(const cc_xml_path * path);
COIN_DLL_API const char *  cc_xml_path_get_type(const cc_xml_path * path, int idx);
COIN_DLL_API int           cc_xml_path_get_index(const cc_xml_path * path, int idx);
COIN_DLL_API int           cc_xml_path_match_p(const cc_xml_path * path, const cc_xml_elt * elt);

COIN_DLL_API void          cc_xml_path_append_x(cc_xml_path * path, const char * elt, int idx);
COIN_DLL_API void          cc_xml_path_append_path_x(cc_xml_path * path, cc_xml_path * path2);
COIN_DLL_API void          cc_xml_path_prepend_x(cc_xml_path * path, const char * elt, int idx);
COIN_DLL_API void          cc_xml_path_prepend_path_x(cc_xml_path * path, cc_xml_path * path2);
COIN_DLL_API void          cc_xml_path_truncate_x(cc_xml_path * path, int length);

COIN_DLL_API void          cc_xml_path_dump(const cc_xml_path * path);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

/* ********************************************************************** */

#endif /* !COIN_XML_PATH_H */
