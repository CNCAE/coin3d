/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2003 by Systems in Motion.  All rights reserved.
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
 *  See <URL:http://www.coin3d.org> for  more information.
 *
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no>.
 *
\**************************************************************************/

/*!
  \class SoText3 SoText3.h Inventor/nodes/SoText3.h
  \brief The SoText3 class renders extruded 3D text.
  \ingroup nodes

  Render text as 3D geometry.

  Beware that using a lot of SoText3 text characters in a scene will
  usually have severe impact on the rendering performance, as each and
  every character of the text contributes a lot of polygon primitives
  to the rendering system.

  Due to the above mentioned fact, SoText3 nodes are best used in
  situations where you need just one or a few characters to be placed
  in your scene, than to visualize e.g. complete sentences.

  \sa SoText2, SoAsciiText, SoProfile
*/

#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoSubNodeP.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetPrimitiveCountAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/details/SoTextDetail.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoGLShapeHintsElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoCreaseAngleElement.h>
#include <Inventor/elements/SoGLTexture3EnabledElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/misc/SoGlyph.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/misc/SoNormalGenerator.h>
#include <Inventor/nodes/SoProfile.h>
#include <Inventor/nodes/SoNurbsProfile.h>
#include <Inventor/SbLine.h>
#include <Inventor/lists/SbList.h>
#include <string.h>

#include <coindefs.h> // COIN_OBSOLETED()

#include <float.h> // FLT_MAX, FLT_MIN

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H
#include <Inventor/system/gl.h>

#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

#include "../fonts/glyph3d.h"

/*!
  \enum SoText3::Part
  Used to specify which parts should be rendered/generated.
*/
/*!
  \var SoText3::Part SoText3::FRONT
  Front of characters.
*/
/*!
  \var SoText3::Part SoText3::SIDES
  Sides of characters.
*/
/*!
  \var SoText3::Part SoText3::BACK
  Back of characters.
*/
/*!
  \var SoText3::Part SoText3::ALL
  All parts.
*/

/*!
  \enum SoText3::Justification
  Used to specify horizontal string alignment.
*/
/*!
  \var SoText3::Justification SoText3::LEFT
  Left edges of strings are aligned.
*/
/*!
  \var SoText3::Justification SoText3::RIGHT
  Right edges of strings are aligned.
*/
/*!
  \var SoText3::Justification SoText3::CENTER
  Strings are centered.
*/


/*!
  \var SoMFString SoText3::string
  The strings. Defaults to empty set.
*/
/*!
  \var SoSFFloat SoText3::spacing
  Vertical spacing. 1.0 is the default spacing.
*/
/*!
  \var SoSFEnum SoText3::justification
  Horizontal justification. Default is alignment at the left border.
*/
/*!
  \var SoSFBitMask SoText3::parts
  Character parts. Default is to show only the front-facing part.
*/

// FIXME: missing features, pederb 20000224
//   - Texture coordinates are not generated yet.
//   - Normals for SIDES should be smoothed.

// *************************************************************************

class SoText3P {
public:
  SoText3P(SoText3 * master) : master(master) { }

  void render(SoState * state, const cc_font_specification * fontspec, unsigned int part);
  void generate(SoAction * action, const cc_font_specification * fontspec, unsigned int part);

  SbList <float> widths;
  void setUpGlyphs(SoState * state, const cc_font_specification * fontspec, SoText3 * textnode);
  SbBool needsetup;
  SbBox3f maxglyphbbox;

  cc_string * prevfontname; // Store important fontspecs so that changes can be detected
  cc_string * prevfontstyle;
  float prevfontsize;
  SoNormalGenerator * normalgenerator;

private:
  SoText3 * master;
};

#define PRIVATE(p) ((p)->pimpl)
#define PUBLIC(p) ((p)->master)

// *************************************************************************

SO_NODE_SOURCE(SoText3);

// *************************************************************************

SoText3::SoText3(void)
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoText3);

  SO_NODE_ADD_FIELD(string, (""));
  SO_NODE_ADD_FIELD(spacing, (1.0f));
  SO_NODE_ADD_FIELD(justification, (SoText3::LEFT));
  SO_NODE_ADD_FIELD(parts, (SoText3::FRONT));

  SO_NODE_DEFINE_ENUM_VALUE(Justification, LEFT);
  SO_NODE_DEFINE_ENUM_VALUE(Justification, RIGHT);
  SO_NODE_DEFINE_ENUM_VALUE(Justification, CENTER);
  SO_NODE_SET_SF_ENUM_TYPE(justification, Justification);

  SO_NODE_DEFINE_ENUM_VALUE(Part, FRONT);
  SO_NODE_DEFINE_ENUM_VALUE(Part, SIDES);
  SO_NODE_DEFINE_ENUM_VALUE(Part, BACK);
  SO_NODE_DEFINE_ENUM_VALUE(Part, ALL);
  SO_NODE_SET_SF_ENUM_TYPE(parts, Part);

  PRIVATE(this) = new SoText3P(this);
  PRIVATE(this)->needsetup = TRUE;
  PRIVATE(this)->prevfontname = cc_string_construct_new();
  PRIVATE(this)->prevfontstyle = cc_string_construct_new();
  PRIVATE(this)->prevfontsize = -1;
  PRIVATE(this)->normalgenerator = new SoNormalGenerator(FALSE, 0xff);
}

SoText3::~SoText3()
{
  delete PRIVATE(this)->normalgenerator;
  delete PRIVATE(this);
}

// doc in parent
void
SoText3::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoText3, SO_FROM_INVENTOR_2_1);
}

