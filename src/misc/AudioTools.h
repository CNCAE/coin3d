#ifndef COIN_AUDIOTOOLS_H
#define COIN_AUDIOTOOLS_H

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

#ifndef COIN_INTERNAL
#error this is a private header file
#endif /* !COIN_INTERNAL */

class SbString;

const char * coin_get_openal_error(int errcode);
int coin_debug_audio(void);

#define SOUND_NOT_ENABLED_BY_DEFAULT_STRING \
        "The main reason for considering sound on this platform " \
        "experimental is that we have encountered various problems with " \
        "OpenAL (www.openal.org) on platforms other than Win32. If you " \
        "still want to use sound in Coin, please consider getting the " \
        "latest version of OpenAL from cvs only if you have " \
        "problems. Common problems are stuttering sound and the " \
        "occasional crash. If you run into problems, please try running " \
        "the various test-programs that comes with the OpenAL " \
        "distribution. Specifically, try running the " \
        "linux/test/teststream.c sample and verify that everything " \
        "sounds OK. "


#endif // COIN_AUDIOTOOLS_H
