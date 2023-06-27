/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
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

#ifndef _VISTA2DDRAWINGOBJECT_H
#define _VISTA2DDRAWINGOBJECT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <list>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/*	GENERAL NOTE:
  Due to the many different display resolutions in which ViSTA is supposed to work
  all 2D screen positions for the following 2D objects are given as float tuples
  (x,y), where 0.<=x,y<=1.0. (0.0,0.0) specifies the lower left corner of the screen
  whereas (1.0,1.0) specifies (guess what) the upper right one.
*/
// class IVistaGraphicsBridge;
class IVistaDisplayBridge;

//*************************************************************************
//***						Vista2DDrawingObject
//***
//*************************************************************************
class VISTAKERNELAPI Vista2DDrawingObject {
 public:
  enum Vista2DObjectType {
    VISTA2D_TEXT,
    VISTA2D_BITMAP,
    VISTA2D_LINE,
    VISTA2D_RECTANGLE,
    VISTA2D_COMPOUND
  };

  Vista2DDrawingObject();
  Vista2DDrawingObject(
      float fPosX, float fPosY, unsigned char nRed, unsigned char nGreen, unsigned char nBlue);

  virtual ~Vista2DDrawingObject();

  /** Set the 2D screen position
   * @param float fPosX : x position of 2D element (from [0..1] where 0 marks left border)
   * @param float fPosY : y position of 2D element (from [0..1] where 0 marks top  border)
   * @return bool true/false
   */
  virtual bool SetPosition(float fPosX, float fPosY);
  /** Get the 2D screen position
   * @param float& fPosX : x position of 2D element (from [0..1] where 0 marks left border)
   * @param float& fPosY : y position of 2D element (from [0..1] where 0 marks top  border)
   * @return bool true/false
   */
  virtual bool GetPosition(float& fPosX, float& fPosY) const;
  /** Set color of 2D screen element
   * @param unsigned char nRed : red fraction of color
   * @param unsigned char nGreen : green fraction of color
   * @param unsigned char nBlue : blue fraction of color
   * @return bool true/false
   */
  virtual bool SetColor(unsigned char nRed, unsigned char nGreen, unsigned char nBlue);
  /** Get color of 2D screen element
   * @param unsigned char& nRed : red fraction of color
   * @param unsigned char& nGreen : green fraction of color
   * @param unsigned char& nBlue : blue fraction of color
   * @return bool true/false
   */
  virtual bool GetColor(unsigned char& nRed, unsigned char& nGreen, unsigned char& nBlue);
  /** Draw the 2D element (implementation is application specific)
   * @return bool true/false
   */
  // virtual bool Draw() =0;
  /** Retrieve internal object type
   * @return Vista2DObjectType : Internal type of 2D element
   * @see Vista2DObjectType
   */
  virtual Vista2DObjectType GetType();
  /** Retrieve object's init status
   * Object will be drawn only after successful init
   * @return bool true/false
   */
  virtual bool IsInitialized();

  virtual bool GetEnabled() const;
  virtual void SetEnabled(bool bEnabled);

 protected:
  float         m_fXPos;
  float         m_fYPos;
  unsigned char m_nRed;
  unsigned char m_nGreen;
  unsigned char m_nBlue;

  Vista2DObjectType m_Type;
  bool              m_bIsInitialized;
  bool              m_bEnabled;
};

class VISTAKERNELAPI Vista2DCompound : public Vista2DDrawingObject {
 public:
  Vista2DCompound();
  Vista2DCompound(float fPosX, float fPosY);
  virtual ~Vista2DCompound();

  virtual bool SetPosition(float fPosX, float fPosY);

 protected:
 private:
  std::list<Vista2DDrawingObject*> m_liMembers;
};

//*************************************************************************
//***                          Vista2DText ***
//*************************************************************************
class VISTAKERNELAPI Vista2DText : public Vista2DDrawingObject {
 public:
  enum Vista2DTextFontFamily { INVALID = -1, SANS = 0, TYPEWRITER, SERIF };

  Vista2DText();
  virtual ~Vista2DText();