// doc in parent
void
SoText3::computeBBox(SoAction * action, SbBox3f & box, SbVec3f & center)
{
  SoState * state = action->getState();

  cc_font_specification fontspec;
  cc_fontspec_construct(&fontspec, SoFontNameElement::get(state).getString(),
                        SoFontSizeElement::get(state),
                        SoComplexityElement::get(state));

  PRIVATE(this)->setUpGlyphs(state, &fontspec, this);
  int i, n = PRIVATE(this)->widths.getLength();
  if (n == 0) {
    center = SbVec3f(0.0f, 0.0f, 0.0f);
    box.setBounds(center, center);
    cc_fontspec_clean(&fontspec);
    return;
  }

  float maxw = FLT_MIN;
  for (i = 0; i < n; i++) 
    maxw = SbMax(maxw, PRIVATE(this)->widths[i]); 
  
  if(maxw == FLT_MIN) { // There is no text to bound. Returning.
    box.setBounds(SbVec3f(0.0f, 0.0f, 0.0f), SbVec3f(0.0f, 0.0f, 0.0f));
    center = SbVec3f(0,0,0);
    return; 
  }

  SbBox2f maxbox;
  float maxglyphsize = 1;
  
  float maxy = fontspec.size;
  float miny = this->spacing.getValue() * fontspec.size * (n-1);

  float minx, maxx;
  switch (this->justification.getValue()) {
  case SoText3::LEFT:
    minx = 0.0f;
    maxx = maxw;
    break;
  case SoText3::RIGHT:
    minx = -maxw;
    maxx = 0.0f;
    break;
  case SoText3::CENTER:
    maxx = maxw * 0.5f;
    minx = -maxx;
    break;
  default:
    assert(0 && "unknown justification");
    minx = maxx = 0.0f;
    break;
  }

  // check profiles and extend bounding box if necessary
  float profsize = 0;
  float minz = 0.0f, maxz = 0.0f;
  
  const SoNodeList profilenodes = SoProfileElement::get(state);
  int numprofiles = profilenodes.getLength();
  if ( numprofiles > 0) {
    assert(profilenodes[0]->getTypeId().isDerivedFrom(SoProfile::getClassTypeId()));
    for (int i = numprofiles-1; i >= 0; i--) {
      SoProfile *pn = (SoProfile *)profilenodes[i];
      if (pn->isOfType(SoNurbsProfile::getClassTypeId())) {
        // Don't use SoProfile::getVertices() for SoNurbsProfile
        // nodes as this would cause a call to the GLU library, which
        // requires a valid GL context. Instead we approximate using
        // SoNurbsProfile::getTrimCurve(), and use the control points
        // to calculate the bounding box. This is an approximation,
        // but the same technique is used in So[Indexed]NurbsSurface
        // and So[Indexed]NurbsCurve. To avoid this approximation we
        // would need our own NURBS library.    pederb, 20000926
        SoNurbsProfile * np = (SoNurbsProfile*) pn;
        float * knots;
        int32_t numknots;
        int dim;
        int32_t numpts;
        float * points;
        np->getTrimCurve(state, numpts, points, dim,
                         numknots, knots);
        for (int j = 0; j < numpts; j++) {
          if (-points[j*dim] > maxz) maxz = -points[j*dim];
          if (-points[j*dim] < minz) minz = -points[j*dim];
          if (points[j*dim+1] > profsize) profsize = points[j*dim+1];
        }
      }
      else {
        int32_t num;
        SbVec2f *coords;
        pn->getVertices(state, num, coords);
        for (int j = 0; j < num; j++) {
          if (-coords[j][0] > maxz) maxz = -coords[j][0];
          if (-coords[j][0] < minz) minz = -coords[j][0];
          if (coords[j][1] > profsize) profsize = coords[j][1];
        }
      }
    }
  }
  else {
    // extrude
    if (this->parts.getValue() == SoText3::BACK) {
      maxz = -1.0f;
    }
    else if (this->parts.getValue() == SoText3::FRONT) {
      minz = 0.0f;
    }
  }
 
  box.setBounds(SbVec3f(minx, miny, minz), SbVec3f(maxx, maxy, maxz));

  // Expanding bbox so that glyphs like 'j's and 'q's are completely inside.
  box.extendBy(SbVec3f(0,PRIVATE(this)->maxglyphbbox.getMin()[1] - (n-1) * fontspec.size, 0));
  box.extendBy(PRIVATE(this)->maxglyphbbox);

  box.extendBy(SbVec3f(box.getMax()[0] + profsize, box.getMax()[1] + profsize, 0));
  box.extendBy(SbVec3f(box.getMin()[0] - profsize, box.getMin()[1] - profsize, 0));
    
  center = box.getCenter();

  cc_fontspec_clean(&fontspec);
}


/*!
  Not implemented. Should probably have been private in OIV. Let us
  know if you need this method for anything, and we'll implement it.
*/
SbBox3f
SoText3::getCharacterBounds(SoState * state, int stringindex, int charindex)
{
  COIN_OBSOLETED();
  return SbBox3f();
}

