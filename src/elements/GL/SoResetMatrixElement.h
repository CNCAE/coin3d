#ifndef COIN_SORESETMATRIXELEMENT_H
#define COIN_SORESETMATRIXELEMENT_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2008 by Kongsberg SIM.  All rights reserved.
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

// private helper element to avoid caching problems with SoResetTransform

#include <Inventor/elements/SoViewingMatrixElement.h>

class SoResetMatrixElement : public SoElement {
  typedef SoElement inherited;
  
  SO_ELEMENT_HEADER(SoResetMatrixElement);

public:
  static void initClass(void);
protected:
  virtual ~SoResetMatrixElement();

public:
  virtual void init(SoState * state);
  static void set(SoState * const state, const SbMatrix & m);
  static const SbMatrix  & get(SoState * state);

protected:
  virtual void setElt(const SbMatrix & matrix);
  virtual SbBool matches(const SoElement * element) const;
  virtual SoElement * copyMatchInfo(void) const;

private:
  SbMatrix matrix;
};

#endif // COIN_SORESETMATRIXELEMENT_H
