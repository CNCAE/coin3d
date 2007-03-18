/* include/setup.h.  Generated from setup.h.in by configure.  */
/***************************************************************************
 * setup.h
 *
 * This file is a collection of the user-tunable parameters for Coin.
 * You can edit the setting of the below defines to alter how Coin behaves.
 *
 * Note that this file comes from a template file called "setup.h.in".
 * Editing the template file does nothing - make sure you are editing the
 * file "setup.h".
 */

/***************************************************************************
 * HAVE_VRML97
 *
 * If you want to disable VRML97 suport in Coin, do not set this define.
 * The reason to disable this is mostly to avoid compiling a lot of code
 * that causes the Coin library to grow considerably if you need a
 * particularly small version of Coin.
 *
 * Default is to have VRML97 support enabled.
 */

#define HAVE_VRML97 

/***************************************************************************
 * HAVE_SOUND
 *
 * Define this if you want Coin to have sound support.  You will need
 * OpenAL on the run-time system as well for sound to work.
 *
 * Default is to have sound enabled.
 */

#define HAVE_SOUND 

/***************************************************************************
 * COIN_THREADSAFE
 *
 * If you want to enable extra code in Coin that makes render traversals
 * multi-thread safe, enable this define.  This does not make the whole of
 * Coin thread safe - just doing parallel render traversals on multipipe
 * systems.
 *
 * Default is to have thread safety code disabled.  It is disabled because
 * it costs a lot of overhead in the most common cases where it is not
 * necessary.
 */

/* #undef COIN_THREADSAFE */

/***************************************************************************
 * HAVE_3DS_IMPORT_CAPABILITIES
 *
 * If you want to be able to import .3ds files directly into Coin, set this
 * define.
 *
 * Default is to not have this define set, as the import code has not been
 * thoroughty tested and robustified by the main Coin developers yet.
 */

#define HAVE_3DS_IMPORT_CAPABILITIES 

/***************************************************************************
 * COIN_HAVE_JAVASCRIPT
 *
 * All the SpiderMonkey javascript code is wrapped in this
 * define. This is done to make sure the code in Coin-2 and Coin-dev
 * is as equal as possible, making it easier to port fixes and
 * enchancements.
 *
 * Default is to have this define set
 */

#define COIN_HAVE_JAVASCRIPT 

/***************************************************************************
 * The end.
 *
 * Make sure this header file is not installed, and only used when building
 * Coin itself.
 */

#ifndef COIN_INTERNAL
#error this is a private header file
#endif