// doc in parent
void
SoText3::GLRender(SoGLRenderAction * action)
{

  if (!this->shouldGLRender(action)) return;

  SoState * state = action->getState();

  cc_font_specification fontspec;
  cc_fontspec_construct(&fontspec, SoFontNameElement::get(state).getString(),
                        SoFontSizeElement::get(state),
                        this->getComplexityValue(state->getAction()));

  PRIVATE(this)->setUpGlyphs(state, &fontspec, this);

  SoMaterialBindingElement::Binding binding =
    SoMaterialBindingElement::get(state);

  SbBool matperpart = (binding != SoMaterialBindingElement::OVERALL);

  SoMaterialBundle mb(action);
  mb.sendFirst();

  unsigned int prts = this->parts.getValue();

  if (prts & SoText3::FRONT) {
    PRIVATE(this)->render(state, &fontspec, SoText3::FRONT);
  }
  if (prts & SoText3::SIDES) {
    if (matperpart) mb.send(1, FALSE);
    PRIVATE(this)->render(state, &fontspec, SoText3::SIDES);
  }
  if (prts & SoText3::BACK) {
    if (matperpart) mb.send(2, FALSE);
    PRIVATE(this)->render(state, &fontspec, SoText3::BACK);
  }
  
  if (SoComplexityTypeElement::get(state) == SoComplexityTypeElement::OBJECT_SPACE) 
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DO_AUTO_CACHE);
    
  cc_fontspec_clean(&fontspec);
}

// doc in parent
void
SoText3::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  if (action->is3DTextCountedAsTriangles()) {
    // will cause a call to generatePrimitives() 
    // slow, but we can't be bothered to implement a new loop to count triangles
    inherited::getPrimitiveCount(action);
  }
  else {
    action->addNumText(this->string.getNum());
  }
}

// doc in parent
void
SoText3::generatePrimitives(SoAction * action)
{
  SoState * state = action->getState();

  cc_font_specification fontspec;
  cc_fontspec_construct(&fontspec, SoFontNameElement::get(state).getString(),
                        SoFontSizeElement::get(state),
                        this->getComplexityValue(state->getAction()));

  PRIVATE(this)->setUpGlyphs(state, &fontspec, this);

  unsigned int prts = this->parts.getValue();

  if (prts & SoText3::FRONT) {
    PRIVATE(this)->generate(action, &fontspec, SoText3::FRONT);
  }
  if (prts & SoText3::SIDES) {
    PRIVATE(this)->generate(action, &fontspec, SoText3::SIDES);
  }
  if (prts & SoText3::BACK) {
    PRIVATE(this)->generate(action, &fontspec, SoText3::BACK);
  }

  cc_fontspec_clean(&fontspec);
}

// doc in parent
SoDetail *
SoText3::createTriangleDetail(SoRayPickAction * action,
                              const SoPrimitiveVertex * v1,
                              const SoPrimitiveVertex * v2,
                              const SoPrimitiveVertex * v3,
                              SoPickedPoint * pp)
{
  // generatePrimitives() places text details inside each primitive vertex
  assert(v1->getDetail());
  return v1->getDetail()->copy();
}

