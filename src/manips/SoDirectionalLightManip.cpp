/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2001 by Systems in Motion. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation. See the
 *  file LICENSE.LGPL at the root directory of the distribution for
 *  more details.
 *
 *  If you want to use Coin for applications not compatible with the
 *  LGPL, please contact SIM to acquire a Professional Edition license.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  http://www.sim.no support@sim.no Voice: +47 22114160 Fax: +47 22207097
 *
\**************************************************************************/

/*!
  \class SoDirectionalLightManip SoDirectionalLightManip.h Inventor/manips/SoDirectionalLightManip.h
  \brief The SoDirectionalLightManip class is used to manipulate directional light nodes.
  \ingroup manips

  A manipulator is used by replacing the node you want to edit in the
  graph with the manipulator. Draggers are used to manipulate the
  node. When manipulation is finished, the node is put back into the
  graph, replacing the manipulator.
*/

#include <Inventor/manips/SoDirectionalLightManip.h>
#include <Inventor/nodes/SoSubNodeP.h>
#include <Inventor/draggers/SoDirectionalLightDragger.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/SoNodeKitPath.h>

#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

SO_NODE_SOURCE(SoDirectionalLightManip);

/*!
  \var SoFieldSensor * SoDirectionalLightManip::directionFieldSensor
  \internal
*/
/*!
  \var SoFieldSensor * SoDirectionalLightManip::colorFieldSensor
  \internal
*/
/*!
  \var SoChildList * SoDirectionalLightManip::children
  \internal
*/


// documented in superclass
void
SoDirectionalLightManip::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoDirectionalLightManip);
}

/*!
  The constructor sets up the internal SoDiretionalLightDragger used
  by the manipulator.
*/
SoDirectionalLightManip::SoDirectionalLightManip(void)
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoDirectionalLightManip);

  this->children = new SoChildList(this);
  this->directionFieldSensor = new SoFieldSensor(SoDirectionalLightManip::fieldSensorCB, this);
  this->directionFieldSensor->setPriority(0);

  this->colorFieldSensor = new SoFieldSensor(SoDirectionalLightManip::fieldSensorCB, this);
  this->colorFieldSensor->setPriority(0);

  this->attachSensors(TRUE);
  this->setDragger(new SoDirectionalLightDragger);
}

/*!
  Destructor. Clean up resources used by manipulator.
*/
SoDirectionalLightManip::~SoDirectionalLightManip()
{
  this->setDragger(NULL);

  delete this->colorFieldSensor;
  delete this->directionFieldSensor;

  delete this->children;
}

/*!
  Set dragger to be used by this manipulator.
*/
void
SoDirectionalLightManip::setDragger(SoDragger * newdragger)
{
  SoDragger * olddragger = this->getDragger();
  if (olddragger) {
    olddragger->removeValueChangedCallback(SoDirectionalLightManip::valueChangedCB, this);
    this->children->remove(0);
  }
  if (newdragger != NULL) {
    if (this->children->getLength() > 0) {
      this->children->set(0, newdragger);
    }
    else {
      this->children->append(newdragger);
    }
    SoDirectionalLightManip::fieldSensorCB(this, NULL);
    newdragger->addValueChangedCallback(SoDirectionalLightManip::valueChangedCB, this);
  }
}

/*!
  Return pointer to internal SoDirectionalLightDragger dragger.
*/
SoDragger *
SoDirectionalLightManip::getDragger(void)
{
  if (this->children->getLength() > 0) {
    SoNode * node = (*children)[0];
    if (node->isOfType(SoDragger::getClassTypeId()))
      return (SoDragger *)node;
    else {
#if COIN_DEBUG
      SoDebugError::post("SoDirectionalLightManip::getDragger",
                         "Child is not a dragger!");
#endif // debug
    }
  }
  return NULL;
}

