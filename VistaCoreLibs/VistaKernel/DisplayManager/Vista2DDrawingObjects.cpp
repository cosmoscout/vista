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

#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
//#include <VistaKernel/GraphicsManager/VistaGraphicsBridge.h>
#include <VistaKernel/DisplayManager/VistaDisplayBridge.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

//*************************************************************************
//***						Vista2DDrawingObject
//***
//*************************************************************************

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
Vista2DDrawingObject::Vista2DDrawingObject() {
  m_fXPos          = 0.0;
  m_fYPos          = 0.0;
  m_nRed           = 0x0;
  m_nGreen         = 0x0;
  m_nBlue          = 0x0;
  m_bIsInitialized = false;
  m_bEnabled       = true;
}

Vista2DDrawingObject::Vista2DDrawingObject(
    float fXPos, float fYPos, unsigned char nRed, unsigned char nGreen, unsigned char nBlue) {
  m_fXPos          = fXPos;
  m_fYPos          = fYPos;
  m_nRed           = nRed;
  m_nGreen         = nGreen;
  m_nBlue          = nBlue;
  m_bIsInitialized = false;
  m_bEnabled       = true;
}

Vista2DDrawingObject::~Vista2DDrawingObject() {
  m_fXPos    = 0.0f;
  m_fYPos    = 0.0f;
  m_nRed     = 0x0;
  m_nGreen   = 0x0;
  m_nBlue    = 0x0;
  m_bEnabled = false;
}
// ============================================================================
// ============================================================================
bool Vista2DDrawingObject::SetPosition(float fXPos, float fYPos) {
  if (fXPos >= 0.0f && fXPos <= 1.0f && fYPos >= 0.0f && fYPos <= 1.0f) {
    m_fXPos = fXPos;
    m_fYPos = fYPos;
    return true;
  }
  return false;
}
// ============================================================================
// ============================================================================
bool Vista2DDrawingObject::GetPosition(float& fXPos, float& fYPos) const {
  fXPos = m_fXPos;
  fYPos = m_fYPos;
  return true;
}
// ============================================================================
// ============================================================================
bool Vista2DDrawingObject::SetColor(unsigned char nRed, unsigned char nGreen, unsigned char nBlue) {
  m_nRed   = nRed;
  m_nGreen = nGreen;
  m_nBlue  = nBlue;
  return true;
}
// ============================================================================
// ============================================================================
bool Vista2DDrawingObject::GetColor(
    unsigned char& nRed, unsigned char& nGreen, unsigned char& nBlue) {
  nRed   = m_nRed;
  nGreen = m_nGreen;
  nBlue  = m_nBlue;
  return true;
}
// ============================================================================
// ============================================================================
Vista2DDrawingObject::Vista2DObjectType Vista2DDrawingObject::GetType() {
  return m_Type;
}
// ============================================================================
// ============================================================================
bool Vista2DDrawingObject::IsInitialized() {
  return m_bIsInitialized;
}

bool Vista2DDrawingObject::GetEnabled() const {
  return m_bEnabled;
}

void Vista2DDrawingObject::SetEnabled(bool bEnabled) {
  m_bEnabled = bEnabled;
}

Vista2DCompound::Vista2DCompound()
    : Vista2DDrawingObject() {
}

Vista2DCompound::Vista2DCompound(float fPosX, float fPosY)
    : Vista2DDrawingObject(fPosX, fPosY, 0x0, 0x0, 0x0) {
}

Vista2DCompound::~Vista2DCompound() {
  // std::list<Vista2DDrawingObject *>::const_iterator it;
  // for(it = m_liMembers.begin(); it != m_liMembers.end(); ++it)
  //    delete *it;
}

bool Vista2DCompound::SetPosition(float fPosX, float fPosY) {
  return false;
}

//*************************************************************************
//***                          Vista2DText ***
//*************************************************************************
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
Vista2DText::Vista2DText()
    : m_strText("")
    , m_iFontIndex(0)
    , m_b3DMode(false)
    , m_family(INVALID) {
  m_Type = VISTA2D_TEXT;
}

Vista2DText::~Vista2DText() {
}
// ============================================================================
// ============================================================================
bool Vista2DText::Init(const std::string& strText, float nPosX, float nPosY, unsigned char nRed,
    unsigned char nGreen, unsigned char nBlue, unsigned int size, Vista2DTextFontFamily family) {
  bool success = SetText(strText);
  success &= SetPosition(nPosX, nPosY);
  success &= SetColor(nRed, nGreen, nBlue);
  success &= SetSize(float(size));
  success &= SetFontFamily(family);
  m_bIsInitialized = success;
  m_b3DMode        = false;
  return success;
}
// ============================================================================
// ============================================================================
bool Vista2DText::SetText(const std::string& strNewText) {
  m_strText.assign(strNewText.c_str());
  return true;
}
// ============================================================================
// ============================================================================
std::string Vista2DText::GetText() const {
  return m_strText;
}

const std::string& Vista2DText::GetTextConstRef() const {
  return m_strText;
}

void Vista2DText::GetText(std::string& sTarget) const {
  sTarget = m_strText;
}

