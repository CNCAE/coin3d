/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2000 by Systems in Motion. All rights reserved.
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

// TODO: Skei: Clean up everything.... it's quite messy at the moment... :-/

/*!
  \class SoMarkerSet SoMarkerSet.h Inventor/nodes/SoMarkerSet.h
  \brief The SoMarkerSet class ...
  \ingroup nodes

  FIXME: write class doc
*/

#include <Inventor/nodes/SoMarkerSet.h>
#include <Inventor/nodes/SoSubNodeP.h>
#include <coindefs.h> // COIN_STUB()

#include <Inventor/misc/SoState.h>

#include <Inventor/bundles/SoTextureCoordinateBundle.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoGLRenderAction.h>
#ifdef _WIN32
#include <windows.h>
#endif // !_WIN32
#include <GL/gl.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/actions/SoGetPrimitiveCountAction.h>
#include <Inventor/elements/SoGLCoordinateElement.h>
#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/elements/SoGLLightModelElement.h>
#include <Inventor/caches/SoNormalCache.h>
#include <Inventor/details/SoPointDetail.h>
#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>


/*!
  \enum SoMarkerSet::MarkerType
  FIXME: write documentation for enum
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CROSS_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::PLUS_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::MINUS_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SLASH_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::BACKSLASH_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::BAR_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::STAR_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::Y_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::LIGHTNING_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::WELL_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CIRCLE_LINE_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SQUARE_LINE_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::DIAMOND_LINE_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::TRIANGLE_LINE_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::RHOMBUS_LINE_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::HOURGLASS_LINE_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SATELLITE_LINE_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::PINE_TREE_LINE_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CAUTION_LINE_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SHIP_LINE_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CIRCLE_FILLED_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SQUARE_FILLED_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::DIAMOND_FILLED_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::TRIANGLE_FILLED_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::RHOMBUS_FILLED_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::HOURGLASS_FILLED_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SATELLITE_FILLED_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::PINE_TREE_FILLED_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CAUTION_FILLED_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SHIP_FILLED_5_5
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CROSS_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::PLUS_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::MINUS_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SLASH_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::BACKSLASH_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::BAR_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::STAR_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::Y_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::LIGHTNING_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::WELL_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CIRCLE_LINE_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SQUARE_LINE_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::DIAMOND_LINE_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::TRIANGLE_LINE_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::RHOMBUS_LINE_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::HOURGLASS_LINE_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SATELLITE_LINE_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::PINE_TREE_LINE_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CAUTION_LINE_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SHIP_LINE_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CIRCLE_FILLED_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SQUARE_FILLED_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::DIAMOND_FILLED_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::TRIANGLE_FILLED_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::RHOMBUS_FILLED_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::HOURGLASS_FILLED_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SATELLITE_FILLED_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::PINE_TREE_FILLED_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CAUTION_FILLED_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SHIP_FILLED_7_7
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CROSS_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::PLUS_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::MINUS_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SLASH_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::BACKSLASH_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::BAR_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::STAR_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::Y_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::LIGHTNING_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::WELL_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CIRCLE_LINE_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SQUARE_LINE_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::DIAMOND_LINE_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::TRIANGLE_LINE_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::RHOMBUS_LINE_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::HOURGLASS_LINE_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SATELLITE_LINE_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::PINE_TREE_LINE_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CAUTION_LINE_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SHIP_LINE_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CIRCLE_FILLED_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SQUARE_FILLED_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::DIAMOND_FILLED_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::TRIANGLE_FILLED_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::RHOMBUS_FILLED_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::HOURGLASS_FILLED_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SATELLITE_FILLED_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::PINE_TREE_FILLED_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::CAUTION_FILLED_9_9
  FIXME: write documentation for enum definition
*/
/*!
  \var SoMarkerSet::MarkerType SoMarkerSet::SHIP_FILLED_9_9
  FIXME: write documentation for enum definition
*/


/*!
  \var SoMFInt32 SoMarkerSet::markerIndex
  FIXME: write documentation for field
*/


// *************************************************************************

SO_NODE_SOURCE(SoMarkerSet);

/*!
  Constructor.
*/
SoMarkerSet::SoMarkerSet()
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoMarkerSet);
  SO_NODE_ADD_FIELD(markerIndex, (0));
}

/*!
  Destructor.
*/
SoMarkerSet::~SoMarkerSet()
{
}

