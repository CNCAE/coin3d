#ifndef COIN_GLUE_FREETYPE_H
#define COIN_GLUE_FREETYPE_H

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

#ifndef COIN_INTERNAL
#error this is a private header file
#endif

#include <Inventor/C/basic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if 0 /* to get proper auto-indentation in emacs */
}
#endif /* emacs indentation */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_FREETYPE
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#else /* HAVE_FREETYPE */

/* 
   We need some freetype structs, so define them here for runtime
   linking support (we want to avoid including <freetype.h>).
*/

/* we also need some defines */
#define FT_LOAD_DEFAULT 0x0
#define FT_FACE_FLAG_KERNING (1L <<  6)
#define FT_KERNING_DEFAULT  0
#define FT_RENDER_MODE_MONO 2
#define ft_kerning_default FT_KERNING_DEFAULT
#define ft_render_mode_mono FT_RENDER_MODE_MONO

#define FT_HAS_KERNING(face) (face->face_flags & FT_FACE_FLAG_KERNING)

/* ...and lots of typedefs */
typedef void * FT_Library;
typedef int FT_Error;
typedef int FT_Int;
typedef long FT_Long;
typedef char FT_String;
typedef short FT_Short;
typedef unsigned short FT_UShort;
typedef struct FT_FaceRec_* FT_Face;
typedef struct FT_SizeRec_ * FT_Size;
typedef signed long FT_Fixed;
typedef signed long FT_Pos;
typedef int32_t FT_UInt32;
typedef unsigned int FT_UInt;
typedef struct FT_GlyphRec_ * FT_Glyph;
typedef struct FT_CharMapRec_ * FT_CharMap;
typedef struct FT_GlyphSlotRec_ * FT_GlyphSlot;
typedef struct FT_BitmapGlyphRec_ * FT_BitmapGlyph;

typedef struct FT_Matrix_ {
  long xx, xy;
  long yx, yy;
} FT_Matrix;

typedef struct FT_Vector_ {
  long x;
  long y;
} FT_Vector;

typedef struct {
  FT_Short height;
  FT_Short width;
} FT_Bitmap_Size;

typedef void (*FT_Generic_Finalizer)(void*  object);

typedef struct {
  void * data;
  FT_Generic_Finalizer finalizer;
} FT_Generic;

typedef struct {
  FT_Pos  xMin, yMin;
  FT_Pos  xMax, yMax;  
} FT_BBox;

struct FT_CharMapRec_ {
  FT_Face face;
  int encoding;
  FT_UShort platform_id;
  FT_UShort encoding_id;  
};

typedef struct {
  int rows;
  int width;
  int pitch;
  unsigned char * buffer;
  short num_grays;
  char pixel_mode;
  char palette_mode;
  void * palette;
} FT_Bitmap;

struct FT_FaceRec_ {
  FT_Long num_faces;
  FT_Long face_index;
  
  FT_Long face_flags;
  FT_Long style_flags;
  
  FT_Long num_glyphs;
  
  FT_String * family_name;
  FT_String * style_name;
  
  FT_Int num_fixed_sizes;
  FT_Bitmap_Size * available_sizes;
  
  FT_Int num_charmaps;
  FT_CharMap * charmaps;
  
  FT_Generic generic;
  
  FT_BBox bbox;
  
  FT_UShort units_per_EM;
  FT_Short ascender;
  FT_Short descender;
  FT_Short height;
  
  FT_Short max_advance_width;
  FT_Short max_advance_height;
  
  FT_Short underline_position;
  FT_Short underline_thickness;
  
  FT_GlyphSlot glyph;
  
  FT_Size size;
  FT_CharMap charmap;
  
  /* private begin.
   *
   * FT_Driver         driver;
   * FT_Memory         memory;
   * FT_Stream         stream;
   *
   * FT_ListRec        sizes_list;
   * 
   * FT_Generic        autohint;
   * void*             extensions;
   * 
   * FT_Face_Internal  internal;
   * 
   * private end */ 
};

typedef struct FT_GlyphRec_ {
  FT_Library library;
  void * class;
  int format;
  FT_Vector advance;
} FT_GlyphRec;

typedef struct {
  short       n_contours;
  short       n_points;
  FT_Vector*  points;
  char*       tags;
  short*      contours;
  int         flags;
} FT_Outline;

typedef struct {
  FT_Pos width;
  FT_Pos height;
  FT_Pos horiBearingX;
  FT_Pos horiBearingY;
  FT_Pos horiAdvance;
  FT_Pos vertBearingX;
  FT_Pos vertBearingY;
  FT_Pos vertAdvance;
} FT_Glyph_Metrics;

struct FT_GlyphSlotRec_ {
  FT_Library library;
  FT_Face face;
  FT_GlyphSlot next;
  FT_UInt flags;
  FT_Generic generic;
  
  FT_Glyph_Metrics metrics;
  FT_Fixed linearHoriAdvance;
  FT_Fixed linearVertAdvance;
  FT_Vector advance;
  
  int format; /* FT_Glyph_Format (enum) */
  
  FT_Bitmap bitmap;
  FT_Int bitmap_left;
  FT_Int bitmap_top;
  
  FT_Outline outline;
  
  FT_UInt num_subglyphs;
  void * subglyphs; /* FT_SubGlyph (struct ptr) */
  
  void * control_data;
  long control_len;
  
  void * other;
  void * internal;
};

typedef struct {
  FT_UShort x_ppem;
  FT_UShort y_ppem;

  FT_Fixed x_scale;
  FT_Fixed y_scale;
  
  FT_Pos ascender;
  FT_Pos descender;
  FT_Pos height;
  FT_Pos max_advance;
} FT_Size_Metrics;

struct FT_Size_ {
  FT_Face face;
  FT_Generic generic;
  FT_Size_Metrics metrics;
  void * internal;
};

struct FT_BitmapGlyphRec_ {
  FT_GlyphRec  root;
  FT_Int left;
  FT_Int top;
  FT_Bitmap bitmap;
};

#endif /* !HAVE_FREETYPE */

int cc_ftglue_available(void);

FT_Error cc_ftglue_FT_Init_FreeType(void ** library);
void cc_ftglue_FT_Library_Version(void * library, int * major, int * minor, int * patch);
void cc_ftglue_FT_Done_FreeType(void * library);
FT_Error cc_ftglue_FT_New_Face(void * library, const char * filepathname, long faceindex, FT_Face * face);
FT_Error cc_ftglue_FT_Done_Face(void * face);
FT_Error cc_ftglue_FT_Select_Charmap(FT_Face face, int encoding);
FT_Error cc_ftglue_FT_Set_Char_Size(FT_Face face, long width, long height, unsigned int hres, unsigned int vres);
void cc_ftglue_FT_Set_Transform(FT_Face face, FT_Matrix * matrix, FT_Vector * delta);
FT_Error cc_ftglue_FT_Get_Char_Index(FT_Face face, unsigned long charidx);
FT_Error cc_ftglue_FT_Load_Glyph(FT_Face face, unsigned int glyph, int32_t loadflags);
FT_Error cc_ftglue_FT_Get_Kerning(FT_Face face, unsigned int left, unsigned int right, unsigned int kernmode, FT_Vector * akerning);
FT_Error cc_ftglue_FT_Get_Glyph(void * glyphslot, FT_Glyph * glyph);
FT_Error cc_ftglue_FT_Glyph_To_Bitmap(FT_Glyph * glyph, int rendermode, FT_Vector * origin, int destroy);
void cc_ftglue_FT_Done_Glyph(FT_Glyph glyph);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !COIN_GLUE_FREETYPE_H */
