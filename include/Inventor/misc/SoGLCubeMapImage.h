#ifndef COIN_SOGLCUBEMAPIMAGE_H
#define COIN_SOGLCUBEMAPIMAGE_H

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

#include <Inventor/SbBasic.h>
#include <Inventor/misc/SoGLImage.h>

class COIN_DLL_API SoGLCubeMapImage : public SoGLImage {
  typedef SoGLImage inherited;

public:

  SoGLCubeMapImage();
  virtual void unref(SoState * state = NULL);

  static SoType getClassTypeId(void);
  virtual SoType getTypeId(void) const ;

  enum Target {
    POSITIVE_X = 0,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z
  };

  void setCubeMapImage(const Target target,
                       const unsigned char * bytes,
                       const SbVec2s & size,
                       const int numcomponents);
  
  virtual void setData(const SbImage * image,
                       const Wrap wraps = REPEAT,
                       const Wrap wrapt = REPEAT,
                       const float quality = 0.5f,
                       const int border = 0,
                       SoState * createinstate = NULL);

  virtual void setData(const SbImage * image,
                       const Wrap wraps,
                       const Wrap wrapt,
                       const Wrap wrapr,
                       const float quality = 0.5f,
                       const int border = 0,
                       SoState * createinstate = NULL);

  virtual SoGLDisplayList * getGLDisplayList(SoState * state);


 public:
  static void initClass(void);

 private:
  virtual ~SoGLCubeMapImage();

  class SoGLCubeMapImageP * pimpl;
  friend class SoGLCubeMapImageP;
  static void cleanupClass(void);
};

#endif // !COIN_SOGLCUBEMAPIMAGE_H