void
SoText3P::render(SoState * state, const cc_font_specification * fontspec,
                 unsigned int part)
{
  int i, n = this->widths.getLength();

  int firstprofile = -1;
  int32_t profnum;
  SbVec2f *profcoords;
  float nearz =  FLT_MAX;
  float farz  = -FLT_MAX;

  SbBool do2Dtextures = FALSE;
  SbBool do3Dtextures = FALSE;
  if (SoGLTextureEnabledElement::get(state)) do2Dtextures = TRUE;
  else if (SoGLTexture3EnabledElement::get(state)) do3Dtextures = TRUE;

  // FIXME: implement proper support for 3D-texturing, and get rid of
  // this. (20031010 handegar)
  if (do3Dtextures) {
    static SbBool first = TRUE;
    if (first) {
      first = FALSE;
      SoDebugError::postWarning("SoText3::GLRender",
                                "3D-textures not properly supported for this node type yet.");
    }
  }


  const SoNodeList & profilenodes = SoProfileElement::get(state);
  int numprofiles = profilenodes.getLength();

  if (numprofiles > 0) {
    assert(profilenodes[0]->getTypeId().isDerivedFrom(SoProfile::getClassTypeId()));
    // Find near/far z (for modifying position of front/back)

    for (int l=numprofiles-1; l>=0; l--) {
      SoProfile * pn = (SoProfile *) profilenodes[l];
      pn->getVertices(state, profnum, profcoords);

      if (profnum > 0) {
        if (profcoords[profnum-1][0] > farz) farz = profcoords[profnum-1][0];
        if (profcoords[0][0] < nearz) nearz = profcoords[0][0];
        if (pn->linkage.getValue() == SoProfile::START_FIRST) {
          if (firstprofile == -1) firstprofile = l;
          break;
        }
      }

    }
    nearz = -nearz;
    farz = -farz;
  }
  else {
    nearz = 0.0;
    farz = -1.0;
  }


  int glyphidx = 0;
  float ypos = 0.0f;
 
  for (i = 0; i < n; i++) {

    float xpos = 0.0f;
    switch (PUBLIC(this)->justification.getValue()) {
    case SoText3::RIGHT:
      xpos = -this->widths[i];
      break;
    case SoText3::CENTER:
      xpos = - this->widths[i] * 0.5f;
      break;
    }

    cc_glyph3d * prevglyph = NULL;
    const unsigned int length = PUBLIC(this)->string[i].getLength();
    for (unsigned int strcharidx = 0; strcharidx < length; strcharidx++) {
 
      // Note that the "unsigned char" cast is needed to avoid 8-bit
      // chars using the highest bit (i.e. characters above the ASCII
      // set up to 127) be expanded to huge int numbers that turn
      // negative when casted to integer size.
      const uint32_t glyphidx = (const unsigned char) PUBLIC(this)->string[i][strcharidx];
      cc_glyph3d * glyph = cc_glyph3d_getglyph(glyphidx, fontspec);
      const SbVec2f * coords = (SbVec2f *) cc_glyph3d_getcoords(glyph);

      // Get kerning
      if (strcharidx > 0) {
        float kerningx, kerningy;
        cc_glyph3d_getkerning(prevglyph, glyph, &kerningx, &kerningy);
        xpos += kerningx * fontspec->size;
      }
      prevglyph = glyph;
      

      if (part != SoText3::SIDES) {  // FRONT & BACK
        const int * ptr = cc_glyph3d_getfaceindices(glyph);
        glBegin(GL_TRIANGLES);
       
        while (*ptr >= 0) {
          SbVec2f v0, v1, v2;
          float zval;
          if (part == SoText3::FRONT) {         
            glNormal3f(0.0f, 0.0f, 1.0f);
            v0 = coords[*ptr++];
            v1 = coords[*ptr++];
            v2 = coords[*ptr++];
            zval = nearz;
          }
          else {  // BACK
            glNormal3f(0.0f, 0.0f, -1.0f);
            v2 = coords[*ptr++];
            v1 = coords[*ptr++];
            v0 = coords[*ptr++];
            zval = farz;
          }
          if(do2Dtextures)
            glTexCoord2f(v0[0], v0[1]);           
          glVertex3f(v0[0] * fontspec->size + xpos, v0[1] * fontspec->size + ypos, zval);
          if(do2Dtextures)
            glTexCoord2f(v1[0], v1[1]);                       
          glVertex3f(v1[0] * fontspec->size + xpos, v1[1] * fontspec->size + ypos, zval);
          if(do2Dtextures)
            glTexCoord2f(v2[0], v2[1]);           
          glVertex3f(v2[0] * fontspec->size + xpos, v2[1] * fontspec->size + ypos, zval);

        }

        glEnd();

      }
      else { // SIDES

        if (profilenodes.getLength() == 0) {  // no profile - extrude

          const int * ptr = cc_glyph3d_getedgeindices(glyph);
          SbVec2f v0, v1;
          int counter = 0;

          glBegin(GL_QUADS);
          while (*ptr >= 0) {            

            v0 = coords[*ptr++];
            v1 = coords[*ptr++];
            const int * ccw = (int *) cc_glyph3d_getnextccwedge(glyph, counter);
            const int * cw  = (int *) cc_glyph3d_getnextcwedge(glyph, counter);
            SbVec3f vleft(coords[*(ccw+1)][0], coords[*(ccw+1)][1], 0);
            SbVec3f vright(coords[*cw][0], coords[*cw][1], 0);
            counter++;
                      
            // create two 'normal' vectors pointing out from the edges
            SbVec3f normala(vleft[0] - v0[0], vleft[1] - v0[1], 0.0f);
            normala = normala.cross(SbVec3f(0.0f, 0.0f,  -1.0f));
            if (normala.length() > 0)
              normala.normalize();
       
            SbVec3f normalb(v1[0] - vright[0], v1[1] - vright[1], 0.0f);
            normalb = normalb.cross(SbVec3f(0.0f, 0.0f,  -1.0f));
            if (normalb.length() > 0)
              normalb.normalize();

            SbBool flatshading = FALSE;
            float dot = normala.dot(normalb);
            if(acos(dot) > SoCreaseAngleElement::get(state)) {
              normala = SbVec3f(v1[0] - v0[0], v1[1] - v0[1], 0.0f);
              normala = normala.cross(SbVec3f(0.0f, 0.0f,  -1.0f));
              if (normala.length() > 0)
                normala.normalize();
              flatshading = TRUE;
            }
          
            if (!flatshading) {
              if(do2Dtextures)
                glTexCoord2f(v1[0], v1[1]);   
              glNormal3fv(normala.getValue());
              glVertex3f(v1[0]*fontspec->size + xpos, v1[1]*fontspec->size, 0.0f);     

              if(do2Dtextures)
                glTexCoord2f(v0[0], v0[1]);   
              glNormal3fv(normalb.getValue());
              glVertex3f(v0[0]*fontspec->size + xpos, v0[1]*fontspec->size + ypos, 0.0f);

              if(do2Dtextures)
                 glTexCoord2f(v0[0], v0[1]);   
              glNormal3fv(normalb.getValue());
              glVertex3f(v0[0]*fontspec->size + xpos, v0[1]*fontspec->size + ypos, -1.0f);

              if(do2Dtextures)
                glTexCoord2f(v1[0], v1[1]);
              glNormal3fv(normala.getValue());
              glVertex3f(v1[0]*fontspec->size + xpos, v1[1]*fontspec->size + ypos, -1.0f);

            }
            else {
              glNormal3fv(normala.getValue());
              if(do2Dtextures)
                glTexCoord2f(v1[0], v1[1]);   
              glVertex3f(v1[0]*fontspec->size + xpos, v1[1]*fontspec->size + ypos, 0.0f);          

              if(do2Dtextures)
                glTexCoord2f(v0[0], v0[1]); 
              glVertex3f(v0[0]*fontspec->size + xpos, v0[1]*fontspec->size + ypos, 0.0f);

              if(do2Dtextures)
                glTexCoord2f(v0[0], v0[1]);
              glVertex3f(v0[0]*fontspec->size + xpos, v0[1]*fontspec->size + ypos, -1.0f);

              if(do2Dtextures)
                glTexCoord2f(v1[0], v1[1]);   
              glVertex3f(v1[0]*fontspec->size + xpos, v1[1]*fontspec->size + ypos, -1.0f);
            }
          }
          glEnd();

        }
        else {  // profile
    
          const int * indices = cc_glyph3d_getedgeindices(glyph);
          int ind = 0;
          SbVec3f normala, normalb;

          SbList <SbVec3f> vertexlist;         
          this->normalgenerator->reset(FALSE);

          while (*indices >= 0) {
            
            int i0 = *indices++;
            int i1 = *indices++;
            SbVec3f va(coords[i0][0], coords[i0][1], nearz);
            SbVec3f vb(coords[i1][0], coords[i1][1], nearz);
            const int * ccw = (int *) cc_glyph3d_getnextccwedge(glyph, ind);
            const int * cw  = (int *) cc_glyph3d_getnextcwedge(glyph, ind);
            SbVec3f vleft(coords[*(ccw+1)][0], coords[*(ccw+1)][1], nearz);
            SbVec3f vright(coords[*cw][0], coords[*cw][1], nearz);
            ind++;
        
            va[0] = va[0] * fontspec->size;
            va[1] = va[1] * fontspec->size;
            vb[0] = vb[0] * fontspec->size;
            vb[1] = vb[1] * fontspec->size;
            vleft[0] = vleft[0] * fontspec->size;
            vleft[1] = vleft[1] * fontspec->size;
            vright[0] = vright[0] * fontspec->size;
            vright[1] = vright[1] * fontspec->size;

            // create two 'normal' vectors pointing out from the edges
            SbVec3f normala(vleft[0] - va[0], vleft[1] - va[1], 0.0f);
            normala = normala.cross(SbVec3f(0.0f, 0.0f,  -1.0f));
            if (normala.length() > 0) 
              normala.normalize();

            SbVec3f normalb(vb[0] - vright[0], vb[1] - vright[1], 0.0f);
            normalb = normalb.cross(SbVec3f(0.0f, 0.0f,  -1.0f));
            if (normalb.length() > 0)
              normalb.normalize();
  
            SoProfile * pn = (SoProfile *) profilenodes[firstprofile];
            pn->getVertices(state, profnum, profcoords);

            SbVec3f vc,vd;
            SbVec2f starta(va[0], va[1]);
            SbVec2f startb(vb[0], vb[1]);
            
            for (int j=firstprofile; j<numprofiles; j++) {
              SoProfile * pn = (SoProfile *) profilenodes[j];
              pn->getVertices(state, profnum, profcoords);
              
              for (int k=1; k<profnum; k++) {
                
                // Calc points for two next faces
                vd[0] = starta[0] + (profcoords[k][1] * normalb[0]);
                vd[1] = starta[1] + (profcoords[k][1] * normalb[1]);
                vd[2] = -profcoords[k][0];
                vc[0] = startb[0] + (profcoords[k][1] * normala[0]);
                vc[1] = startb[1] + (profcoords[k][1] * normala[1]);
                vc[2] = -profcoords[k][0];
                
                // The windows tesselation sometimes return
                // illegal/empty tris. A test must be done to
                // prevent stdout from being flooded with
                // normalize() warnings from inside the normal
                // generator.
               
                if ((va != vd) && (va != vb) && (vd != vb)) {	
                  vertexlist.append(va);
                  vertexlist.append(vd);
                  vertexlist.append(vb);
                  normalgenerator->triangle(va,vd,vb);
                }
                
                if ((vb != vd) && (vb != vc) && (vd != vc)) {	               
                  vertexlist.append(vb);
                  vertexlist.append(vd);
                  vertexlist.append(vc);
                  normalgenerator->triangle(vb,vd,vc);
                }                       
                
                va = vd;
                vb = vc;
                
              }            
            }
            
          }
              
          normalgenerator->generate(SoCreaseAngleElement::get(state));
          const SbVec3f * normals = normalgenerator->getNormals();          
          const int size = vertexlist.getLength();

          // NOTE: We add the xpos and ypos to each vertex at this
          // point because Linux systems seems to accumulate an error
          // when calculating the normals (ie. two 'o's in a row
          // doesn't get the same normals due to the xpos
          // difference). This doesn't happen on Windows so it is
          // probably a floating point precision issue linked to the
          // compilator. (Tested on MSVC 6 and GCC 2.95.4) (20031010
          // handegar).

          glBegin(GL_TRIANGLES);         
          for (int z = 0;z < size;z += 3) {

            // FIXME: Add proper texturing for profile
            // coords. (20031010 handegar)

            glNormal3fv(normals[z].getValue());
            glVertex3fv(SbVec3f(vertexlist[z][0] + xpos, 
                                vertexlist[z][1] + ypos, 
                                vertexlist[z][2]).getValue());

            glNormal3fv(normals[z+1].getValue());
            glVertex3fv(SbVec3f(vertexlist[z+1][0] + xpos, 
                                vertexlist[z+1][1] + ypos, 
                                vertexlist[z+1][2]).getValue());
   
            glNormal3fv(normals[z+2].getValue());
            glVertex3fv(SbVec3f(vertexlist[z+2][0] + xpos, 
                                vertexlist[z+2][1] + ypos, 
                                vertexlist[z+2][2]).getValue());
          }
          glEnd();
                    
          vertexlist.truncate(0);

        }

      }

      float advancex, advancey;
      cc_glyph3d_getadvance(glyph, &advancex, &advancey);
      xpos += advancex * fontspec->size;

    }
    ypos -= fontspec->size * PUBLIC(this)->spacing.getValue();
  }




}