// ----------------------------------------------------------------------------------------------------

// static GLubyte markerimages[90*9*4];
GLubyte *markerimages;

void convert_bitmaps(void);
void free_marker_images(void);

// ------------

void free_marker_images(void)
{
	delete markerimages;	
}

/*!
  Does initialization common for all objects of the
  SoMarkerSet class. This includes setting up the
  type system, among other things.
*/
void
SoMarkerSet::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoMarkerSet);
  markerimages = new GLubyte[90*9*4];  // hardcoded 90 markers, 9x9 bitmaps, dword memory alignment
  atexit(free_marker_images);
  convert_bitmaps();
}

// -----

// copy/paste fra pointset...

// Internal method which translates the current material binding
// found on the state to a material binding for this node.
// PER_PART, PER_FACE, PER_VERTEX and their indexed counterparts
// are translated to PER_VERTEX binding. OVERALL means overall
// binding for point set also, of course. The default material
// binding is OVERALL.
SoMarkerSet::Binding
SoMarkerSet::findMaterialBinding(SoState * const state) const
{
  Binding binding = OVERALL;
  if (SoMaterialBindingElement::get(state) !=
      SoMaterialBindingElement::OVERALL) binding = PER_VERTEX;
  return binding;
}

//  Internal method which translates the current normal binding
//  found on the state to a normal binding for this node.
//  PER_PART, PER_FACE, PER_VERTEX and their indexed counterparts
//  are translated to PER_VERTEX binding. OVERALL means overall
//  binding for point set also, of course. The default normal
//  binding is PER_VERTEX.
SoMarkerSet::Binding
SoMarkerSet::findNormalBinding(SoState * const state) const

{
  Binding binding = PER_VERTEX;

  if (SoNormalBindingElement::get(state) ==
      SoNormalBindingElement::OVERALL) binding = OVERALL;
  return binding;
}

