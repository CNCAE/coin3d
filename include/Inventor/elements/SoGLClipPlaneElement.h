#ifndef COIN_SOGLCLIPPLANEELEMENT_H
#define COIN_SOGLCLIPPLANEELEMENT_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2002 by Systems in Motion.  All rights reserved.
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
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no>.
 *
\**************************************************************************/

#include <Inventor/elements/SoClipPlaneElement.h>

class COIN_DLL_API SoGLClipPlaneElement : public SoClipPlaneElement {
  typedef SoClipPlaneElement inherited;

  SO_ELEMENT_HEADER(SoGLClipPlaneElement);
public:
  static void initClass(void);
protected:
  virtual ~SoGLClipPlaneElement();

public:
  virtual void init(SoState * state);
  virtual void pop(SoState * state,
                   const SoElement * prevTopElement);
  static  int getMaxGLPlanes(void);

protected:
  virtual void addToElt(const SbPlane & plane,
                        const SbMatrix & modelMatrix);

private:
  static int maxGLPlanes;

};

#endif // !COIN_SOGLCLIPPLANEELEMENT_H