// render text geometry
void
SoText3::render(SoState * state, unsigned int part)
{
  assert(FALSE && "obsoleted");
}

void
SoText3::generate(SoAction * action, unsigned int part)
{
  assert(FALSE && "obsoleted");
}

// generate text geometry
void
SoText3P::generate(SoAction * action, const cc_font_specification * fontspec,
                   unsigned int part)
{
  SoState * state = action->getState();

  int matidx = 0;
  if (SoMaterialBindingElement::get(state) != 
      SoMaterialBindingElement::OVERALL) {
    if (part == SoText3::SIDES) matidx = 1;
    else if (part == SoText3::BACK) matidx = 2;
  }

  SoPrimitiveVertex vertex;
  SoTextDetail detail;
  detail.setPart(part);
  vertex.setDetail(&detail);
  vertex.setMaterialIndex(matidx);

  int i, n = this->widths.getLength();

  int firstprofile = -1;
  int32_t profnum;
  SbVec2f *profcoords;
  float nearz =  FLT_MAX;
  float farz  = -FLT_MAX;

  const SoNodeList & profilenodes = SoProfileElement::get(state);
  int numprofiles = profilenodes.getLength();

  if (numprofiles > 0) {
    assert(profilenodes[0]->getTypeId().isDerivedFrom(SoProfile::getClassTypeId()));
    // Find near/far z (for modifying position of front/back)

    for (int l = numprofiles-1; l >= 0; l--) {
      SoProfile * pn = (SoProfile *)profilenodes[l];
      pn->getVertices(state, profnum, profcoords);

      if (profnum > 0) {
        if (profcoords[profnum-1][0] > farz) farz = profcoords[profnum-1][0];
        if (profcoords[0][0] < nearz) nearz = profcoords[0][0];
        if (pn->linkage.getValue() == SoProfile::START_FIRST) {
          if (firstprofile == -1) firstprofile = l;
          break;
        }
      }
    }

    nearz = -nearz;
    farz = -farz;
  }
  else {
    nearz = 0.0;
    farz = -1.0;
  }

  int glyphidx = 0;
  float ypos = 0.0f;

  for (i = 0; i < n; i++) {
    detail.setStringIndex(i);
    float xpos = 0.0f;
    switch (PUBLIC(this)->justification.getValue()) {
    case SoText3::RIGHT:
      xpos = -this->widths[i];
      break;
    case SoText3::CENTER:
      xpos = - this->widths[i] * 0.5f;
      break;
    }

    cc_glyph3d * prevglyph = NULL;
    const unsigned int length = PUBLIC(this)->string[i].getLength();
    for (unsigned int strcharidx = 0; strcharidx < length; strcharidx++) {

      // Note that the "unsigned char" cast is needed to avoid 8-bit
      // chars using the highest bit (i.e. characters above the ASCII
      // set up to 127) be expanded to huge int numbers that turn
      // negative when casted to integer size.      
      const uint32_t glyphidx = (const unsigned char) PUBLIC(this)->string[i][strcharidx];
      cc_glyph3d * glyph = cc_glyph3d_getglyph(glyphidx, fontspec);    
      const SbVec2f * coords = (SbVec2f *) cc_glyph3d_getcoords(glyph);

      detail.setCharacterIndex(strcharidx);

      // Get kerning
      if (strcharidx > 0) {
        float kerningx, kerningy;
        cc_glyph3d_getkerning(prevglyph, glyph, &kerningx, &kerningy);
        xpos += kerningx * fontspec->size;
      }
      prevglyph = glyph;


      if (part != SoText3::SIDES) {  // FRONT & BACK
        const int * ptr = cc_glyph3d_getfaceindices(glyph);
        PUBLIC(this)->beginShape(action, SoShape::TRIANGLES, NULL);
        
        while (*ptr >= 0) {
          SbVec2f v0, v1, v2;
          float zval;
          if (part == SoText3::FRONT) {
            vertex.setNormal(SbVec3f(0.0f, 0.0f, 1.0f));
            v0 = coords[*ptr++];
            v1 = coords[*ptr++];
            v2 = coords[*ptr++];
            zval = nearz;
          }
          else {  // BACK
            vertex.setNormal(SbVec3f(0.0f, 0.0f, -1.0f));
            v2 = coords[*ptr++];
            v1 = coords[*ptr++];
            v0 = coords[*ptr++];
            zval = farz;
          }
          vertex.setPoint(SbVec3f(v0[0] * fontspec->size + xpos, v0[1] * fontspec->size + ypos, zval));
          PUBLIC(this)->shapeVertex(&vertex);
          vertex.setPoint(SbVec3f(v1[0] * fontspec->size + xpos, v1[1] * fontspec->size + ypos, zval));
          PUBLIC(this)->shapeVertex(&vertex);
          vertex.setPoint(SbVec3f(v2[0] * fontspec->size + xpos, v2[1] * fontspec->size + ypos, zval));
          PUBLIC(this)->shapeVertex(&vertex);
        }

        PUBLIC(this)->endShape();

      }
      else { // SIDES
        if (profilenodes.getLength() == 0) {  // no profile - extrude
         
          const int * ptr = cc_glyph3d_getedgeindices(glyph);
          SbVec2f v0, v1;
          int counter = 0;          
          PUBLIC(this)->beginShape(action, SoShape::QUADS, NULL);

          while (*ptr >= 0) {            

            v0 = coords[*ptr++];
            v1 = coords[*ptr++];
            const int * ccw = (int *) cc_glyph3d_getnextccwedge(glyph, counter);
            const int * cw  = (int *) cc_glyph3d_getnextcwedge(glyph, counter);
            SbVec3f vleft(coords[*(ccw+1)][0], coords[*(ccw+1)][1], 0);
            SbVec3f vright(coords[*cw][0], coords[*cw][1], 0);
            counter++;
           
            v0[0] = v0[0] * fontspec->size + xpos;
            v0[1] = v0[1] * fontspec->size + ypos;
            v1[0] = v1[0] * fontspec->size + xpos;
            v1[1] = v1[1] * fontspec->size + ypos;
            vleft[0] = vleft[0] * fontspec->size + xpos;
            vleft[1] = vleft[1] * fontspec->size + ypos;
            vright[0] = vright[0] * fontspec->size + xpos;
            vright[1] = vright[1] * fontspec->size + ypos;

            // create two 'normal' vectors pointing out from the edges
            SbVec3f normala(vleft[0] - v0[0], vleft[1] - v0[1], 0.0f);
            normala = normala.cross(SbVec3f(0.0f, 0.0f,  -1.0f));
            if (normala.length() > 0)
              normala.normalize();
       
            SbVec3f normalb(v1[0] - vright[0], v1[1] - vright[1], 0.0f);
            normalb = normalb.cross(SbVec3f(0.0f, 0.0f,  -1.0f));
            if (normalb.length() > 0)
              normalb.normalize();

            SbBool flatshading = FALSE;
            float dot = normala.dot(normalb);
            if(acos(dot) > SoCreaseAngleElement::get(state)) {
              normala = SbVec3f(v1[0] - v0[0], v1[1] - v0[1], 0.0f);
              normala = normala.cross(SbVec3f(0.0f, 0.0f,  -1.0f));
              if (normala.length() > 0)
                normala.normalize();
              flatshading = TRUE;
            }

            if (!flatshading) {
              vertex.setNormal(normala);
              vertex.setPoint(SbVec3f(v1[0], v1[1], 0.0f));  
              PUBLIC(this)->shapeVertex(&vertex);              
              vertex.setNormal(normalb);
              vertex.setPoint(SbVec3f(v0[0], v0[1], 0.0f));  
              PUBLIC(this)->shapeVertex(&vertex);              
              vertex.setNormal(normalb);
              vertex.setPoint(SbVec3f(v0[0], v0[1], -1.0f));  
              PUBLIC(this)->shapeVertex(&vertex);              
              vertex.setNormal(normala);
              vertex.setPoint(SbVec3f(v1[0], v1[1], -1.0f));  
              PUBLIC(this)->shapeVertex(&vertex);
            }
            else {
              vertex.setNormal(normala);
              vertex.setPoint(SbVec3f(v1[0], v1[1], 0.0f));  
              PUBLIC(this)->shapeVertex(&vertex);
              vertex.setPoint(SbVec3f(v0[0], v0[1], 0.0f));  
              PUBLIC(this)->shapeVertex(&vertex);              
              vertex.setPoint(SbVec3f(v0[0], v0[1], -1.0f));  
              PUBLIC(this)->shapeVertex(&vertex);              
              vertex.setPoint(SbVec3f(v1[0], v1[1], -1.0f));  
              PUBLIC(this)->shapeVertex(&vertex);
            }
          }
          
          PUBLIC(this)->endShape();

        }
        else {  // profile

          const int *indices = cc_glyph3d_getedgeindices(glyph);
          int ind = 0;
          SbVec3f normala, normalb;

          SbList <SbVec3f> vertexlist;         
          this->normalgenerator->reset(FALSE);

          while (*indices >= 0) {

            int i0 = *indices++;
            int i1 = *indices++;
            SbVec3f va(coords[i0][0], coords[i0][1], nearz);
            SbVec3f vb(coords[i1][0], coords[i1][1], nearz);
            const int *ccw = (int *) cc_glyph3d_getnextccwedge(glyph, ind);
            const int *cw  = (int *) cc_glyph3d_getnextcwedge(glyph, ind);
            SbVec3f vleft(coords[*(ccw+1)][0], coords[*(ccw+1)][1], nearz);
            SbVec3f vright(coords[*cw][0], coords[*cw][1], nearz);        
            ind++;

            va[0] = va[0] * fontspec->size;
            va[1] = va[1] * fontspec->size;
            vb[0] = vb[0] * fontspec->size;
            vb[1] = vb[1] * fontspec->size;
            vleft[0] = vleft[0] * fontspec->size;
            vleft[1] = vleft[1] * fontspec->size;
            vright[0] = vright[0] * fontspec->size;
            vright[1] = vright[1] * fontspec->size;

            // create two 'normal' vectors pointing out from the edges
            SbVec3f normala(vleft[0] - va[0], vleft[1] - va[1], 0.0f);
            normala = normala.cross(SbVec3f(0.0f, 0.0f,  -1.0f));
            if (normala.length() > 0) 
              normala.normalize();

            SbVec3f normalb(vb[0] - vright[0], vb[1] - vright[1], 0.0f);
            normalb = normalb.cross(SbVec3f(0.0f, 0.0f,  -1.0f));
            if (normalb.length() > 0)
              normalb.normalize();

            SoProfile *pn = (SoProfile *)profilenodes[firstprofile];
            pn->getVertices(state, profnum, profcoords);
         
            SbVec3f vc,vd;
            SbVec2f starta(va[0], va[1]);
            SbVec2f startb(vb[0], vb[1]);
        
            for (int j=firstprofile; j<numprofiles; j++) {
              SoProfile *pn = (SoProfile *)profilenodes[j];
              pn->getVertices(state, profnum, profcoords);

              for (int k=1; k<profnum; k++) {
                
                // Calc points for two next faces
                vd[0] = starta[0] + (profcoords[k][1] * normalb[0]);
                vd[1] = starta[1] + (profcoords[k][1] * normalb[1]);
                vd[2] = -profcoords[k][0];
                vc[0] = startb[0] + (profcoords[k][1] * normala[0]);
                vc[1] = startb[1] + (profcoords[k][1] * normala[1]);
                vc[2] = -profcoords[k][0];

                // The windows tesselation sometimes return
                // illegal/empty tris. A test must be done to prevent
                // stdout from being flooded with normalize() warnings
                // from inside the normal generator.
               
                if ((va != vd) && (va != vb) && (vd != vb)) {	
                  vertexlist.append(va);
                  vertexlist.append(vd);
                  vertexlist.append(vb);
                  normalgenerator->triangle(va,vd,vb);
                }
                
                if ((vb != vd) && (vb != vc) && (vd != vc)) {	               
                  vertexlist.append(vb);
                  vertexlist.append(vd);
                  vertexlist.append(vc);
                  normalgenerator->triangle(vb,vd,vc);
                }                       
                
                va = vd;
                vb = vc;

              }       
              
            }

          }


          normalgenerator->generate(SoCreaseAngleElement::get(state));
          const SbVec3f * normals = normalgenerator->getNormals();          
          const int size = vertexlist.getLength();

          PUBLIC(this)->beginShape(action, SoShape::TRIANGLES, NULL);
          for (int z = 0;z < size;z += 3) {      
            vertex.setNormal(normals[z].getValue());
            vertex.setPoint(SbVec3f(vertexlist[z][0] + xpos, 
                                    vertexlist[z][1] + ypos, 
                                    vertexlist[z][2]).getValue());
            PUBLIC(this)->shapeVertex(&vertex);              
          
            vertex.setNormal(normals[z+1].getValue());
            vertex.setPoint(SbVec3f(vertexlist[z+1][0] + xpos, 
                                    vertexlist[z+1][1] + ypos, 
                                    vertexlist[z+1][2]).getValue());
            PUBLIC(this)->shapeVertex(&vertex);              
          
            vertex.setNormal(normals[z+2].getValue());
            vertex.setPoint(SbVec3f(vertexlist[z+2][0] + xpos, 
                                    vertexlist[z+2][1] + ypos, 
                                    vertexlist[z+2][2]).getValue());
            PUBLIC(this)->shapeVertex(&vertex);              
                             
          }
          PUBLIC(this)->endShape();          
          vertexlist.truncate(0);

        }
      }

      float advancex, advancey;
      cc_glyph3d_getadvance(glyph, &advancex, &advancey);
      xpos += advancex * fontspec->size;

    }
    ypos -= fontspec->size * PUBLIC(this)->spacing.getValue();
  }


}

