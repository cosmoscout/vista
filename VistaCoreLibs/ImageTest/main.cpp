/*============================================================================*/
/*       1         2         3         4         5         6         7        */
/*3456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*============================================================================*/
/*                                             .                              */
/*                                               RRRR WW  WW   WTTTTTTHH  HH  */
/*                                               RR RR WW WWW  W  TT  HH  HH  */
/*      FileName :  main.cpp                     RRRR   WWWWWWWW  TT  HHHHHH  */
/*                                               RR RR   WWW WWW  TT  HH  HH  */
/*      Module   :  VistaTemplate                RR  R    WW  WW  TT  HH  HH  */
/*                                                                            */
/*      Project  :  ViSTA                          Rheinisch-Westfaelische    */
/*                                               Technische Hochschule Aachen */
/*      Purpose  :                                                            */
/*                                                                            */
/*                                                 Copyright (c)  1998-2016   */
/*                                                 by  RWTH-Aachen, Germany   */
/*                                                 All rights reserved.       */
/*                                             .                              */
/*============================================================================*/
/*
 * $Id: main.cpp,v 1.1 2006/10/30 16:01:11 av006ai Exp $
 */

#include <GL/glew.h>

#include <VistaTools/VistaBasicProfiler.h>

#include <VistaAspects/VistaPropertyList.h>

#include "VistaBase/VistaTimeUtils.h"
#include "VistaBase/VistaTimer.h"
#include "VistaKernel/DisplayManager/VistaDisplayManager.h"
#include "VistaKernel/DisplayManager/VistaSimpleTextOverlay.h"
#include "VistaKernel/DisplayManager/VistaTextEntity.h"
#include "VistaKernel/DisplayManager/VistaWindow.h"
#include "VistaKernel/DisplayManager/VistaWindowingToolkit.h"
#include "VistaKernel/EventManager/VistaEventHandler.h"
#include "VistaKernel/EventManager/VistaEventManager.h"
#include "VistaKernel/EventManager/VistaSystemEvent.h"
#include "VistaKernel/GraphicsManager/VistaGLTexture.h"
#include "VistaKernel/GraphicsManager/VistaImage.h"
#include "VistaKernel/GraphicsManager/VistaOpenGLDraw.h"
#include "VistaKernel/GraphicsManager/VistaSceneGraph.h"
#include "VistaKernel/GraphicsManager/VistaTransformNode.h"
#include "VistaKernel/InteractionManager/VistaKeyboardSystemControl.h"
#include "VistaKernel/VistaSystem.h"
#include <VistaBase/VistaVectorMath.h>

#pragma warning(push, 0)
#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGSHLChunk.h>
#include <OpenSG/OSGSHLParameterChunk.h>
#pragma warning(pop)
#include "VistaKernel/GraphicsManager/VistaGeometryFactory.h"
#include "VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h"
#include "VistaKernelOpenSGExt/VistaOpenSGModelOptimizer.h"
#include "VistaKernelOpenSGExt/VistaOpenSGPerMaterialShader.h"
#include "VistaTools/VistaRandomNumberGenerator.h"

class DrawCallback : public IVistaOpenGLDraw {
 public:
  DrawCallback(VistaGLTexture oTexture, const float nExtents, const float nRepeats)
      : m_oTexture(oTexture)
      , m_nExtents(nExtents)
      , m_nRepeats(nRepeats) {
  }

  virtual bool Do() {
    VistaBasicProfiler::GetSingleton()->StartSection("QuadDraw");
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    // VistaTimer oTimer;
    m_oTexture.Bind();
    // vstr::outi() << "Bind took " << vstr::formattime( 1000000.0f * oTimer.GetLifeTime(), 3 ) <<
    // "us" << std::endl;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-0.5f * m_nExtents, -0.5f * m_nExtents, 0.0f);

    glTexCoord2f(m_nRepeats, 0);
    glVertex3f(0.5f * m_nExtents, -0.5f * m_nExtents, 0.0f);

    glTexCoord2f(m_nRepeats, m_nRepeats);
    glVertex3f(0.5f * m_nExtents, 0.5f * m_nExtents, 0.0f);

    glTexCoord2f(0, m_nRepeats);
    glVertex3f(-0.5f * m_nExtents, 0.5f * m_nExtents, 0.0f);
    glEnd();
    VistaBasicProfiler::GetSingleton()->StopSection("QuadDraw");
    return true;
  }

