/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/

#ifndef VISTA_ABUFFER_OIT_H
#define VISTA_ABUFFER_OIT_H
/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

#include "../../VistaOGLExtConfig.h"

#include <vector>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSceneGraph;
class VistaOpenGLNode;
class VistaGLSLShader;
class VistaTexture;
class IVistaABuffer;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This class allows you tho render objects with order independent transparency.
 * There for the A-Buffer is used to store fragments and later display them.
 *
 * @TODO !WARNING:
 *	This is an early Prototype.
 *	The interface of this class will probably change.
 */
class VISTAOGLEXTAPI VistaABufferOIT {
 public:
  /**************************************************************************/
  /* CONSTRUCTORS / DESTRUCTOR                                              */
  /**************************************************************************/
  VistaABufferOIT();
  virtual ~VistaABufferOIT();

  enum EABufferImplementation {
    ABUFFER_IMPLEMENTATION_ARRAY,
    ABUFFER_IMPLEMENTATION_LINKEDLIST,
    ABUFFER_IMPLEMENTATION_PAGES,
  };

  /**
   * This method will initializes the A-Buffer.
   * Because there are multiple A-Buffer-Implementations available,
   * you have to specify witch one should be used.
   * There for eABufferImp can have following values:
   *	ABUFFER_IMPLEMENTATION_ARRAY
   *	ABUFFER_IMPLEMENTATION_LINKEDLIST
   *	ABUFFER_IMPLEMENTATION_PAGES
   *
   * uiFragmentesPerPixel specifies how much memory should be preserved for each pixel.
   * If eABufferImp is ABUFFER_IMPLEMENTATION_LINKEDLIST or ABUFFER_IMPLEMENTATION_PAGES,
   * uiFragmentesPerPixel will specify the average amount of Fragments that can be
   * sored for each Pixel.
   * If eABufferImp is ABUFFER_IMPLEMENTATION_ARRAY, uiFragmentesPerPixel will
   * specify the maximum amount of Fragments that can be sored for each Pixel.
   *
   * uiPageSize is only used if eABufferImp is ABUFFER_IMPLEMENTATION_PAGES.
   * It specifies how many fragments should be stored in an single page.
   */
  bool Init(EABufferImplementation eABufferImp, unsigned int uiFragmentesPerPixel,
      unsigned int uiPageSize = 4);

  /**
   * This method will add two OpenGLNode to the RealRoot of the specified SceneGraph.
   * These OpenGLNodes will then be used to call ClearABuffer()/DisplayABuffer().
   *
   * @TODO fined a better way for calling ClearABuffer()/DisplayABuffer()
   *       before/after the transparent geometry is rendered.
   */
  bool AddToSceneGraph(VistaSceneGraph* pSG);

  /**
   * This method returns the OpenGLNodes.
   * They are used to call ClearABuffer()/DisplayABuffer().
   */
  VistaOpenGLNode* GetClearNode() const;
  VistaOpenGLNode* GetDisplayNode() const;

  /**
   * This method sets the OpenGLNodes, they are not added to a SceneGraph, though.
   * These OpenGLNodes will then be used to call ClearABuffer()/DisplayABuffer()
   */
  void SetOpenGLNodes(VistaOpenGLNode* pNodeClear, VistaOpenGLNode* pNodeDisplay);

  /**
   * returns a shader that can be used to store transparent fragments in the A-Buffer.
   */
  VistaGLSLShader* GetDefaultShader();

  /**
   * returns a new shader, that already has initialized fragment shader,
   * with specify methods needed to store Fragments in the A-Buffer.
   * For example "void StoreFragment( vec4 v4Color )" is specifies in these
   * shaders, there for it can be used in your fragment shader.
   */
  virtual VistaGLSLShader* CreateShaderPrototype();

  /**
   * If your are using a non default shader for OIT-Rendering, you have to
   * resister this shader, so all required uniform variables can be applied.
   */
  bool RegisterShader(VistaGLSLShader* pShader);
  bool DeregisterShader(VistaGLSLShader* pShader);

  /**
   * This method should be called to clear the A-Buffer ( delete all Fragments that are stored in
   * the A-Buffer ). Must be called before rendering the Scene. Make sure GL_VIEWPORT is set correct
   * when calling this method, because this method will resize the A-Buffer, to match the ViewPort
   * size.
   */
  void ClearABuffer();

  /**
   * This method should be called to display all fragments that have been
   * written to the A-Buffer. There for a Quad is drawn over the whole
   * ViewPort, to generate on Fragment for each Pixel. In the fragment shader
   * all fragments for one Pixel will be read from the A-Buffer and copied
   * into a local Array. While reading fragments from the A-Buffer the
   * depth-value of thees Fragments will be compared to the depth-value of the
   * opaque geometry and all fragments that are further away from the viewer
   * than opaque geometry will be discarded.
   * Afterward the Fragments are sort by dept and blend to compute the color
   * of the Pixel.
   *
   * !WARNING:
   *	The local Array to with fragments are copied only has a fixed size,
   *	there for only a fixed number of fragments can be displayed at once.
   *	The size of this Array is independent form the number of Fragments
   *	that can be stored in the A-Buffer, so increasing the size of the
   *	A-Buffer wont change anything.
   *	If this cause problems in your application you have to change the Value
   *	of MAX_NUM_FRAGMETS in VistaABufferOIT_Display_frag.glsl (Line 35).
   */
  void DisplayABuffer();

 protected:
  virtual bool InitABuffer(unsigned int uiFragmentesPerPixel, unsigned int uiPageSize);
  virtual bool InitShader();
  virtual bool InitOpaqueDepthTexture();

 protected:
  VistaGLSLShader* m_pDisplayShader;
  VistaGLSLShader* m_pDefaultShader;

  std::vector<VistaGLSLShader*> m_vecShader;

  VistaOpenGLNode* m_pNodeClear;
  VistaOpenGLNode* m_pNodeDisplay;

  IVistaABuffer* m_pABuffer;
  VistaTexture*  m_pOpaqueDepthTexture;

  int m_aViewport[4];
};
#endif // Include guard.
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