static char marker_char_bitmaps[] =
{
  // CROSS_5_5
  "         "
  "         "
  "  x   x  "
  "   x x   "
  "    x    "
  "   x x   "
  "  x   x  "
  "         "
  "         "
  // PLUS_5_5
  "         "
  "         "
  "    x    "
  "    x    "
  "  xxxxx  "
  "    x    "
  "    x    "
  "         "
  "         "
  // MINUS_5_5
  "         "
  "         "
  "         "
  "         "
  "  xxxxx  "
  "         "
  "         "
  "         "
  "         "
  // SLASH_5_5
  "         "
  "         "
  "      x  "
  "     x   "
  "    x    "
  "   x     "
  "  x      "
  "         "
  "         "
  // BACKSLASH_5_5
  "         "
  "         "
  "  x      "
  "   x     "
  "    x    "
  "     x   "
  "      x  "
  "         "
  "         "
  // BAR_5_5
  "         "
  "         "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "         "
  "         "
  // STAR_5_5
  "         "
  "         "
  "  x x x  "
  "   xxx   "
  "  xxxxx  "
  "   xxx   "
  "  x x x  "
  "         "
  "         "
  // Y_5_5
  "         "
  "         "
  "  x   x  "
  "   x x   "
  "    x    "
  "    x    "
  "    x    "
  "         "
  "         "
  // LIGHTNING_5_5
  "         "
  "         "
  "    x    "
  "     x   "
  "  xxxxx  "
  "   x     "
  "    x    "
  "         "
  "         "
  // WELL_5_5
  "         "
  "         "
  "    x    "
  "    x    "
  "   x x   "
  "   xxx   "
  "  x   x  "
  "         "
  "         "
  // CIRCLE_LINE_5_5
  "         "
  "         "
  "   xxx   "
  "  x   x  "
  "  x   x  "
  "  x   x  "
  "   xxx   "
  "         "
  "         "
  // SQUARE_LINE_5_5
  "         "
  "         "
  "  xxxxx  "
  "  x   x  "
  "  x   x  "
  "  x   x  "
  "  xxxxx  "
  "         "
  "         "
  // DIAMOND_LINE_5_5
  "         "
  "         "
  "    x    "
  "   x x   "
  "  x   x  "
  "   x x   "
  "    x    "
  "         "
  "         "
  // TRIANGLE_LINE_5_5
  "         "
  "         "
  "    x    "
  "    x    "
  "   x x   "
  "   x x   "
  "  xxxxx  "
  "         "
  "         "
  // RHOMBUS_LINE_5_5
  "         "
  "         "
  "         "
  "   xxxxx "
  "  x   x  "
  " xxxxx   "
  "         "
  "         "
  "         "
  // HOURGLASS_LINE_5_5
  "         "
  "         "
  "  xxxxx  "
  "   x x   "
  "    x    "
  "   x x   "
  "  xxxxx  "
  "         "
  "         "
  // SATELLITE_LINE_5_5
  "         "
  "         "
  "  x   x  "
  "   xxx   "
  "   x x   "
  "   xxx   "
  "  x   x  "
  "         "
  "         "
  // PINE_TREE_LINE_5_5
  "         "
  "         "
  "    x    "
  "   x x   "
  "  xxxxx  "
  "    x    "
  "    x    "
  "         "
  "         "
  // CAUTION_LINE_5_5
  "         "
  "         "
  "  xxxxx  "
  "   x x   "
  "   x x   "
  "    x    "
  "    x    "
  "         "
  "         "
  // SHIP_LINE_5_5
  "         "
  "         "
  "    x    "
  "    x    "
  "  xxxxx  "
  "   x x   "
  "    x    "
  "         "
  "         "
  // CIRCLE_FILLED_5_5
  "         "
  "         "
  "   xxx   "
  "  xxxxx  "
  "  xxxxx  "
  "  xxxxx  "
  "   xxx   "
  "         "
  "         "
  // SQUARE_FILLED_5_5
  "         "
  "         "
  "  xxxxx  "
  "  xxxxx  "
  "  xxxxx  "
  "  xxxxx  "
  "  xxxxx  "
  "         "
  "         "
  // DIAMOND_FILLED_5_5
  "         "
  "         "
  "    x    "
  "   xxx   "
  "  xxxxx  "
  "   xxx   "
  "    x    "
  "         "
  "         "
  // TRIANGLE_FILLED_5_5
  "         "
  "         "
  "    x    "
  "    x    "
  "   xxx   "
  "   xxx   "
  "  xxxxx  "
  "         "
  "         "
  // RHOMBUS_FILLED_5_5
  "         "
  "         "
  "         "
  "   xxxxx "
  "  xxxxx  "
  " xxxxx   "
  "         "
  "         "
  "         "
  // HOURGLASS_FILLED_5_5
  "         "
  "         "
  "  xxxxx  "
  "   xxx   "
  "    x    "
  "   xxx   "
  "  xxxxx  "
  "         "
  "         "
  // SATELLITE_FILLED_5_5
  "         "
  "         "
  "  x   x  "
  "   xxx   "
  "   xxx   "
  "   xxx   "
  "  x   x  "
  "         "
  "         "
  // PINE_TREE_FILLED_5_5
  "         "
  "         "
  "    x    "
  "   xxx   "
  "  xxxxx  "
  "    x    "
  "    x    "
  "         "
  "         "
  // CAUTION_FILLED_5_5
  "         "
  "         "
  "  xxxxx  "
  "   xxx   "
  "   xxx   "
  "    x    "
  "    x    "
  "         "
  "         "
  // SHIP_FILLED_5_5
  "         "
  "         "
  "    x    "
  "    x    "
  "  xxxxx  "
  "   xxx   "
  "    x    "
  "         "
  "         "
  // CROSS_7_7
  "         "
  " x     x "
  "  x   x  "
  "   x x   "
  "    x    "
  "   x x   "
  "  x   x  "
  " x     x "
  "         "
  // PLUS_7_7
  "         "
  "    x    "
  "    x    "
  "    x    "
  " xxxxxxx "
  "    x    "
  "    x    "
  "    x    "
  "         "
  // MINUS_7_7
  "         "
  "         "
  "         "
  "         "
  " xxxxxxx "
  "         "
  "         "
  "         "
  "         "
  // SLASH_7_7
  "         "
  "       x "
  "      x  "
  "     x   "
  "    x    "
  "   x     "
  "  x      "
  " x       "
  "         "
  // BACKSLASH_7_7
  "         "
  " x       "
  "  x      "
  "   x     "
  "    x    "
  "     x   "
  "      x  "
  "       x "
  "         "
  // BAR_7_7,
  "         "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "         "
  // STAR_7_7
  "         "
  "    x    "
  "  x x x  "
  "   xxx   "
  " xxxxxxx "
  "   xxx   "
  "  x x x  "
  "    x    "
  "         "
  // Y_7_7
  "         "
  " x     x "
  "  x   x  "
  "   x x   "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "         "
  // LIGHTNING_7_7
  "         "
  "    x    "
  "     x   "
  "      x  "
  " xxxxxxx "
  "  x      "
  "   x     "
  "    x    "
  "         "
  // WELL_7_7
  "         "
  "    x    "
  "   x x   "
  "   xxx   "
  "  xx xx  "
  "  x x x  "
  "  xx xx  "
  " xx   xx "
  "         "
  // CIRCLE_LINE_7_7
  "         "
  "  xxxxx  "
  " x     x "
  " x     x "
  " x     x "
  " x     x "
  " x     x "
  "  xxxxx  "
  "         "
  // SQUARE_LINE_7_7
  "         "
  " xxxxxxx "
  " x     x "
  " x     x "
  " x     x "
  " x     x "
  " x     x "
  " xxxxxxx "
  "         "
  // DIAMOND_LINE_7_7
  "         "
  "    x    "
  "   x x   "
  "  x   x  "
  " x     x "
  "  x   x  "
  "   x x   "
  "    x    "
  "         "
  // TRIANGLE_LINE_7_7
  "         "
  "    x    "
  "    x    "
  "   x x   "
  "   x x   "
  "  x   x  "
  "  x   x  "
  " xxxxxxx "
  "         "
  // RHOMBUS_LINE_7_7
  "         "
  "         "
  "    xxxxx"
  "   x   x "
  "  x   x  "
  " x   x   "
  "xxxx     "
  "         "
  "         "
  // HOURGLASS_LINE_7_7
  "         "
  " xxxxxxx "
  "  x   x  "
  "   x x   "
  "    x    "
  "   x x   "
  "  x   x  "
  " xxxxxxx "
  "         "
  // SATELLITE_LINE_7_7
  "         "
  " x     x "
  "  x x x  "
  "   x x   "
  "  x   x  "
  "   x x   "
  "  x x x  "
  " x     x "
  "         "
  // PINE_TREE_LINE_7_7
  "         "
  "    x    "
  "   x x   "
  "  x   x  "
  " xxxxxxx "
  "    x    "
  "    x    "
  "    x    "
  "         "
  // CAUTION_LINE_7_7
  "         "
  " xxxxxxx "
  "  x   x  "
  "  x   x  "
  "   x x   "
  "   x x   "
  "    x    "
  "    x    "
  "         "
  // SHIP_LINE_7_7
  "         "
  "    x    "
  "    x    "
  "    x    "
  " xxxxxxx "
  "  x   x  "
  "   x x   "
  "    x    "
  "         "
  // CIRCLE_FILLED_7_7
  "         "
  "   xxx   "
  "  xxxxx  "
  " xxxxxxx "
  " xxxxxxx "
  " xxxxxxx "
  "  xxxxx  "
  "   xxx   "
  "         "
  // SQUARE_FILLED_7_7
  "         "
  " xxxxxxx "
  " xxxxxxx "
  " xxxxxxx "
  " xxxxxxx "
  " xxxxxxx "
  " xxxxxxx "
  " xxxxxxx "
  "         "
  // DIAMOND_FILLED_7_7
  "         "
  "    x    "
  "   xxx   "
  "  xxxxx  "
  " xxxxxxx "
  "  xxxxx  "
  "   xxx   "
  "    x    "
  "         "
  // TRIANGLE_FILLED_7_7
  "         "
  "    x    "
  "    x    "
  "   xxx   "
  "   xxx   "
  "  xxxxx  "
  "  xxxxx  "
  " xxxxxxx "
  "         "
  // RHOMBUS_FILLED_7_7
  "         "
  "         "
  "    xxxxx"
  "   xxxxx "
  "  xxxxx  "
  " xxxxx   "
  "xxxxx    "
  "         "
  "         "
  // HOURGLASS_FILLED_7_7
  "         "
  " xxxxxxx "
  "  xxxxx  "
  "   xxx   "
  "    x    "
  "   xxx   "
  "  xxxxx  "
  " xxxxxxx "
  "         "
  // SATELLITE_FILLED_7_7
  "         "
  " x     x "
  "  x x x  "
  "   xxx   "
  "  xxxxx  "
  "   xxx   "
  "  x x x  "
  " x     x "
  "         "
  // PINE_TREE_FILLED_7_7
  "         "
  "    x    "
  "   xxx   "
  "  xxxxx  "
  " xxxxxxx "
  "    x    "
  "    x    "
  "    x    "
  "         "
  // CAUTION_FILLED_7_7
  "         "
  " xxxxxxx "
  "  xxxxx  "
  "  xxxxx  "
  "   xxx   "
  "   xxx   "
  "    x    "
  "    x    "
  "         "
  // SHIP_FILLED_7_7
  "         "
  "    x    "
  "    x    "
  "    x    "
  " xxxxxxx "
  "  xxxxx  "
  "   xxx   "
  "    x    "
  "         "
  // CROSS_9_9
  "x       x"
  " x     x "
  "  x   x  "
  "   x x   "
  "    x    "
  "   x x   "
  "  x   x  "
  " x     x "
  "x       x"
  // PLUS_9_9
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "xxxxxxxxx"
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  // MINUS_9_9
  "         "
  "         "
  "         "
  "         "
  "xxxxxxxxx"
  "         "
  "         "
  "         "
  "         "
  // SLASH_9_9
  "        x"
  "       x "
  "      x  "
  "     x   "
  "    x    "
  "   x     "
  "  x      "
  " x       "
  "x        "
  // BACKSLASH_9_9
  "x        "
  " x       "
  "  x      "
  "   x     "
  "    x    "
  "     x   "
  "      x  "
  "       x "
  "        x"
  // BAR_9_9,
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  // STAR_9_9
  "    x    "
  " x  x  x "
  "  x x x  "
  "   xxx   "
  "xxxxxxxxx"
  "   xxx   "
  "  x x x  "
  " x  x  x "
  "    x    "
  // Y_9_9
  "x       x"
  " x     x "
  "  x   x  "
  "   x x   "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  // LIGHTNING_9_9
  "    x    "
  "     x   "
  "      x  "
  "       x "
  "xxxxxxxxx"
  " x       "
  "  x      "
  "   x     "
  "    x    "
  // WELL_9_9
  "    x    "
  "   xxx   "
  "   x x   "
  "   x x   "
  "  x x x  "
  "  xxxxx  "
  "  x x x  "
  " x x x x "
  "xxx   xxx"
  // CIRCLE_LINE_9_9
  "   xxx   "
  " xx   xx "
  " x     x "
  "x       x"
  "x       x"
  "x       x"
  " x     x "
  " xx   xx "
  "   xxx   "
  // SQUARE_LINE_9_9
  "xxxxxxxxx"
  "x       x"
  "x       x"
  "x       x"
  "x       x"
  "x       x"
  "x       x"
  "x       x"
  "xxxxxxxxx"
  // DIAMOND_LINE_9_9
  "    x    "
  "   x x   "
  "  x   x  "
  " x     x "
  "x       x"
  " x     x "
  "  x   x  "
  "   x x   "
  "    x    "
  // TRIANGLE_LINE_9_9
  "    x    "
  "    x    "
  "   x x   "
  "   x x   "
  "  x   x  "
  "  x   x  "
  " x     x "
  " x     x "
  "xxxxxxxxx"
  // RHOMBUS_LINE_9_9
  "         "
  "     xxxx"
  "    x   x"
  "   x   x "
  "  x   x  "
  " x   x   "
  "x   x    "
  "xxxx     "
  "         "
  // HOURGLASS_LINE_9_9
  "xxxxxxxxx"
  " x     x "
  "  x   x  "
  "   x x   "
  "    x    "
  "   x x   "
  "  x   x  "
  " x     x "
  "xxxxxxxxx"
  // SATELLITE_LINE_9_9
  "x       x"
  " x xxx x "
  "  x   x  "
  " x     x "
  " x     x "
  " x     x "
  "  x   x  "
  " x xxx x "
  "x       x"
  // PINE_TREE_LINE_9_9
  "    x    "
  "   x x   "
  "  x   x  "
  " x     x "
  "xxxxxxxxx"
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  // CAUTION_LINE_9_9
  "xxxxxxxxx"
  " x     x "
  " x     x "
  "  x   x  "
  "  x   x  "
  "   x x   "
  "   x x   "
  "    x    "
  "    x    "
  // SHIP_LINE_9_9
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "xxxxxxxxx"
  " x     x "
  "  x   x  "
  "   x x   "
  "    x    "
  // CIRCLE_FILLED_9_9
  "   xxx   "
  " xxxxxxx "
  " xxxxxxx "
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  " xxxxxxx "
  " xxxxxxx "
  "   xxx   "
  // SQUARE_FILLED_9_9
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  // DIAMOND_FILLED_9_9
  "    x    "
  "   xxx   "
  "  xxxxx  "
  " xxxxxxx "
  "xxxxxxxxx"
  " xxxxxxx "
  "  xxxxx  "
  "   xxx   "
  "    x    "
  // TRIANGLE_FILLED_9_9
  "    x    "
  "    x    "
  "   xxx   "
  "   xxx   "
  "  xxxxx  "
  "  xxxxx  "
  " xxxxxxx "
  " xxxxxxx "
  "xxxxxxxxx"
  // RHOMBUS_FILLED_9_9
  "         "
  "     xxxx"
  "    xxxxx"
  "   xxxxx "
  "  xxxxx  "
  " xxxxx   "
  "xxxxx    "
  "xxxx     "
  "         "
  // HOURGLASS_FILLED_9_9
  "xxxxxxxxx"
  " xxxxxxx "
  "  xxxxx  "
  "   xxx   "
  "    x    "
  "   xxx   "
  "  xxxxx  "
  " xxxxxxx "
  "xxxxxxxxx"
  // SATELLITE_FILLED_9_9
  "x       x"
  " x xxx x "
  "  xxxxx  "
  " xxxxxxx "
  " xxxxxxx "
  " xxxxxxx "
  "  xxxxx  "
  " x xxx x "
  "x       x"
  // PINE_TREE_FILLED_9_9
  "    x    "
  "   xxx   "
  "  xxxxx  "
  " xxxxxxx "
  "xxxxxxxxx"
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  // CAUTION_FILLED_9_9
  "xxxxxxxxx"
  " xxxxxxx "
  " xxxxxxx "
  "  xxxxx  "
  "  xxxxx  "
  "   xxx   "
  "   xxx   "
  "    x    "
  "    x    "
  // SHIP_FILLED_9_9
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "xxxxxxxxx"
  " xxxxxxx "
  "  xxxxx  "
  "   xxx   "
  "    x    "
  //"#"
};

