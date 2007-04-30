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

#include <Inventor/SbVec3i32.h>

#include <Inventor/SbVec3ui32.h>
#include <Inventor/SbVec3b.h>
#include <Inventor/SbVec3s.h>

/*!
  \class SbVec3i32 Inventor/SbVec3i32.h

  \since 2007-04-28
*/

SbVec3i32 &
SbVec3i32::setValue(const SbVec3ui32 & v)
{
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  vec[2] = static_cast<int32_t>(v[2]);
  return *this;
}

SbVec3i32 &
SbVec3i32::setValue(const SbVec3b & v)
{
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  vec[2] = static_cast<int32_t>(v[2]);
  return *this;
}

SbVec3i32 &
SbVec3i32::setValue(const SbVec3s & v)
{
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  vec[2] = static_cast<int32_t>(v[2]);
  return *this;
}

SbVec3i32 &
SbVec3i32::operator *= (double d)
{
  vec[0] = static_cast<int32_t>(vec[0] * d);
  vec[1] = static_cast<int32_t>(vec[1] * d);
  vec[2] = static_cast<int32_t>(vec[2] * d);
  return *this;
}