// Documented in superclass.
void 
SoText3::notify(SoNotList * list)
{
  // Overridden to detect when the string field changes.
  SoField * f = list->getLastField();
  if (f == &this->string) PRIVATE(this)->needsetup = TRUE;
  inherited::notify(list);
}

// recalculate glyphs
void
SoText3P::setUpGlyphs(SoState * state, const cc_font_specification * fontspec, SoText3 * textnode)
{
  // Note that this code is duplicated in SoAsciiText::setUpGlyphs(),
  // so migrate bugfixes and other improvements.
 
  // We have to force a new setup if style, size or font has
  // changed. This must be done if boundingbox and text alignment
  // shall stay correct
  if (cc_string_compare(&fontspec->name, this->prevfontname) != 0 ||
      cc_string_compare(&fontspec->style, this->prevfontstyle)) {
    this->needsetup = TRUE; // Force new a setup
    cc_string_set_text(this->prevfontname, cc_string_get_text(&fontspec->name));
    cc_string_set_text(this->prevfontstyle, cc_string_get_text(&fontspec->style));
  }
  if(fontspec->size != this->prevfontsize) {
    this->prevfontsize = fontspec->size;
    this->needsetup = TRUE;
  }

  if (!this->needsetup) return;
  this->needsetup = FALSE;

  this->widths.truncate(0);

  for (int i = 0; i < textnode->string.getNum(); i++) {

    const unsigned int length = textnode->string[i].getLength(); 
    float stringwidth = 0.0f;
    float kerningx = 0;
    float kerningy = 0;
    float advancex = 0;
    float advancey = 0;
    cc_glyph3d * prevglyph = NULL;

    const float * maxbbox;
    this->maxglyphbbox.makeEmpty();

    for (unsigned int strcharidx = 0; strcharidx < length; strcharidx++) {

      // Note that the "unsigned char" cast is needed to avoid 8-bit
      // chars using the highest bit (i.e. characters above the ASCII
      // set up to 127) be expanded to huge int numbers that turn
      // negative when casted to integer size.
      const uint32_t glyphidx = (const unsigned char) textnode->string[i][strcharidx];
      cc_glyph3d * glyph = cc_glyph3d_getglyph(glyphidx, fontspec);
      assert(glyph);

      maxbbox = cc_glyph3d_getboundingbox(glyph); // Get max height
      this->maxglyphbbox.extendBy(SbVec3f(0, maxbbox[0] * fontspec->size, 0));
      this->maxglyphbbox.extendBy(SbVec3f(0, maxbbox[1] * fontspec->size, 0));
              
      if (strcharidx > 0) 
        cc_glyph3d_getkerning(prevglyph, glyph, &kerningx, &kerningy);      
      cc_glyph3d_getadvance(glyph, &advancex, &advancey);
      
 
      stringwidth += (advancex + kerningx) * fontspec->size;
      prevglyph = glyph;

    }

    // Italic font might cause last letter to be outside bbox. Add width if needed.
    if (advancex < cc_glyph3d_getwidth(prevglyph)) 
      stringwidth += (cc_glyph3d_getwidth(prevglyph) - advancex) * fontspec->size;

    this->widths.append(stringwidth);
  }

  // Make sure boundingbox is updated if this method was called due to
  // a fontspec change.
  this->master->touch();

}
