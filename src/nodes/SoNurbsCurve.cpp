/**************************************************************************\
 *
 *  Copyright (C) 1998-2000 by Systems in Motion.  All rights reserved.
 *
 *  This file is part of the Coin library.
 *
 *  This file may be distributed under the terms of the Q Public License
 *  as defined by Troll Tech AS of Norway and appearing in the file
 *  LICENSE.QPL included in the packaging of this file.
 *
 *  If you want to use Coin in applications not covered by licenses
 *  compatible with the QPL, you can contact SIM to aquire a
 *  Professional Edition license for Coin.
 *
 *  Systems in Motion AS, Prof. Brochs gate 6, N-7030 Trondheim, NORWAY
 *  http://www.sim.no/ sales@sim.no Voice: +47 22114160 Fax: +47 67172912
 *
\**************************************************************************/

/*!
  \class SoNurbsCurve SoNurbsCurve.h Inventor/nodes/SoNurbsCurve.h
  \brief The SoNurbsCurve class is a node for representing smooth curves.
  \ingroup nodes

  Explaining NURBS is beyond the scope of this documentation. If you
  are unfamiliar with the principles of representing smooth curves and
  surfaces when doing 3D visualization, we recommend finding a good
  book on the subject.
*/

#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoSubNodeP.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/caches/SoBoundingBoxCache.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoPickStyleElement.h>
#include <Inventor/elements/SoDrawStyleElement.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/elements/SoGLLightModelElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/misc/SoGL.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/errors/SoDebugError.h>

#include <coindefs.h> // COIN_STUB()
#ifdef _WIN32
#include <windows.h>
#endif // !_WIN32
#include <GL/gl.h>
#include <GL/glu.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H


/*!
  \var SoSFInt32 SoNurbsCurve::numControlPoints
  Number of control points to use in this NURBS curve.
*/
/*!
  \var SoMFFloat SoNurbsCurve::knotVector
  The knot vector.
*/

// *************************************************************************

SO_NODE_SOURCE(SoNurbsCurve);

/*!
  Constructor.
*/
SoNurbsCurve::SoNurbsCurve(void)
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoNurbsCurve);

  SO_NODE_ADD_FIELD(numControlPoints, (0));
  SO_NODE_ADD_FIELD(knotVector, (0));

  this->nurbsrenderer = NULL;
}

/*!
  Destructor.
*/
SoNurbsCurve::~SoNurbsCurve()
{
  if (this->nurbsrenderer) {
#ifdef HAVE_GLU_NURBSOBJECT
    gluDeleteNurbsRenderer((HAVE_GLU_NURBSOBJECT *)this->nurbsrenderer);
#endif // HAVE_GLU_NURBSOBJECT
  }
}

// Doc from parent class.
void
SoNurbsCurve::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoNurbsCurve);
}

// Doc from parent class.
void
SoNurbsCurve::GLRender(SoGLRenderAction * action)
{
  if (!this->shouldGLRender(action)) return;

  SoState * state = action->getState();

  // initialize current material
  SoMaterialBundle mb(action);
  mb.sendFirst();

  // disable lighting
  const SoGLLightModelElement * lm = (const SoGLLightModelElement *)
    state->getConstElement(SoGLLightModelElement::getClassStackIndex());
  lm->forceSend(SoLightModelElement::BASE_COLOR);

  // disable texturing
  const SoGLTextureEnabledElement * te = (const SoGLTextureEnabledElement *)
    state->getConstElement(SoGLTextureEnabledElement::getClassStackIndex());
  te->forceSend(FALSE);

  // Create lazy element for GL_AUTO_NORMAL ?
  glEnable(GL_AUTO_NORMAL);
  this->doNurbs(action, TRUE, SoDrawStyleElement::get(action->getState()) == SoDrawStyleElement::POINTS);
  glDisable(GL_AUTO_NORMAL);
}