void convert_bitmaps(void)
{
  int img, l, x;
  int rpos,wpos;
  unsigned char v1,v2;
	
  rpos = 0;
  wpos = 0;
  for (img=0;img<90;img++)
    {
      for (l=8;l>=0;l--)
        {
          v1 = v2 = 0;
          //if (marker_char_bitmaps[rpos        ] == '#') return;
          // glBitmap = upside down.....
          if (marker_char_bitmaps[(l*9)+rpos  ] == 'x') v1 += 0x80;
          if (marker_char_bitmaps[(l*9)+rpos+1] == 'x') v1 += 0x40;
          if (marker_char_bitmaps[(l*9)+rpos+2] == 'x') v1 += 0x20;
          if (marker_char_bitmaps[(l*9)+rpos+3] == 'x') v1 += 0x10;
          if (marker_char_bitmaps[(l*9)+rpos+4] == 'x') v1 += 0x08;
          if (marker_char_bitmaps[(l*9)+rpos+5] == 'x') v1 += 0x04;
          if (marker_char_bitmaps[(l*9)+rpos+6] == 'x') v1 += 0x02;
          if (marker_char_bitmaps[(l*9)+rpos+7] == 'x') v1 += 0x01;
          if (marker_char_bitmaps[(l*9)+rpos+8] == 'x') v2 += 0x80;
          markerimages[wpos  ] = v1;
          markerimages[wpos+1] = v2;
          markerimages[wpos+2] = 0;
          markerimages[wpos+3] = 0;
          //rpos += 9;
          wpos += 4;
        }
      rpos += (9*9);
    }
}


