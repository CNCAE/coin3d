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
  \class SoSFName SoSFName.h Inventor/fields/SoSFName.h
  \brief The SoSFName class is a container for an SbName.
  \ingroup fields

  This field is used where nodes, engines or other field containers
  needs to store a single name string.

  Fields of this type stores their value to file as a the name string
  within quotes.

  \sa SoMFName
*/

// *************************************************************************

#include <Inventor/fields/SoSFName.h>
#include <Inventor/fields/SoSubFieldP.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/errors/SoDebugError.h>

// *************************************************************************

SO_SFIELD_SOURCE(SoSFName, SbName, const SbName &);

// *************************************************************************

// Override from parent class.
void
SoSFName::initClass(void)
{
  SO_SFIELD_INTERNAL_INIT_CLASS(SoSFName);
}

// *************************************************************************

// No need to document readValue() and writeValue() here, as the
// necessary information is provided by the documentation of the
// parent classes.
#ifndef DOXYGEN_SKIP_THIS

SbBool
SoSFName::readValue(SoInput * in)
{
  // Reading a string instead of a name, because the name field value
  // is not necessarily a node name, node type or field name, and the
  // name might be quoted. The read value is just the value of a name
  // field.
  SbString s;
  SbBool ok = in->read(s);
  if (!ok) return FALSE;
  this->value = s;
  return TRUE;
}

// Write SbName value to output stream. Also used from SoMFName class.
void
sosfname_write_value(SoOutput * out, const SbName & val)
{
  out->write(val);
}

void
SoSFName::writeValue(SoOutput * out) const
{
  sosfname_write_value(out, this->getValue());
}

#endif // DOXYGEN_SKIP_THIS

// *************************************************************************

/*!
  Set value of field.
*/
void
SoSFName::setValue(const char * const name)
{
  this->setValue(SbName(name));
}

// *************************************************************************