/*!
  Calculates the bounding box of all control points, and sets the
  center to the average of these points.
*/
void
SoNurbsCurve::computeBBox(SoAction * action, SbBox3f & box, SbVec3f & center)
{
  SoState * state = action->getState();
  const SoCoordinateElement * coordelem =
    SoCoordinateElement::getInstance(state);

  int numCoords = coordelem->getNum();
  int num = this->numControlPoints.getValue();

  assert(num <= numCoords);

  SbVec3f acccenter(0.0f, 0.0f, 0.0f);
  box.makeEmpty();
  if (coordelem->is3D()) {
    const SbVec3f * coords = coordelem->getArrayPtr3();
    assert(coords);
    for (int i = 0; i < num; i++) {
      box.extendBy(coords[i]);
      acccenter += coords[i];
    }
  }
  else {
    const SbVec4f * coords = coordelem->getArrayPtr4();
    assert(coords);
    for (int i = 0; i < num; i++) {
      SbVec4f tmp = coords[i];
      float mul = (tmp[3] != 0.0f) ? 1.0f / tmp[3] : 1.0f;
      SbVec3f tmp3D(tmp[0]*mul, tmp[1]*mul, tmp[2]*mul);
      box.extendBy(tmp3D);
      acccenter += tmp3D;
    }
  }
  if (num) center = acccenter / float(num);
}

// Doc from parent class.
void
SoNurbsCurve::rayPick(SoRayPickAction * action)
{
#if GLU_VERSION_1_3
  SoShape::rayPick(action); // do normal generatePrimitives() pick
#else // ! GLU_VERSION_1_3
  if (!this->shouldRayPick(action)) return;
  static SbBool firstpick = TRUE;
  if (firstpick) {
    firstpick = FALSE;
    SoDebugError::postWarning("SoNurbsCurve::rayPick",
                              "Proper NURBS picking requires\n"
                              "GLU version 1.3. Picking will be done on bounding box.");
  }
  SoState * state = action->getState();
  state->push();
  SoPickStyleElement::set(state, this, SoPickStyleElement::BOUNDING_BOX);
  (void)this->shouldRayPick(action); // this will cause a pick on bbox
  state->pop();
#endif // ! GLU_VERSION_1_3
}

// Doc from parent class.
void
SoNurbsCurve::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  COIN_STUB();
}

/*!
  Redefined to notify open caches that this shape contains lines.
*/
void
SoNurbsCurve::getBoundingBox(SoGetBoundingBoxAction * action)
{
  inherited::getBoundingBox(action);
  SoBoundingBoxCache::setHasLinesOrPoints(action->getState());
}

/*!
  Not implemented in Coin. Should probably have been private in OIV.
*/
void
SoNurbsCurve::sendPrimitive(SoAction *,  SoPrimitiveVertex *)
{
  COIN_STUB();
}

// Doc from parent class.
void
SoNurbsCurve::generatePrimitives(SoAction * action)
{
  this->doNurbs(action, FALSE, FALSE);
}

/*!
  Overloaded to return NULL.
*/
SoDetail *
SoNurbsCurve::createLineSegmentDetail(SoRayPickAction * /* action */,
                                      const SoPrimitiveVertex * /* v1 */,
                                      const SoPrimitiveVertex * /* v2 */,
                                      SoPickedPoint * /* pp */)
{
  return NULL;
}

//
// used only for GLU callbacks
//
typedef struct {
  SoAction * action;
  SoNurbsCurve * thisp;
  SoPrimitiveVertex vertex;
} coin_nc_cbdata;