  virtual bool GetBoundingBox(VistaBoundingBox& bb) {
    bb.Expand(1.0f);
    return true;
  }

  VistaGLTexture m_oTexture;
  float          m_nExtents;
  float          m_nRepeats;
};

class KeyHandler : public VistaKeyboardSystemControl::IVistaDirectKeySink {
 public:
  KeyHandler(VistaSystem* pSystem, const VistaGLTexture& oTexture1, const VistaGLTexture& oTexture2)
      : m_oTexture1(oTexture1)
      , m_oTexture2(oTexture2)
      , m_pSystem(pSystem) {
  }

  virtual bool HandleKeyPress(int nKey, int nMod, bool bIsKeyRepeat = false) {
    switch (nKey) {
    case '1': {
      m_oTexture1.SetMinFilter(GL_NEAREST);
      m_oTexture2.SetMinFilter(GL_NEAREST);
      m_oTexture1.SetMagFilter(GL_NEAREST);
      m_oTexture2.SetMagFilter(GL_NEAREST);
      break;
    }
    case '2': {
      m_oTexture1.SetMinFilter(GL_LINEAR);
      m_oTexture2.SetMinFilter(GL_LINEAR);
      m_oTexture1.SetMagFilter(GL_LINEAR);
      m_oTexture2.SetMagFilter(GL_LINEAR);
      break;
    }
    case '3': {
      m_oTexture1.SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
      m_oTexture2.SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
      m_oTexture1.SetMagFilter(GL_LINEAR);
      m_oTexture2.SetMagFilter(GL_LINEAR);
      break;
    }

    case '4': {
      m_oTexture1.SetWrapS(GL_CLAMP);
      m_oTexture1.SetWrapT(GL_CLAMP);
      m_oTexture2.SetWrapS(GL_CLAMP);
      m_oTexture2.SetWrapT(GL_CLAMP);
      break;
    }
    case '5': {
      m_oTexture1.SetWrapS(GL_REPEAT);
      m_oTexture1.SetWrapT(GL_REPEAT);
      m_oTexture2.SetWrapS(GL_REPEAT);
      m_oTexture2.SetWrapT(GL_REPEAT);
      break;
    }
    case '6': {
      m_oTexture1.SetWrapS(GL_MIRRORED_REPEAT);
      m_oTexture1.SetWrapT(GL_MIRRORED_REPEAT);
      m_oTexture2.SetWrapS(GL_MIRRORED_REPEAT);
      m_oTexture2.SetWrapT(GL_MIRRORED_REPEAT);
      break;
    }

    case '7': {
      m_oTexture1.SetTextureEnvMode(GL_MODULATE);
      m_oTexture2.SetTextureEnvMode(GL_MODULATE);
      break;
    }
    case '/': {
      m_oTexture1.SetTextureEnvMode(GL_REPLACE);
      m_oTexture2.SetTextureEnvMode(GL_REPLACE);
      break;
    }

    case '8': {
      float nCurrent = m_oTexture1.GetLodBias();
      float nNew     = nCurrent + 0.5f;
      std::cout << "Changing LodBias from " << nCurrent << " to " << nNew << std::endl;
      m_oTexture1.SetLodBias(nNew);
      m_oTexture2.SetLodBias(nNew);
      break;
    }
    case '(': {
      float nCurrent = m_oTexture1.GetLodBias();
      float nNew     = nCurrent - 0.5f;
      std::cout << "Changing LodBias from " << nCurrent << " to " << nNew << std::endl;
      m_oTexture1.SetLodBias(nNew);
      m_oTexture2.SetLodBias(nNew);
      break;
    }
    case '9': {
      float nMaxAnistropy;
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &nMaxAnistropy);
      float nCurrent = m_oTexture1.GetAnisotropy();
      float nNew     = 2 * nCurrent;
      if (nNew > nMaxAnistropy)
        nNew = 1.0f;
      std::cout << "Changing anisotropy from " << nCurrent << " to " << nNew << std::endl;
      m_oTexture1.SetAnisotropy(nNew);
      m_oTexture2.SetAnisotropy(nNew);
      break;
    }

    case '0': {
      m_oTexture1.CreateMipmaps();
      m_oTexture2.CreateMipmaps();
      m_oTexture1.SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
      m_oTexture2.SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
      break;
    }

    case 't': {
      VistaWindow* pWindow = m_pSystem->GetDisplayManager()->GetWindowByName("RTT_WINDOW");
      if (pWindow == NULL)
        pWindow = m_pSystem->GetDisplayManager()->GetWindowByName("MAIN_WINDOW");
      IVistaWindowingToolkit* pWinTK = m_pSystem->GetDisplayManager()->GetWindowingToolkit();

      VistaTimer oTimer;
      VistaImage oImage = pWinTK->GetRGBImage(pWindow);
      std::cout << "Reading RGB took " << vstr::formattime(1000.0 * oTimer.GetLifeTime(), 3) << "ms"
                << std::endl;
      oTimer.ResetLifeTime();
      oImage.WriteToFile("testshot.jpg");
      std::cout << "Saving RGB took " << vstr::formattime(1000.0 * oTimer.GetLifeTime(), 3) << "ms"
                << std::endl;
      oTimer.ResetLifeTime();

      // VistaImage oDepthImage = pWinTK->GetDepthImage( pWindow );
      // std::cout << "Reading Depth took " << vstr::formattime( 1000.0 * oTimer.GetLifeTime(), 3 )
      // << "ms" << std::endl; oTimer.ResetLifeTime(); oDepthImage.ReformatImage( GL_RGB,
      // GL_UNSIGNED_BYTE ); std::cout << "Convert Depth took " << vstr::formattime( 1000.0 *
      // oTimer.GetLifeTime(), 3 ) << "ms" << std::endl; oTimer.ResetLifeTime();
      // oDepthImage.WriteToFile( "testshot_d.png" );
      // std::cout << "Saving Depth took " << vstr::formattime( 1000.0 * oTimer.GetLifeTime(), 3 )
      // << "ms" << std::endl;

      break;
    }

    case 'u': {
      VistaWindow* pWindow = m_pSystem->GetDisplayManager()->GetWindowsConstRef().begin()->second;
      IVistaWindowingToolkit* pWinTK = m_pSystem->GetDisplayManager()->GetWindowingToolkit();

      VistaImage oDepthImage = pWinTK->GetDepthImage(pWindow);
      m_oTexture1.SetData(oDepthImage);

      break;
    }

    default:
      return false;
    }