/*!
  Replaces the node specified by \a path with this manipulator.  The
  manipulator will copy the field data from the node, to make it
  affect the state in the same way as the node.
*/
SbBool
SoDirectionalLightManip::replaceNode(SoPath * path)
{
  SoFullPath * fullpath = (SoFullPath *)path;
  SoNode * fulltail = fullpath->getTail();
  if (!fulltail->isOfType(SoDirectionalLight::getClassTypeId())) {
#if COIN_DEBUG
    SoDebugError::post("SoDirectionalLightManip::replaceNode",
                       "End of path is not a SoDirectionalLight");
#endif // debug
    return FALSE;
  }
  SoNode * tail = path->getTail();
  if (tail->isOfType(SoBaseKit::getClassTypeId())) {
    SoBaseKit * kit = (SoBaseKit *) ((SoNodeKitPath *)path)->getTail();
    SbString partname = kit->getPartString(path);
    if (partname != "") {
      SoDirectionalLight * oldpart = (SoDirectionalLight *) kit->getPart(partname, TRUE);
      if (oldpart != NULL) {
        this->attachSensors(FALSE);
        this->transferFieldValues(oldpart, this);
        this->attachSensors(TRUE);
        SoDirectionalLightManip::fieldSensorCB(this, NULL);
        kit->setPart(partname, this);
        return TRUE;
      }
      else {
        return FALSE;
      }
    }
  }
  if (fullpath->getLength() < 2) {
#if COIN_DEBUG
    SoDebugError::post("SoDirectionalLightManip::replaceNode",
                       "Path is too short");
#endif // debug
    return FALSE;
  }
  SoNode * parent = fullpath->getNodeFromTail(1);
  if (!parent->isOfType(SoGroup::getClassTypeId())) {
#if COIN_DEBUG
    SoDebugError::post("SoDirectionalLightManip::replaceNode",
                       "Parent node is not a group");
#endif // debug
    return FALSE;
  }
  this->ref();
  this->attachSensors(FALSE);
  this->transferFieldValues((SoDirectionalLight *)fulltail, this);
  this->attachSensors(TRUE);
  SoDirectionalLightManip::fieldSensorCB(this, NULL);

  ((SoGroup *)parent)->replaceChild(fulltail, this);
  this->unrefNoDelete();
  return TRUE;
}

// documented in superclass
void
SoDirectionalLightManip::doAction(SoAction * action)
{
  int numindices;
  const int * indices;
  if (action->getPathCode(numindices, indices) == SoAction::IN_PATH) {
    this->children->traverseInPath(action, numindices, indices);
  }
  else {
    this->children->traverse(action);
  }
}

// documented in superclass
void
SoDirectionalLightManip::callback(SoCallbackAction * action)
{
  SoDirectionalLightManip::doAction(action);
  SoDirectionalLight::callback(action);
}

// documented in superclass
void
SoDirectionalLightManip::GLRender(SoGLRenderAction * action)
{
  SoDirectionalLightManip::doAction(action);
  SoDirectionalLight::GLRender(action);
}

// documented in superclass
void
SoDirectionalLightManip::getBoundingBox(SoGetBoundingBoxAction * action)
{
  int numindices;
  const int * indices;
  int lastchild;
  SbVec3f center(0.0f, 0.0f, 0.0f);
  int numcenters = 0;

  if (action->getPathCode(numindices, indices) == SoAction::IN_PATH) {
    lastchild  = indices[numindices-1];
  }
  else {
    lastchild = this->children->getLength() - 1;
  }
  for (int i = 0; i <= lastchild; i++) {
    this->children->traverse(action, i, i);
    if (action->isCenterSet()) {
      center += action->getCenter();
      numcenters++;
      action->resetCenter();
    }
  }
  SoDirectionalLight::getBoundingBox(action);
  if (action->isCenterSet()) {
    center += action->getCenter();
    numcenters++;
    action->resetCenter();
  }
  if (numcenters != 0) {
    action->setCenter(center / numcenters, FALSE);
  }
}