void
SoMarkerSet::GLRender(SoGLRenderAction * action)
{
  if (!this->shouldGLRender(action)) return;
  SoState *state = action->getState();
  const SoCoordinateElement *tmp;
  const SbVec3f *normals;
  SbBool doTextures;
  SbBool needNormals = (SoLightModelElement::get(state) != SoLightModelElement::BASE_COLOR);
  SoVertexShape::getVertexData(action->getState(), tmp, normals, needNormals);
  if (normals == NULL) needNormals = FALSE;
  const SoGLCoordinateElement * coords = (SoGLCoordinateElement *)tmp;
  SoTextureCoordinateBundle tb(action, TRUE, FALSE);
  doTextures = tb.needCoordinates();

  Binding mbind = this->findMaterialBinding(action->getState());
  Binding nbind = this->findNormalBinding(action->getState());
  if (!needNormals) {
    nbind = OVERALL;
    const SoGLLightModelElement * lm = (SoGLLightModelElement *)
      state->getConstElement(SoGLLightModelElement::getClassStackIndex());
    lm->forceSend(SoLightModelElement::BASE_COLOR);
  }
  SbVec3f dummynormal(0.0f, 0.0f, 1.0f);
  const SbVec3f * currnormal = &dummynormal;
  if (normals) currnormal = normals;
  if (nbind == OVERALL && needNormals)
    glNormal3fv((const GLfloat *)currnormal);

  SoMaterialBundle mb(action);
  mb.sendFirst(); // make sure we have the correct material

  int32_t idx = this->startIndex.getValue();
  int32_t numpts = this->numPoints.getValue();
  if (numpts < 0) numpts = coords->getNum() - idx;
  int matnr = 0;
  int texnr = 0;

  const SbMatrix & mat = SoModelMatrixElement::get(state);
  const SbViewVolume & vv = SoViewVolumeElement::get(state);
  const SbViewportRegion & vp = SoViewportRegionElement::get(state);
  SbVec2s vpsize = vp.getViewportSizePixels();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, vpsize[0], 0, vpsize[1], -1.0f, 1.0f);

  //glPixelStorei(GL_UNPACK_ALIGNMENT,4);

  SbVec3f nilpoint;
	
  //glBegin(GL_POINTS);
  for (int i = 0; i < numpts; i++)
    {
      if (nbind == PER_VERTEX) {
        currnormal = normals++;
        glNormal3fv((const GLfloat *)currnormal);
      }
      if (mbind == PER_VERTEX) mb.send(matnr++, TRUE);
      if (doTextures) tb.send(texnr++, coords->get3(idx), *currnormal);

      //coords->send(idx++);
      nilpoint = coords->get3(idx++);
      mat.multVecMatrix(nilpoint, nilpoint);
      vv.projectToScreen(nilpoint, nilpoint);
      nilpoint[0] = nilpoint[0] * float(vpsize[0]);
      nilpoint[1] = nilpoint[1] * float(vpsize[1]);
      // glRasterPos2i(nilpoint[0],nilpoint[1]);
      // glBitmap(9,9,4,4,0,0,markerimages + (markerIndex[i] * 36) );
      glRasterPos2i(0,0);
      //glBitmap(9,9,4,4,nilpoint[0],nilpoint[1],NULL);						// -> crash, core dump
      glBitmap(0,0,0,0,nilpoint[0],nilpoint[1],NULL);							// -> working...
      glBitmap(9,9,4,4,0,0,markerimages + (markerIndex[i] * 36) );
    }
  //glEnd();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