    return true;
  }

  VistaGLTexture m_oTexture1;
  VistaGLTexture m_oTexture2;
  VistaSystem*   m_pSystem;
};

class Handler : public VistaEventHandler {
 public:
  Handler(VistaSystem* pSystem)
      : m_pSystem(pSystem)
      , m_nFrameCount(0)
      , m_pWin1TextOverlay(NULL)
      , m_pWin1Text(NULL)
      , m_pWin2TextOverlay(NULL)
      , m_pWin2Text(NULL) {
    m_pSystem->GetEventManager()->AddEventHandler(
        this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_POSTAPPLICATIONLOOP);

    // VistaWindow* pWindow = m_pSystem->GetDisplayManager()->GetWindowByName( "RTT_WINDOW" );
    // if( pWindow != NULL )
    //{
    //	m_pWin1TextOverlay = new VistaSimpleTextOverlay( pWindow->GetViewport( 0 ) );
    //	m_pWin1Text = m_pSystem->GetDisplayManager()->CreateTextEntity();
    //	m_pWin1Text->SetText( "Frame:" );
    //	m_pWin1Text->SetXPos( 1 );
    //	m_pWin1Text->SetYPos( 4 );
    //	m_pWin1Text->SetFont( "SANS", 80 );
    //	m_pWin1Text->SetColor( VistaColor::RED );
    //	m_pWin1TextOverlay->AddText( m_pWin1Text );
    //}
    // pWindow = m_pSystem->GetDisplayManager()->GetWindowByName( "MAIN_WINDOW" );
    // if( pWindow != NULL )
    //{
    //	m_pWin2TextOverlay = new VistaSimpleTextOverlay( pWindow->GetViewport( 0 ) );
    //	m_pWin2Text = m_pSystem->GetDisplayManager()->CreateTextEntity();
    //	m_pWin2Text->SetText( "Frame:" );
    //	m_pWin2Text->SetXPos( 1 );
    //	m_pWin2Text->SetYPos( 4 );
    //	m_pWin2Text->SetFont( "SANS", 80 );
    //	m_pWin2Text->SetColor( VistaColor::RED );
    //	m_pWin2TextOverlay->AddText( m_pWin2Text );
    //}

    // VistaSceneGraph* pSG = m_pSystem->GetGraphicsManager()->GetSceneGraph();
    // VistaGeometryFactory oFac( pSG );
    // m_pShaderTestGeom = oFac.CreateBox( 1, 1, 1, 1, 1, 1, VistaColor::WHITE );
    // VistaTransformNode* pTrans = pSG->NewTransformNode( pSG->GetRoot() );
    // pTrans->SetTranslation( 0.5, 4, -7 );
    // pSG->NewGeomNode( pTrans, m_pShaderTestGeom );

    // std::string sVertexShaderFile = "testshader.vert";
    // std::string sFragmentShaderFile = "testshader_color.frag";

    // m_pShader = new VistaOpenSGPerMaterialShader();
    // m_pShader->SetShadersFromFile( sVertexShaderFile, sFragmentShaderFile );
    // float a3fColor[] = { 0.3f, 0.3f, 0 };
    // VistaOpenSGPerMaterialShader::CShaderUniformParam oParam( "v3Color", a3fColor, 3 );
    // VistaOpenSGPerMaterialShader::CShaderUniformParam oParam2( "warnme", a3fColor, 3 );
    // m_pShader->AddUniformParameter( oParam );
    // m_pShader->AddUniformParameter( oParam2 );
    // m_pShader->ApplyToGeometry( m_pShaderTestGeom );

    /*	VistaOpenSGGeometryData* pGeomData = static_cast< VistaOpenSGGeometryData* >(
       m_pShaderTestGeom->GetData() ); osg::GeometryPtr pGeom = pGeomData->GetGeometry();
            m_pOsgTestShader = osg::SHLChunk::create();
            std::ifstream oVertexShader( sVertexShaderFile.c_str() );
            if( oVertexShader.good() == false )
            {
                    vstr::warnp() << "[VistaOpenSGPerMaterialShader]: Could not load vertex shader
       file ["
                                    << sVertexShaderFile << "]" << std::endl;
                    return;
            }
            std::ifstream oFragmentShader( sFragmentShaderFile.c_str() );
            if( oFragmentShader.good() == false )
            {
                    vstr::warnp() << "[VistaOpenSGPerMaterialShader]: Could not load fragment shader
       file ["
                                    << sFragmentShaderFile << "]" << std::endl;
                    return;
            }
            beginEditCP( m_pOsgTestShader );
            if( m_pOsgTestShader->readVertexProgram( oVertexShader ) == false )
            {
                    vstr::warnp() << "[VistaOpenSGPerMaterialShader]: Could not parse vertex shader
       file ["
                            << sVertexShaderFile << "]" << std::endl;
                    endEditCP( m_pOsgTestShader );
                    return;
            }
            if( m_pOsgTestShader->readFragmentProgram( oFragmentShader ) == false )
            {
                    vstr::warnp() << "[VistaOpenSGPerMatrialShader]: Could not parse fragment shader
       file ["
                            << sFragmentShaderFile << "]" << std::endl;
                    endEditCP( m_pOsgTestShader );
                    return;
            }

            osg::Vec3f v3Color( 0.5f, 0.5f, 0.0f );
            m_pOsgTestShader->setUniformParameter( "v3Color", v3Color );
            m_pOsgTestShader->setUniformParameter( "warnme", v3Color );
            endEditCP( m_pOsgTestParams );*/

    // endEditCP( m_pOsgTestShader );
    // osg::Vec3f v3Color( 0.5f, 0.5f, 0.0f );
    // m_pOsgTestParams = osg::SHLParameterChunk::create();
    // beginEditCP( m_pOsgTestParams );
    // m_pOsgTestParams->setUniformParameter( "v3Color", v3Color );
    // m_pOsgTestParams->setUniformParameter( "warnme", v3Color );
    // m_pOsgTestParams->setSHLChunk( m_pOsgTestShader );
    // endEditCP( m_pOsgTestParams );

    // osg::ChunkMaterialPtr pMat = osg::ChunkMaterialPtr::dcast( pGeom->getMaterial() );
    // assert( pMat != osg::NullFC );
    // beginEditCP( pMat );
    // pMat->addChunk( m_pOsgTestShader );
    ////pMat->addChunk( m_pOsgTestParams );
    // endEditCP( pMat );
  }