void
SoNurbsCurve::doNurbs(SoAction * action, const SbBool glrender, const SbBool drawaspoints)
{
#ifdef HAVE_GLU_NURBSOBJECT
  HAVE_GLU_NURBSOBJECT * nurbsobj;
  if (this->nurbsrenderer == NULL) {
    nurbsobj = gluNewNurbsRenderer();
    this->nurbsrenderer = (void*) nurbsobj;

#if GLU_VERSION_1_3
    gluNurbsCallback(nurbsobj, (GLenum) GLU_NURBS_BEGIN_DATA, (void (*)(...))tessBegin);
    gluNurbsCallback(nurbsobj, (GLenum) GLU_NURBS_TEXTURE_COORD_DATA, (void (*)(...))tessTexCoord);
    gluNurbsCallback(nurbsobj, (GLenum) GLU_NURBS_NORMAL_DATA, (void (*)(...))tessNormal);
    gluNurbsCallback(nurbsobj, (GLenum) GLU_NURBS_VERTEX_DATA, (void (*)(...))tessVertex);
    gluNurbsCallback(nurbsobj, (GLenum) GLU_NURBS_END_DATA, (void (*)(...))tessEnd);
#endif // GLU_VERSION_1_3
  }
  nurbsobj = (HAVE_GLU_NURBSOBJECT*) this->nurbsrenderer;

#if GLU_VERSION_1_3
  coin_nc_cbdata cbdata;
  if (!glrender) {
    gluNurbsCallbackData(nurbsobj, &cbdata);
    cbdata.action = action;
    cbdata.thisp = this;
    cbdata.vertex.setNormal(SbVec3f(0.0f, 0.0f, 1.0f));
    cbdata.vertex.setMaterialIndex(0);
    cbdata.vertex.setTextureCoords(SbVec4f(0.0f, 0.0f, 0.0f, 1.0f));
    cbdata.vertex.setPoint(SbVec3f(0.0f, 0.0f, 0.0f));
    cbdata.vertex.setDetail(NULL);
  }
#endif // GLU_VERSION_1_3

  sogl_render_nurbs_curve(action, this, this->nurbsrenderer,
                          this->numControlPoints.getValue(),
                          this->knotVector.getValues(0),
                          this->knotVector.getNum(),
                          glrender,
                          drawaspoints);
#else // !HAVE_GLU_NURBSOBJECT
#if COIN_DEBUG
  static int first = 1;
  if (first) {
    SoDebugError::postInfo("SoNurbsCurve::doNurbs",
                           "Looks like your GLU library doesn't have NURBS "
                           "functionality");
    first = 0;
  }
#endif // COIN_DEBUG
#endif // !HAVE_GLU_NURBSOBJECT
}

void
SoNurbsCurve::tessBegin(int type, void * data)
{
  coin_nc_cbdata * cbdata = (coin_nc_cbdata*) data;
  TriangleShape shapetype;
  switch ((int)type) {
  case GL_LINES:
    shapetype = SoShape::LINES;
    break;
  case GL_LINE_STRIP:
    shapetype = SoShape::LINE_STRIP;
    break;
  case GL_LINE_LOOP:
    shapetype = SoShape::LINE_STRIP; // will not be closed...
#if COIN_DEBUG && 1 // debug
    SoDebugError::postInfo("SoNurbsCurve::tessBegin",
                           "LINE_LOOP is not supported yet");
#endif // debug
    break;
  case GL_POINTS:
    shapetype = SoShape::POINTS;
    break;
  default:
    shapetype = SoShape::POLYGON; // illegal value
#if COIN_DEBUG && 1 // debug
    SoDebugError::postInfo("SoNurbsCurve::tessBegin",
                           "unsupported GL enum: 0x%x", type);
#endif // debug
    break;
  }
  if (shapetype != SoShape::POINTS) {
    cbdata->thisp->beginShape(cbdata->action, shapetype, NULL);
  }
}

void
SoNurbsCurve::tessTexCoord(GLfloat * texcoord, void * data)
{
  coin_nc_cbdata * cbdata = (coin_nc_cbdata*) data;
  cbdata->vertex.setTextureCoords(SbVec4f(texcoord[0], texcoord[1], texcoord[2], texcoord[3]));
}

void
SoNurbsCurve::tessNormal(GLfloat * normal, void * data)
{
  coin_nc_cbdata * cbdata = (coin_nc_cbdata*) data;
  cbdata->vertex.setNormal(SbVec3f(normal[0], normal[1], normal[2]));
}

void
SoNurbsCurve::tessVertex(GLfloat * vertex, void * data)
{
  coin_nc_cbdata * cbdata = (coin_nc_cbdata*) data;
  cbdata->vertex.setPoint(SbVec3f(vertex[0], vertex[1], vertex[2]));
  cbdata->thisp->shapeVertex(&cbdata->vertex);
}

void
SoNurbsCurve::tessEnd(void * data)
{
  coin_nc_cbdata * cbdata = (coin_nc_cbdata*) data;
  cbdata->thisp->endShape();
}
