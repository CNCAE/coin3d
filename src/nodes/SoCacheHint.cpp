/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2004 by Systems in Motion.  All rights reserved.
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
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no/>.
 *
\**************************************************************************/

/*!
  \class SoCacheHint SoCacheHint.h Inventor/nodes/SoCacheHint.h
  \brief The SoCacheHint class is a node containing hints about how to cache geometry.
  \ingroup nodes

  The SoCacheHint node is used to set up clues to the rendering
  subsystem about how Coin should cache vertex data.

  Please note that this is an experimental class. The API might change
  a lot before/if it's included in any official Coin release.

*/

#include <Inventor/nodes/SoCacheHint.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/nodes/SoSubNodeP.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoCacheHintElement.h>


// *************************************************************************

SO_NODE_SOURCE(SoCacheHint);

/*!
  Constructor.
*/
SoCacheHint::SoCacheHint(void)
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoCacheHint);
  SO_NODE_ADD_FIELD(value, (0.5f));
}

/*!
  Destructor.
*/
SoCacheHint::~SoCacheHint()
{
}

// doc in super
void
SoCacheHint::initClass(void)
{
  //  SO_NODE_INTERNAL_INIT_CLASS(SoCacheHint, SO_FROM_COIN_2_4);
  SO_NODE_INTERNAL_INIT_CLASS(SoCacheHint, SO_FROM_COIN_2_3);
  
  SO_ENABLE(SoGLRenderAction, SoCacheHintElement);
}

void
SoCacheHint::doAction(SoAction * action)
{
  SoState * state = action->getState();
  SoCacheHintElement::set(state, this, this->value.getValue());
}

void
SoCacheHint::GLRender(SoGLRenderAction * action)
{
  SoCacheHint::doAction(action);
}

void
SoCacheHint::callback(SoCallbackAction * action)
{
  // do nothing
  SoNode::callback(action);
}

void
SoCacheHint::pick(SoPickAction * action)
{
  // do nothing
  SoNode::pick(action);
}

void
SoCacheHint::getBoundingBox(SoGetBoundingBoxAction * action)
{
  // do nothing
  SoNode::getBoundingBox(action);
}