  /**@todo: change color input to float*/
  /** Init text object
   * @param string strText : Text to be displayed
   * @param float fPosX : x position of 2D element (from [0..1] where 0 marks left border)
   * @param float fPosY : y position of 2D element (from [0..1] where 0 marks top  border)
   * @param unsigned char nRed : red fraction of color
   * @param unsigned char nGreen : green fraction of color
   * @param unsigned char nBlue : blue fraction of color
   * @return bool true/false
   */
  virtual bool Init(const std::string& strText, float fPosX, float fPosY, unsigned char nRed = 255,
      unsigned char nGreen = 255, unsigned char nBlue = 0, unsigned int size = 20,
      Vista2DTextFontFamily family = SANS);
  /** Set text to be displayed
   * @param string strNewText : Text to be displayed
   * @return bool true/false
   */
  bool SetText(const std::string& strNewText);
  /** Retrieve text currently displayed
   * @return string : Currently displayed text
   */
  std::string        GetText() const;
  void               GetText(std::string& sTarget) const;
  const std::string& GetTextConstRef() const;

  const char* GetTextC() const;

  int  GetFontIndex() const;
  void SetFontIndex(int iIndex);

  bool SetPosition(float fPosX, float fPosY);
  bool GetPosition(float& fPosX, float& fPosY) const;
  bool SetPosition(float fPosX, float fPosY, float fPosZ);
  bool GetPosition(float& fPosX, float& fPosY, float& fPosZ) const;
  bool SetFontFamily(Vista2DTextFontFamily family);
  bool GetFontFamily(Vista2DTextFontFamily& family) const;
  bool SetSize(float size);
  bool GetSize(float& size) const;

  bool IsIn3DMode();
  void Set3DMode(bool b3DMode);

 protected:
  std::string           m_strText;
  Vista2DTextFontFamily m_family;
  float                 m_uiSize;
  int                   m_iFontIndex; /**< default: 0 */
  bool                  m_b3DMode;
  float                 m_fZPos;
};

//*************************************************************************
//***                          Vista2DBitmap							***
//*************************************************************************
class VISTAKERNELAPI Vista2DBitmap : public Vista2DDrawingObject {
 public:
  Vista2DBitmap(IVistaDisplayBridge* pGrBridge);
  // Vista2DBitmap(std::string strFName, float fPosX, float fPosY);
  virtual ~Vista2DBitmap();
  /** Init bitmap object
   * @param string strFName: Name of the bitmap file to be loaded and displayed
   * @param float fPosX : x position of 2D element (from [0..1] where 0 marks left border)
   * @param float fPosY : y position of 2D element (from [0..1] where 0 marks top  border)
   * @return bool true/false
   */
  virtual bool Init(const std::string& strFName, float fPosX, float fPosY);

  /** Set the bitmap to be displayed
   * If this returns false you'll have to re-init the bitmap object before the bitmap is seen again
   * @param string strNewFName : Name of the new bitmap file to be loaded and displayed
   * @return bool true/false
   */
  virtual bool SetBitmap(const std::string& strNewFName);
  /** Retrieve the name of the currently displayed bitmap
   * @return string : Name of the currently used bitmap file
   */
  std::string GetBitmapName() const;
  /** Retrieve current bitmap's dimensions
   * @param int& nHeight : bitmap's height within original bitmap file
   * @param int& nWidth	: bitmap's width  within original bitmap file
   * @return bool true/false
   */

  const char* GetBitmapNameC() const;

  virtual bool GetDimensions(int& nWidth, int& nHeight);
  /** Retrieve current bitmap's windows dimensions
   * @param int& nWidth	: bitmap's width in window dimensions
   * @param int& nHeight : bitmap's height in window dimensions
   * @return bool true/false
   */
  bool GetWindowDimensions(float& fUserWidth, float& fUserHeight);
  /** Set window dimenstion for bitmap
   * @param float fWidth  : bitmap's width in window dimensions  (0.<= fWidth  <=1.0)
   * @param float fHeight : bitmap's height in window dimensions (0.<= fHeight <=1.0)
   */
  virtual bool SetWindowDimensions(float fWidth, float fHeight);
  /** Retrieve update flag
   * @return bool true/false
   */
  bool NeedsUpdate();
  /** Set update flag to false
   * @return void
   */
  void Update();
  /** Set the 2D screen position
   * @param float fPosX : x position of 2D element (from [0..1] where 0 marks left border)
   * @param float fPosY : y position of 2D element (from [0..1] where 0 marks top  border)
   * @return bool true/false
   */
  virtual bool SetPosition(float fPosX, float fPosY);

  /**
   * @deprecated
   */
  bool SetColor(unsigned char nRed, unsigned char nGreen, unsigned char nBlue);

 protected:
  Vista2DBitmap();

  IVistaDisplayBridge* m_pDispBridge;
  std::string          m_strBitmapName;
  bool                 m_bNeedsUpdate;
  int                  m_nHeight;
  int                  m_nWidth;
  bool                 m_bAlphaChannel;
  float                m_fWindowWidth;
  float                m_fWindowHeight;
  unsigned char*       m_pBitmapData;
};