// ============================================================================
// ============================================================================

const char* Vista2DText::GetTextC() const {
  return m_strText.c_str();
}

int Vista2DText::GetFontIndex() const {
  return m_iFontIndex;
}

void Vista2DText::SetFontIndex(int iIndex) {
  m_iFontIndex = iIndex;
}

bool Vista2DText::SetPosition(float fPosX, float fPosY) {
  m_b3DMode = false;
  m_fXPos   = fPosX;
  m_fYPos   = fPosY;
  return true;
}

bool Vista2DText::GetPosition(float& fPosX, float& fPosY) const {
  fPosX = m_fXPos;
  fPosY = m_fYPos;
  return !m_b3DMode;
}

bool Vista2DText::SetPosition(float fPosX, float fPosY, float fPosZ) {
  m_b3DMode = true;
  m_fXPos   = fPosX;
  m_fYPos   = fPosY;
  m_fZPos   = fPosZ;
  return true;
}

bool Vista2DText::GetPosition(float& fPosX, float& fPosY, float& fPosZ) const {
  fPosX = m_fXPos;
  fPosY = m_fYPos;
  fPosZ = m_fZPos;
  return m_b3DMode;
}

bool Vista2DText::SetFontFamily(Vista2DTextFontFamily family) {
  m_family = family;
  return true;
}

bool Vista2DText::GetFontFamily(Vista2DTextFontFamily& family) const {
  family = m_family;
  return true;
}

bool Vista2DText::SetSize(float size) {
  m_uiSize = size;
  return true;
}

bool Vista2DText::GetSize(float& size) const {
  size = m_uiSize;
  return true;
}

bool Vista2DText::IsIn3DMode() {
  return m_b3DMode;
}

void Vista2DText::Set3DMode(bool b3DMode) {
  m_b3DMode = b3DMode;
}

//*************************************************************************
//***                          Vista2DBitmap							***
//*************************************************************************
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
Vista2DBitmap::Vista2DBitmap() {
  m_strBitmapName = "";
  m_pDispBridge   = NULL;
  m_pBitmapData   = NULL;
  m_bNeedsUpdate  = true;
  m_Type          = VISTA2D_BITMAP;
  m_nHeight       = 0;
  m_nWidth        = 0;
  m_bAlphaChannel = false;
}

Vista2DBitmap::Vista2DBitmap(IVistaDisplayBridge* pGrBridge) {
  m_strBitmapName = "";
  m_pDispBridge   = pGrBridge;
  m_pBitmapData   = NULL;
  m_bNeedsUpdate  = true;
  m_Type          = VISTA2D_BITMAP;
  m_nHeight       = 0;
  m_nWidth        = 0;
  m_bAlphaChannel = false;
  m_fWindowWidth  = 0.0;
  m_fWindowHeight = 0.0;
}

Vista2DBitmap::~Vista2DBitmap() {
  delete m_pBitmapData;
}
// ============================================================================
// ============================================================================
bool Vista2DBitmap::Init(const std::string& strFName, float nPosX, float nPosY) {
  bool success = SetBitmap(strFName);
  success &= SetPosition(nPosX, nPosY);
  m_bIsInitialized = success;
  return success;
}
// ============================================================================
// ============================================================================
bool Vista2DBitmap::SetBitmap(const std::string& strNewFName) {
  if (m_pDispBridge->DoLoadBitmap(
          strNewFName, &m_pBitmapData, m_nWidth, m_nHeight, m_bAlphaChannel)) {
    m_strBitmapName  = strNewFName;
    m_bNeedsUpdate   = true;
    m_bIsInitialized = true;
    return true;
  }
  m_bIsInitialized = false;
  return false;
}
// ============================================================================
// ============================================================================
std::string Vista2DBitmap::GetBitmapName() const {
  return m_strBitmapName;
}

const char* Vista2DBitmap::GetBitmapNameC() const {
  return m_strBitmapName.c_str();
}

// ============================================================================
// ============================================================================
bool Vista2DBitmap::GetDimensions(int& nWidth, int& nHeight) {
  nWidth  = m_nWidth;
  nHeight = m_nHeight;
  return true;
}
// ============================================================================
// ============================================================================
bool Vista2DBitmap::GetWindowDimensions(float& fWidth, float& fHeight) {
  fWidth  = m_fWindowWidth;
  fHeight = m_fWindowHeight;
  return true;
}
// ============================================================================
// ============================================================================
bool Vista2DBitmap::SetWindowDimensions(float fWidth, float fHeight) {
  m_fWindowWidth  = fWidth;
  m_fWindowHeight = fHeight;
  return true;
}
// ============================================================================
// ============================================================================
bool Vista2DBitmap::NeedsUpdate() {
  return m_bNeedsUpdate;
}
// ============================================================================
// ============================================================================
void Vista2DBitmap::Update() {
  m_bNeedsUpdate = false;
}
// ============================================================================
// ============================================================================
bool Vista2DBitmap::SetPosition(float fXPos, float fYPos) {
  m_bNeedsUpdate = Vista2DDrawingObject::SetPosition(fXPos, fYPos);
  return m_bNeedsUpdate;
}
// ============================================================================
// ============================================================================
bool Vista2DBitmap::SetColor(unsigned char nRed, unsigned char nGreen, unsigned char nBlue) {
  m_bNeedsUpdate = Vista2DDrawingObject::SetColor(nRed, nGreen, nBlue);
  return m_bNeedsUpdate;
}