  virtual void HandleEvent(VistaEvent* pEvent) {
    // if( true )
    //{
    //	VistaWindow* pWindow = m_pSystem->GetDisplayManager()->GetWindowByName( "WINDOW_OFFSCREEN"
    //); 	if( pWindow != NULL )
    //	{
    //		VistaTimer oTimer;
    //		VistaImage oImage = pWindow->ReadRGBImage();
    //		std::cout << "Reading RGB took " << vstr::formattime( 1000.0 * oTimer.GetLifeTime(), 3 )
    //<< "ms" << std::endl; 		oTimer.ResetLifeTime(); 		oImage.WriteToFile( "screenshot" +
    //VistaConversion::ToString(m_nFrameCount) + "_rtt.jpg" ); 		std::cout << "Saving RGB took " <<
    //vstr::formattime( 1000.0 * oTimer.GetLifeTime(), 3 ) << "ms" << std::endl;
    //		oTimer.ResetLifeTime();
    //	}
    //	pWindow = m_pSystem->GetDisplayManager()->GetWindowByName( "WINDOW_SIMPLE" );
    //	if( pWindow != NULL )
    //	{
    //		VistaTimer oTimer;
    //		VistaImage oImage = pWindow->ReadRGBImage();
    //		std::cout << "Reading RGB took " << vstr::formattime( 1000.0 * oTimer.GetLifeTime(), 3 )
    //<< "ms" << std::endl; 		oTimer.ResetLifeTime(); 		oImage.WriteToFile( "screenshot" +
    //VistaConversion::ToString(m_nFrameCount) + "_w.jpg" ); 		std::cout << "Saving RGB took " <<
    //vstr::formattime( 1000.0 * oTimer.GetLifeTime(), 3 ) << "ms" << std::endl;
    //		oTimer.ResetLifeTime();
    //	}
    //}
    //++m_nFrameCount;
    // std::string sText = "Frame: " + VistaConversion::ToString( m_nFrameCount );
    // if( m_pWin1Text )
    //	m_pWin1Text->SetText( sText );
    // if( m_pWin2Text )
    //	m_pWin2Text->SetText( sText );

    // if( m_nFrameCount % 2 == 0 )
    //	m_pSystem->GetGraphicsManager()->SetBackgroundColor( VistaColor::RED );
    // else
    //	m_pSystem->GetGraphicsManager()->SetBackgroundColor( VistaColor::GREEN );

    // VistaTimeUtils::Sleep( 1000 );

    VistaRandomNumberGenerator* pRNG = VistaRandomNumberGenerator::GetStandardRNG();

    // osg::Vec3f v3Rand( pRNG->GenerateFloat( 0, 1 ), pRNG->GenerateFloat( 0, 1 ),
    // pRNG->GenerateFloat( 0, 1 ) ); beginEditCP( m_pOsgTestShader,
    // osg::SHLChunk::ParametersFieldMask ); m_pOsgTestShader->setUniformParameter( "v3Color", v3Rand
    // ); endEditCP( m_pOsgTestShader, osg::SHLChunk::ParametersFieldMask );

    // float a3fColor[] = { pRNG->GenerateFloat( 0, 1 ), pRNG->GenerateFloat( 0, 1 ),
    // pRNG->GenerateFloat( 0, 1 ) }; VistaOpenSGPerMaterialShader::CShaderUniformParam oParam(
    // "v3Color", a3fColor, 3 ); m_pShader->AddUniformParameter( oParam );
  }

