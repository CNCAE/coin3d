#ifndef COIN_SOTEXTURESCALEPOLICYELEMENT_H
#define COIN_SOTEXTURESCALEPOLICYELEMENT_H

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
#endif // !COIN_INTERNAL

#include <Inventor/elements/SoInt32Element.h>

class SoTextureScalePolicyElement : public SoInt32Element {
  typedef SoInt32Element inherited;

  SO_ELEMENT_HEADER(SoTextureScalePolicyElement);

public:
  static void initClass(void);
protected:
  virtual ~SoTextureScalePolicyElement();

public:
  enum Policy {
    USE_TEXTURE_QUALITY,
    SCALE_DOWN,
    SCALE_UP,
    FRACTURE
  };

  virtual void init(SoState * state);

  static  void set(SoState * const state, SoNode * const node,
                   const Policy policy);
  static  void set(SoState * const state, const Policy policy);
  static Policy get(SoState * const state);
  static Policy getDefault(void);

};

#endif // !COIN_SOTEXTURESCALEPOLICYELEMENT_H