// ----------------------------------------------------------------------------------------------------

/*!
  FIXME: write doc
 */
void
SoMarkerSet::getPrimitiveCount(SoGetPrimitiveCountAction * /* action */)
{
  COIN_STUB();
}

/*!
  FIXME: write doc
 */
int
SoMarkerSet::getNumDefinedMarkers(void)
{
  //COIN_STUB();
  //return 0;
  return 90;
}

/*!
  FIXME: write doc
 */
void
SoMarkerSet::addMarker(int /* markerIndex */, const SbVec2s & /* size */,
                       const unsigned char * /* bytes */, SbBool /* isLSBFirst */,
                       SbBool /* isUpToDown */)
{
  COIN_STUB();
}

/*!
  FIXME: write doc
 */
SbBool
SoMarkerSet::getMarker(int /* markerIndex */, SbVec2s & /* size */,
                       const unsigned char *& /* bytes */, SbBool & /* isLSBFirst */)
{
  COIN_STUB();
  return FALSE;
}

/*!
  FIXME: write doc
 */
SbBool
SoMarkerSet::removeMarker(int /* markerIndex */)
{
  COIN_STUB();
  return FALSE;
}

/*!
  FIXME: write doc
 */
SbBool
SoMarkerSet::isMarkerBitSet(int /* markerIndex */, int /* bitNumber */)
{
  COIN_STUB();
  return FALSE;
}
