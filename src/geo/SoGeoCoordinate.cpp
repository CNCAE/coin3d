/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2007 by Systems in Motion.  All rights reserved.
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
 *  See http://www.coin3d.org/ for more information.
 *
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  http://www.sim.no/  sales@sim.no  coin-support@coin3d.org
 *
\**************************************************************************/

/*!
  \class SoGeoCoordinate SoGeoCoordinate.h Inventor/nodes/SoGeoCoordinate.h
  \brief The SoGeoCoordinate class is used to specify a list of geographical coordinates.
  \ingroup nodes

  <b>FILE FORMAT/DEFAULTS:</b>
  \code
    GeoCoordinate {
      geoSystem ["GD", "WE"]
      point ""
    }
  \endcode

  \since Coin 2.5
*/

// *************************************************************************

#include <Inventor/nodes/SoGeoCoordinate.h>
#include <Inventor/nodes/SoGeoOrigin.h>
#include <Inventor/nodes/SoSubNodeP.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoGeoElement.h>
#include <Inventor/lists/SbList.h>

#include "SoGeo.h"

#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

// *************************************************************************

/*!
  \var SoSFString SoGeoCoordinate::point

  \sa SoGeoOrigin::geoSystem
*/

/*!
  \var SoMFString SoGeoCoordinate::geoSystem

  \sa SoGeoOrigin::geoSystem
*/


// *************************************************************************

class SoGeoCoordinateP {
public:
  uint32_t originid;
  uint32_t thisid;
  SbList <SbVec3f> coords;
};

#define PRIVATE(obj) obj->pimpl

// *************************************************************************

SO_NODE_SOURCE(SoGeoCoordinate);

/*!
  Constructor.
*/
SoGeoCoordinate::SoGeoCoordinate(void)
{
  PRIVATE(this) = new SoGeoCoordinateP;
  PRIVATE(this)->originid = 0;
  PRIVATE(this)->thisid = 0;

  SO_NODE_INTERNAL_CONSTRUCTOR(SoGeoCoordinate);

  SO_NODE_ADD_FIELD(point, (0.0, 0.0, 0.0));
  SO_NODE_ADD_FIELD(geoSystem, (""));

  this->geoSystem.setNum(2);
  this->geoSystem.set1Value(0, "GD");
  this->geoSystem.set1Value(1, "WE");
  this->geoSystem.setDefault(TRUE);
}

/*!
  Destructor.
*/
SoGeoCoordinate::~SoGeoCoordinate()
{
  delete PRIVATE(this);
}

// Doc from superclass.
void
SoGeoCoordinate::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoGeoCoordinate, SO_FROM_INVENTOR_1|SoNode::VRML1);
}

// Doc from superclass.
void
SoGeoCoordinate::doAction(SoAction * action)
{
  SoState * state = action->getState();

  if (origin->getNodeId() != PRIVATE(this)->originid ||
      this->getNodeId() != PRIVATE(this)->thisid) {

    if (PRIVATE(this)->originid != origin->getNodeId()) {
      this->touch();
    }
    PRIVATE(this)->originid = origin->getNodeId();
    PRIVATE(this)->thisid = this->getNodeId();

    PRIVATE(this)->coords.truncate(0);
    const int n = this->point.getNum();
    
    for (int i = 0; i < n; i++) {
      SbMatrix m = this->getTransform(origin, i);
      SbVec3f p(0.0f, 0.0f, 0.0f);
      m.multVecMatrix(p,p);
      PRIVATE(this)->coords.append(p);
    }
  }

  SoCoordinateElement::set3(state, this, PRIVATE(this)->coords.getLength(),
                            PRIVATE(this)->coords.getArrayPtr());
}

// Doc from superclass.
void
SoGeoCoordinate::GLRender(SoGLRenderAction * action)
{
  SoGeoCoordinate::doAction((SoAction *)action);
}

// Doc from superclass.
void
SoGeoCoordinate::getBoundingBox(SoGetBoundingBoxAction * action)
{
  SoGeoCoordinate::doAction((SoAction *)action);
}

// Doc from superclass.
void
SoGeoCoordinate::callback(SoCallbackAction * action)
{
  SoGeoCoordinate::doAction((SoAction *)action);
}

// Doc from superclass.
void
SoGeoCoordinate::pick(SoPickAction * action)
{
  SoGeoCoordinate::doAction((SoAction *)action);
}

// Doc from superclass.
void
SoGeoCoordinate::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  SoGeoCoordinate::doAction((SoAction *)action);
}

// *************************************************************************

SbMatrix
SoGeoCoordinate::getTransform(SoGeoOrigin * origin, const int idx) const
{
  return SoGeo::calculateTransform(origin->geoSystem.getValues(0),
                                   origin->geoSystem.getNum(),
                                   origin->geoCoords.getValue(),
                                   
                                   this->geoSystem.getValues(0),
                                   this->geoSystem.getNum(),
                                   this->point[idx]);
}
