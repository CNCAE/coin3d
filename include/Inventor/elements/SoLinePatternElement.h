#ifndef COIN_SOLINEPATTERNELEMENT_H
#define COIN_SOLINEPATTERNELEMENT_H

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

#include <Inventor/elements/SoInt32Element.h>

class COIN_DLL_API SoLinePatternElement : public SoInt32Element {
  typedef SoInt32Element inherited;

  SO_ELEMENT_HEADER(SoLinePatternElement);
public:
  static void initClass(void);
protected:
  virtual ~SoLinePatternElement();

public:

  enum Patterns {
    CONTINUOUS = 0xFFFF
  };

  virtual void init(SoState * state);

  static  void set(SoState * const state, SoNode * const node,
                   const int32_t pattern);
  static  void set(SoState * const state, const int32_t pattern);
  static  int32_t get(SoState * const state);
  static  int32_t getDefault();

};

#endif // !COIN_SOLINEPATTERNELEMENT_H
