#ifndef COIN_SOGLYPHCACHE_H
#define COIN_SOGLYPHCACHE_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2005 by Systems in Motion.  All rights reserved.
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
 *  See <URL:http://www.coin3d.org/> for more information.
 *
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  <URL:http://www.sim.no/>.
 *
\**************************************************************************/

#ifndef COIN_INTERNAL
#error this is a private header file
#endif /* !COIN_INTERNAL */

// *************************************************************************

#include <Inventor/caches/SoCache.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/SbVec4f.h>
#include <Inventor/SbVec2f.h>
#include "../src/fonts/glyph2d.h"
#include "../src/fonts/glyph3d.h"
#include "../src/fonts/fontspec.h"

class SoGlyphCacheP;
class SoState;

// *************************************************************************

class SoGlyphCache : public SoCache {
  typedef SoCache inherited;

public:
  SoGlyphCache(SoState * state);
  ~SoGlyphCache();

  void readFontspec(SoState * state);
  const cc_font_specification * getCachedFontspec(void) const;

  void addGlyph(cc_glyph2d * glyph);
  void addGlyph(cc_glyph3d * glyph);

private:
  friend class SoGlyphCacheP;
  SoGlyphCacheP * pimpl;
};

// *************************************************************************

#endif // !COIN_SOGLYPHCACHE_H