//*************************************************************************
//***                          Vista2DRectangle						***
//*************************************************************************
class VISTAKERNELAPI Vista2DRectangle : public Vista2DDrawingObject {
 public:
  Vista2DRectangle();
  virtual ~Vista2DRectangle();
  /** Init bitmap object
   * @param string strFName: Name of the bitmap file to be loaded and displayed
   * @param float fPosX : x position of 2D element (from [0..1] where 0 marks left border)
   * @param float fPosY : y position of 2D element (from [0..1] where 0 marks top  border)
   * @param unsigned char nRed : red fraction of color
   * @param unsigned char nGreen : green fraction of color
   * @param unsigned char nBlue : blue fraction of color
   * @return bool true/false
   */
  virtual bool Init(float fPosX, float fPosY, float fWidth, float fHeight, unsigned char nRed = 255,
      unsigned char nGreen = 255, unsigned char nBlue = 0);

  /** Set rectangles dimenstions
   * @param float fWidth  : rect's width (0.<= fWidth  <=1.0)
   * @param float fHeight : rect's height (0.<= fHeight <=1.0)
   */
  bool SetDimensions(float fWidth, float fHeight);
  bool GetDimensions(float& fWidth, float& fHeight);
  /** Retrieve update flag
   * @return bool true/false
   */
  bool NeedsUpdate();
  /** Set update flag to false
   * @return void
   */
  void Update();
  /** Set the 2D screen position
   * @param float fPosX : x position of 2D element (from [0..1] where 0 marks left border)
   * @param float fPosY : y position of 2D element (from [0..1] where 0 marks top  border)
   * @return bool true/false
   */
  bool SetPosition(float fPosX, float fPosY);
  bool GetPosition(float& fPosX, float& fPosY) const;

  bool SetColor(unsigned char nRed, unsigned char nGreen, unsigned char nBlue);

 protected:
  bool  m_bNeedsUpdate;
  float m_fHeight;
  float m_fWidth;
};

//*************************************************************************
//***                          Vista2DLine ***
//*************************************************************************
class VISTAKERNELAPI Vista2DLine : public Vista2DDrawingObject {
 public:
  Vista2DLine();
  // Vista2DLine(float nX1, float nY1, float nX2, float nY2, unsigned char nRed, unsigned char
  // nGreen, unsigned char nBlue);
  virtual ~Vista2DLine();

  /** Init line object
   * @param float fPosX1 : x position of start point (from [0..1] where 0 marks left border)
   * @param float fPosY1 : y position of start point (from [0..1] where 0 marks top  border)
   * @param float fPosX2 : x position of end point (from [0..1] where 0 marks left border)
   * @param float fPosY2 : y position of end point (from [0..1] where 0 marks top  border)
   * @param unsigned char nRed : red fraction of color
   * @param unsigned char nGreen : green fraction of color
   * @param unsigned char nBlue : blue fraction of color
   * @return bool true/false
   */
  virtual bool Init(float fPosX1, float fPosY1, float fPosX2, float fPosY2,
      unsigned char nRed = 255, unsigned char nGreen = 255, unsigned char nBlue = 0);
  /** Set coordinates of  line's start point
   * @param float fPosX1 : x position of start point (from [0..1] where 0 marks left border)
   * @param float fPosY1 : y position of start point (from [0..1] where 0 marks top  border)
   * @return bool true/false
   */
  bool SetStartPoint(float fPosX, float fPosY);
  /** Retrieve coordinates of  line's start point
   * @param float& fPosX : x position of start point (from [0..1] where 0 marks left border)
   * @param float& fPosY : y position of start point (from [0..1] where 0 marks top  border)
   * @return bool true/false
   */
  bool GetStartPoint(float& fPosX, float& fPosY);
  /** Set coordinates of  line's end point
   * @param float fPosX : x position of end point (from [0..1] where 0 marks left border)
   * @param float fPosY : y position of end point (from [0..1] where 0 marks top  border)
   * @return bool true/false
   */
  bool SetEndPoint(float fPosX, float fPosY);
  /** Retrieve coordinates of  line's end point
   * @param float& fPosX : x position of end point (from [0..1] where 0 marks left border)
   * @param float& fPosY : y position of end point (from [0..1] where 0 marks top  border)
   * @return bool true/false
   */
  bool GetEndPoint(float& fPosX, float& fPosY);

 protected:
  float m_fXPos2;
  float m_fYPos2;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTA2DDRAWINGOBJECT_H
