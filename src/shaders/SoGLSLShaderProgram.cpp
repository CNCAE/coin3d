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

#include "SoGLSLShaderProgram.h"

#include "SoGLSLShaderObject.h"

// *************************************************************************

// FIXME: no checking is done to see whether "shader objects" (as for
// GL_ARB_shader_objects) is actually supported or not. 20050124 mortene.

// *************************************************************************

SoGLSLShaderProgram::SoGLSLShaderProgram(void)
{
  this->programHandle = 0;
  this->isExecutable = FALSE;
}

SoGLSLShaderProgram::~SoGLSLShaderProgram()
{
}

void
SoGLSLShaderProgram::addShaderObject(SoGLSLShaderObject *shaderObject)
{
  if (shaderObject!=NULL) {
    if (this->indexOfShaderObject(shaderObject) < 0) {
      this->shaderObjects.append(shaderObject);
    }
  }
}
void
SoGLSLShaderProgram::removeShaderObjects(void)
{
  this->shaderObjects.truncate(0);  
}

void
SoGLSLShaderProgram::enable(const cc_glglue * g)
{
  this->ensureLinking(g);

  if (this->isExecutable) {
    g->glUseProgramObjectARB(this->programHandle);
  }
}

void
SoGLSLShaderProgram::disable(const cc_glglue * g)
{
  if (this->isExecutable) {
    g->glUseProgramObjectARB(0);
  }
}

#if defined(SOURCE_HINT)
SbString
SoGLSLShaderProgram::getSourceHint(void) const
{
  SbString result; 
  for (int i=0; i<this->shaderObjects.size(); i++) {
    SoGLSLShaderObject *shader = this->shaderObjects[i];
    if (shader && shader->isActive()) { 
      SbString str = shader->sourceHint;
      if (str.getLength() > 0) str += " ";
      result += str;
    }
  }
  return result;
}
#endif

void
SoGLSLShaderProgram::ensureLinking(const cc_glglue * g)
{
  SbBool shouldlink = FALSE;
  for (int i = 0; i < this->shaderObjects.getLength() && !shouldlink; i++) {
    if (!this->shaderObjects[i]->isAttached()) shouldlink = TRUE;
  }

  if (!shouldlink) return;

  this->isExecutable = FALSE;
  this->ensureProgramHandle(g);

  int cnt = this->shaderObjects.getLength();

  if (cnt > 0) {
    GLint didLink;

    for (int i=0; i<cnt; i++) 
      this->shaderObjects[i]->attach(this->programHandle);

    g->glLinkProgramARB(this->programHandle);

    if (SoGLSLShaderObject::didOpenGLErrorOccur(0)) {
      SoGLSLShaderObject::printInfoLog(g, this->programHandle, 0);
    }
    g->glGetObjectParameterivARB(this->programHandle,
                                 GL_OBJECT_LINK_STATUS_ARB,&didLink);

    this->isExecutable = didLink;
  }
}

int
SoGLSLShaderProgram::indexOfShaderObject(SoGLSLShaderObject *shaderObject)
{
  if (shaderObject == NULL) return -1;

  int cnt = this->shaderObjects.getLength();
  for (int i=0; i<cnt; i++) {
    if (shaderObject == this->shaderObjects[i]) return i;
  }
  return -1;
}

void
SoGLSLShaderProgram::ensureProgramHandle(const cc_glglue * g)
{
  if (this->programHandle == 0) {
    this->programHandle = g->glCreateProgramObjectARB();
  }
}