  VistaGeometry*                m_pShaderTestGeom;
  osg::SHLChunkRefPtr           m_pOsgTestShader;
  osg::SHLParameterChunkPtr     m_pOsgTestParams;
  VistaOpenSGPerMaterialShader* m_pShader;

  VistaSystem*            m_pSystem;
  int                     m_nFrameCount;
  VistaSimpleTextOverlay* m_pWin1TextOverlay;
  IVistaTextEntity*       m_pWin1Text;
  VistaSimpleTextOverlay* m_pWin2TextOverlay;
  IVistaTextEntity*       m_pWin2Text;
};

int main(int argc, char* argv[]) {
  VistaSystem* pSystem = new VistaSystem();

  if (pSystem->Init(argc, argv) == false)
    return -1;

  VistaSceneGraph* pSG = pSystem->GetGraphicsManager()->GetSceneGraph();

  // VistaImage oImage;
  // oImage.LoadFromFile( "checker.tga" );

  // VistaImage oImage2 = oImage;
  // oImage.LoadFromFile( "test.tga" );
  ////std::vector<unsigned char> vecData;
  ////int nSizeX = 32;
  ////int nSizeY = 32;
  ////for( int x = 0; x < nSizeX; ++x )
  ////{
  ////	for( int y = 0; y < nSizeY; ++y )
  ////	{
  ////		vecData.push_back( (unsigned char)( 255.0f * (float)x / (float)nSizeX ) );
  ////		vecData.push_back( (unsigned char)( 255.0f * (float)y / (float)nSizeY ) );
  ////		vecData.push_back( (unsigned char)0 );
  ////	}
  ////}
  //////oImage.Set2DData( nSizeX, nSizeY, &vecData[0], GL_RGB, GL_UNSIGNED_BYTE );

  //// oImage.WriteToFile( "save.tga" );

  //	VistaGLTexture oTexture;
  // oTexture.SetData( oImage, true );
  // oTexture.SetMinFilter( GL_LINEAR_MIPMAP_LINEAR );
  // oTexture.SetMagFilter( GL_LINEAR );
  // oTexture.SetAnisotropy( 0 );

  //
  // VistaTransformNode* pTrans = pSG->NewTransformNode( pSG->GetRoot() );
  // pTrans->SetTranslation( 0, 0, -7 );
  // pSG->NewOpenGLNode( pTrans, new DrawCallback( oTexture, 2, 2 ) );

  // for( int i = 0; i < 100; ++i )
  //{
  //	pTrans = pSG->NewTransformNode( pSG->GetRoot() );
  //	pTrans->SetTranslation( -0.5f*i+i, 1, -6 );
  //	pSG->NewOpenGLNode( pTrans, new DrawCallback( oTexture, 0.5, 1 ) );
  //}
  //
  // VistaGLTexture oTexture2;
  // oTexture2.SetData( oImage2, true );
  // oTexture2.SetMinFilter( GL_LINEAR_MIPMAP_LINEAR );
  // oTexture2.SetMagFilter( GL_LINEAR );
  // oTexture2.SetAnisotropy( 16 );

  // pTrans = pSG->NewTransformNode( pSG->GetRoot() );
  // pTrans->SetTranslation( 0, 0, -8 );
  // pSG->NewOpenGLNode( pTrans, new DrawCallback( oTexture2, 100, 1000 ) );

  // VistaViewport* pRTTViewport = pSystem->GetDisplayManager()->GetViewportByName( "VIEWPORT_RTT"
  // ); if( pRTTViewport != NULL )
  //{
  //	VistaGLTexture* pTexture = pRTTViewport->GetTextureTarget();

  //	pTrans = pSG->NewTransformNode( pSG->GetRoot() );
  //	pTrans->SetTranslation( 0, 0, -1 );
  //	pSG->NewOpenGLNode( pTrans, new DrawCallback( *pTexture, 1.6f, 1 ) );
  //}

  pSG->GetRoot()->AddChild(pSG->LoadNode("D:/dev/InsideSceneData/common/scenery/ground.ac"));

  // pSG->GetRoot()->AddChild( VistaOpenSGModelOptimizer::LoadAutoOptimizedFile( pSG,
  // "N:/InsideSceneData/Cars/mclaren.ac", "tmp" ) );
  pSG->GetRoot()->AddChild(pSG->LoadNode("D:/dev/InsideSceneData/common/testcube.ac"));

  Handler* pHandler = new Handler(pSystem);

  // pSystem->GetKeyboardSystemControl()->SetDirectKeySink( new KeyHandler( pSystem, oTexture,
  // oTexture2 ) );

  pSystem->Run();

  return 0;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "main.cpp"                                                    */
/*============================================================================*/
