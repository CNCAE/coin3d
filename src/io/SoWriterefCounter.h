#ifndef COIN_SOWRITEREFCOUNTER_H
#define COIN_SOWRITEREFCOUNTER_H

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

class SoWriterefCounterP;
class SoOutput;
class SoBase;
class SbString;

#ifndef COIN_INTERNAL
#error this is a private header file
#endif

#include <Inventor/SbBasic.h>
#include <Inventor/SbName.h>

class SoWriterefCounter {
public:
  static void initClass(void);
  
  static void setInstancePrefix(const SbString & s);
  
  static void create(SoOutput * out, SoOutput * copyfrom);
  static void destruct(SoOutput * out);

  static SoWriterefCounter * instance(SoOutput * out);

  SbBool shouldWrite(const SoBase * base) const;
  SbName getWriteName(const SoBase * base) const;
  SbBool hasMultipleWriteRefs(const SoBase * base) const;
  int getWriteref(const SoBase * base) const;
  void setWriteref(const SoBase * base, const int ref);
  void removeWriteref(const SoBase * base);
  void decrementWriteref(const SoBase * base);
  
  SbBool isInGraph(const SoBase * base) const;
  void setInGraph(const SoBase * base, const SbBool ingraph);
  void debugCleanup(void);
  
  enum RefId {
    // Reference id if we don't need to add a suffix to the node name
    NOSUFFIX = -2,
    // Reference id if no DEF instance of a node is written yet
    FIRSTWRITE = -1
  };

  int addReference(const SoBase * base);
  int findReference(const SoBase * base) const;
  void setReference(const SoBase * base, int refid);
  void removeSoBase2IdRef(const SoBase * base);

  static SbBool debugWriterefs(void);

protected:
  SoWriterefCounter(SoOutput * out, SoOutput * copyfrom);
  ~SoWriterefCounter();

private:
  void cleanup(void);

  SoWriterefCounterP * pimpl;

};

#endif // COIN_SOWRITERREFCOUNTER_H