//*************************************************************************
//***                          Vista2DRectangle						***
//*************************************************************************
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

Vista2DRectangle::Vista2DRectangle() {
  m_bNeedsUpdate = true;
  m_Type         = VISTA2D_RECTANGLE;
  m_fHeight      = 0.0;
  m_fWidth       = 0.0;
}

Vista2DRectangle::~Vista2DRectangle() {
}

// ============================================================================
// ============================================================================
bool Vista2DRectangle::Init(float fPosX, float fPosY, float fWidth, float fHeight,
    unsigned char nRed, unsigned char nGreen, unsigned char nBlue) {
  bool success = SetPosition(fPosX, fPosY);
  success &= SetDimensions(fWidth, fHeight);
  success &= SetColor(nRed, nGreen, nBlue);
  m_bIsInitialized = success;
  return success;
}

// ============================================================================
// ============================================================================
bool Vista2DRectangle::SetDimensions(float fWidth, float fHeight) {
  m_fWidth  = fWidth;
  m_fHeight = fHeight;
  return true;
}
// ============================================================================
// ============================================================================
bool Vista2DRectangle::GetDimensions(float& fWidth, float& fHeight) {
  fWidth  = m_fWidth;
  fHeight = m_fHeight;
  return true;
}
// ============================================================================
// ============================================================================
bool Vista2DRectangle::NeedsUpdate() {
  return m_bNeedsUpdate;
}
// ============================================================================
// ============================================================================
void Vista2DRectangle::Update() {
  m_bNeedsUpdate = false;
}
// ============================================================================
// ============================================================================
bool Vista2DRectangle::SetPosition(float fXPos, float fYPos) {
  m_bNeedsUpdate = Vista2DDrawingObject::SetPosition(fXPos, fYPos);
  return m_bNeedsUpdate;
}
// ============================================================================
// ============================================================================
bool Vista2DRectangle::GetPosition(float& fXPos, float& fYPos) const {
  fXPos = m_fXPos;
  fYPos = m_fYPos;
  return true;
}
// ============================================================================
// ============================================================================
bool Vista2DRectangle::SetColor(unsigned char nRed, unsigned char nGreen, unsigned char nBlue) {
  m_bNeedsUpdate = Vista2DDrawingObject::SetColor(nRed, nGreen, nBlue);
  return m_bNeedsUpdate;
}

//*************************************************************************
//***                          Vista2DLine ***
//*************************************************************************
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
Vista2DLine::Vista2DLine() {
  m_fXPos2 = 0.0;
  m_fYPos2 = 0.0;
  m_Type   = VISTA2D_LINE;
}
/*
Vista2DLine::Vista2DLine(float fXPos1, float fYPos1, float fXPos2, float fYPos2, unsigned char nRed,
unsigned char nGreen, unsigned char nBlue) :  Vista2DDrawingObject(fXPos1, fYPos1, nRed, nGreen,
nBlue)
{
        m_fXPos2 = fXPos2;
        m_fYPos2 = fYPos2;
}
*/
Vista2DLine::~Vista2DLine() {
  m_fXPos2 = 0.0;
  m_fYPos2 = 0.0;
}
// ============================================================================
// ============================================================================
bool Vista2DLine::Init(float nPosX1, float nPosY1, float nPosX2, float nPosY2, unsigned char nRed,
    unsigned char nGreen, unsigned char nBlue) {
  bool success = SetStartPoint(nPosX1, nPosY1);
  success &= SetEndPoint(nPosX2, nPosY2);
  success &= SetColor(nRed, nGreen, nBlue);
  m_bIsInitialized = success;
  return success;
}
// ============================================================================
// ============================================================================
bool Vista2DLine::SetStartPoint(float fXPos, float fYPos) {
  if (fXPos >= 0.0f && fXPos <= 1.0f && fYPos >= 0.0f && fYPos <= 1.0f) {
    m_fXPos = fXPos;
    m_fYPos = fYPos;
    return true;
  }
  return false;
}
// ============================================================================
// ============================================================================
bool Vista2DLine::GetStartPoint(float& fXPos, float& fYPos) {
  fXPos = m_fXPos;
  fYPos = m_fYPos;
  return true;
}
// ============================================================================
// ============================================================================
bool Vista2DLine::SetEndPoint(float fXPos, float fYPos) {
  if (fXPos >= 0.0f && fXPos <= 1.0f && fYPos >= 0.0f && fYPos <= 1.0f) {
    m_fXPos2 = fXPos;
    m_fYPos2 = fYPos;
    return true;
  }
  return false;
}
// ============================================================================
// ============================================================================
bool Vista2DLine::GetEndPoint(float& fXPos, float& fYPos) {
  fXPos = m_fXPos2;
  fYPos = m_fYPos2;
  return true;
}
// ============================================================================
// ============================================================================
