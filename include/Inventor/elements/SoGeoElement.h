#ifndef COIN_SOGEOELEMENT_H
#define COIN_SOGEOELEMENT_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2009 by Kongsberg SIM.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Kongsberg SIM about acquiring
 *  a Coin Professional Edition License.
 *
 *  See http://www.coin3d.org/ for more information.
 *
 *  Kongsberg SIM, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  http://www.sim.no/  sales@sim.no  coin-support@coin3d.org
 *
\**************************************************************************/

#include <Inventor/elements/SoReplacedElement.h>

class SoGeoOrigin;
class SoGeoElementP;

class COIN_DLL_API SoGeoElement : public SoReplacedElement {
  typedef SoReplacedElement inherited;

  SO_ELEMENT_HEADER(SoGeoElement);
public:
  static void initClass(void);
protected:
  virtual ~SoGeoElement();

public:

  virtual void init(SoState * state);
  
  static void set(SoState * const state, SoGeoOrigin * origin);
  static SoGeoOrigin * get(SoState * const state);
  
protected:
  
  virtual void setElt(SoGeoOrigin * origin);
  
private:
  SoGeoElementP * pimpl;
};

#endif // !COIN_SOGEOELEMENT_H