// documented in superclass
void
SoDirectionalLightManip::getMatrix(SoGetMatrixAction * action)
{
  int numindices;
  const int * indices;
  switch (action->getPathCode(numindices, indices)) {
  case SoAction::NO_PATH:
  case SoAction::BELOW_PATH:
    break;
  case SoAction::IN_PATH:
    this->children->traverseInPath(action, numindices, indices);
    break;
  case SoAction::OFF_PATH:
    this->children->traverse(action);
    break;
  default:
    assert(0 && "unknown path code");
    break;
  }
}

// documented in superclass
void
SoDirectionalLightManip::handleEvent(SoHandleEventAction * action)
{
  SoDirectionalLightManip::doAction(action);
  SoDirectionalLight::handleEvent(action);
}

// documented in superclass
void
SoDirectionalLightManip::pick(SoPickAction * action)
{
  SoDirectionalLightManip::doAction(action);
  SoDirectionalLight::pick(action);
}

// documented in superclass
void
SoDirectionalLightManip::search(SoSearchAction * action)
{
  inherited::search(action);
  if (action->isFound()) return;
  SoDirectionalLightManip::doAction(action);
}

/*!
  Returns the children of this node. This node only has the dragger as
  a child.
*/
SoChildList *
SoDirectionalLightManip::getChildren(void) const
{
  return this->children;
}

/*!
  \internal
  Convert from matrix to direction.
*/
void
SoDirectionalLightManip::valueChangedCB(void * m, SoDragger * dragger)
{
  SoDirectionalLightManip * thisp = (SoDirectionalLightManip *)m;

  SbMatrix matrix = dragger->getMotionMatrix();
  SbVec3f direction(0.0f, 0.0f, -1.0f);
  matrix.multDirMatrix(direction, direction);
  direction.normalize();

  thisp->attachSensors(FALSE);
  if (thisp->direction.getValue() != direction) {
    thisp->direction = direction;
  }
  thisp->attachSensors(TRUE);
}

/*!
  \internal
*/
void
SoDirectionalLightManip::fieldSensorCB(void * m, SoSensor *)
{
  SoDirectionalLightManip * thisp = (SoDirectionalLightManip *)m;
  SoDragger * dragger = thisp->getDragger();
  if (dragger != NULL) {
    SbVec3f direction = thisp->direction.getValue();
    SbMatrix matrix = dragger->getMotionMatrix();
    SbVec3f t, s;
    SbRotation r, so;
    matrix.getTransform(t, r, s, so);
    r.setValue(SbVec3f(0.0f, 0.0f, -1.0f), direction);
    matrix.setTransform(t, r, s, so);
    dragger->setMotionMatrix(matrix);

    SoMaterial * material = (SoMaterial *)dragger->getPart("material", TRUE);
    if (material->emissiveColor.getNum() != 1 ||
        material->emissiveColor[0].getValue() != thisp->color.getValue()) {
      // replace with new material since the material is reused between
      // all draggers.
      material = new SoMaterial;
      material->diffuseColor = SbColor(0.0f, 0.0f, 0.0f);
      material->emissiveColor = thisp->color.getValue();
      dragger->setPart("material", material);
    }
  }
}

/*!
  Overloaded to copy the dragger.
*/
void
SoDirectionalLightManip::copyContents(const SoFieldContainer * fromfc,
                                      SbBool copyconnections)
{
  assert(fromfc->isOfType(SoDirectionalLightManip::getClassTypeId()));
  this->setDragger(((SoDirectionalLightManip *)fromfc)->getDragger());
  inherited::copyContents(fromfc, copyconnections);
}

/*!
  Copies field values.
*/
void
SoDirectionalLightManip::transferFieldValues(const SoDirectionalLight * from,
                                             SoDirectionalLight * to)
{
  to->direction = from->direction;
  to->color = from->color;
}

void
SoDirectionalLightManip::attachSensors(const SbBool onoff)
{
  if (onoff) {
    this->directionFieldSensor->attach(&this->direction);
    this->colorFieldSensor->attach(&this->color);
  }
  else {
    this->directionFieldSensor->detach();
    this->colorFieldSensor->detach();
  }
}
