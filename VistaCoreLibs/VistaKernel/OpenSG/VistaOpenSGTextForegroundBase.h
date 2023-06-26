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

#ifndef _VISTA_OPENSG_TEXTFOREGROUND_BASE_H_
#define _VISTA_OPENSG_TEXTFOREGROUND_BASE_H_

#ifdef __sgi
#pragma once
#endif

#include <VistaKernel/OpenSG/VistaOpenSGTextForegroundFields.h>
#include <VistaKernel/VistaKernelConfig.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSystemDef.h>

#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGCoredNodePtr.h>
#include <OpenSG/OSGRefPtr.h>

#include <OpenSG/OSGForeground.h>   // Parent
#include <OpenSG/OSGStringFields.h> // Formats type
#include <OpenSG/OSGTextureChunk.h>

#ifdef WIN32
#pragma warning(pop)
#endif

class Vista2DText;

OSG_BEGIN_NAMESPACE

class VistaOpenSGTextForeground;
class TextTXFFace;

class VISTAKERNELAPI VistaOpenSGTextForegroundBase : public Foreground {
 private:
  typedef Foreground Inherited;

  /*==========================  PUBLIC  =================================*/
 public:
  typedef VistaOpenSGTextForegroundPtr Ptr;

  enum { TextsFieldId = Inherited::NextFieldId, NextFieldId = TextsFieldId + 1 };

  static const BitVector TextsFieldMask;
  static const BitVector MTInfluenceMask;

  /*---------------------------------------------------------------------*/
  /*                           Class Get                                 */
  /*---------------------------------------------------------------------*/

  static FieldContainerType& getClassType(void);
  static UInt32              getClassTypeId(void);

  /*---------------------------------------------------------------------*/
  /*                       FieldContainer Get                            */
  /*---------------------------------------------------------------------*/

  virtual FieldContainerType&       getType(void);
  virtual const FieldContainerType& getType(void) const;

  virtual UInt32 getContainerSize(void) const;

  /*---------------------------------------------------------------------*/
  /*                           Field Get                                 */
  /*---------------------------------------------------------------------*/

  MField<void*>* getMFTexts(void);

  void*                getTexts(const UInt32 index);
  MField<void*>&       getTexts(void);
  const MField<void*>& getTexts(void) const;

  /*---------------------------------------------------------------------*/
  /*                           Field Set                                 */
  /*---------------------------------------------------------------------*/

  /*---------------------------------------------------------------------*/
  /*                              Sync                                   */
  /*---------------------------------------------------------------------*/

  /*---------------------------------------------------------------------*/
  /*                          Binary Access                              */
  /*---------------------------------------------------------------------*/

  virtual UInt32 getBinSize(const BitVector& whichField);
  virtual void   copyToBin(BinaryDataHandler& pMem, const BitVector& whichField);
  virtual void   copyFromBin(BinaryDataHandler& pMem, const BitVector& whichField);

  /*---------------------------------------------------------------------*/
  /*                          Construction                               */
  /*---------------------------------------------------------------------*/

  static VistaOpenSGTextForegroundPtr create(void);
  static VistaOpenSGTextForegroundPtr createEmpty(void);

  /*---------------------------------------------------------------------*/
  /*                              Copy                                   */
  /*---------------------------------------------------------------------*/

  virtual FieldContainerPtr shallowCopy(void) const;

  /*=========================  PROTECTED  ===============================*/
 protected:
  /*---------------------------------------------------------------------*/
  /*                             Fields                                  */
  /*---------------------------------------------------------------------*/

  MField<void*> m_mfTexts;

  class _COSGFaceHlp {
   public:
    _COSGFaceHlp(TextTXFFace* face, UInt32 nFaceType, UInt32 nSize, TextureChunkPtr texture);
    ~_COSGFaceHlp();

    TextTXFFace*    m_pFace;
    UInt32          m_nSize;
    UInt32          m_nFaceType;
    TextureChunkPtr m_pTexture;
  };

  MField<void*> m_vFaceLookup;

  /*---------------------------------------------------------------------*/
  /*                          Constructors                               */
  /*---------------------------------------------------------------------*/

  VistaOpenSGTextForegroundBase(void);
  VistaOpenSGTextForegroundBase(const VistaOpenSGTextForegroundBase& source);

  /*---------------------------------------------------------------------*/
  /*                          Destructors                                */
  /*---------------------------------------------------------------------*/

  virtual ~VistaOpenSGTextForegroundBase(void);

  /*---------------------------------------------------------------------*/
  /*                              Sync                                   */
  /*---------------------------------------------------------------------*/
  //#if !defined(OSG_FIXED_MFIELDSYNC)
  void executeSyncImpl(VistaOpenSGTextForegroundBase* pOther, const BitVector& whichField);

  virtual void executeSync(FieldContainer& other, const BitVector& whichField);
  //#else
  /*    void executeSyncImpl(      SimpleStatisticsForegroundBase *pOther,
                                                   const BitVector         &whichField,
                                                   const SyncInfo          &sInfo     );

          virtual void   executeSync(      FieldContainer    &other,
                                                             const BitVector         &whichField,
                                                             const SyncInfo          &sInfo);

          virtual void execBeginEdit     (const BitVector &whichField,
                                                                                    UInt32 uiAspect,
                                                                                    UInt32
     uiContainerSize);

                          void execBeginEditImpl (const BitVector &whichField,
                                                                                    UInt32 uiAspect,
                                                                                    UInt32
     uiContainerSize);

                                            virtual void onDestroyAspect(UInt32 uiId, UInt32
     uiAspect);*/
  //#endif
  /*==========================  PRIVATE  ================================*/
 private:
  friend class FieldContainer;

  static FieldDescription*  _desc[];
  static FieldContainerType _type;

  // prohibit default functions (move to 'public' if you need one)
  void operator=(const VistaOpenSGTextForegroundBase& source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

typedef VistaOpenSGTextForegroundBase* VistaOpenSGTextForegroundBaseP;

typedef osgIF<VistaOpenSGTextForegroundBase::isNodeCore, CoredNodePtr<VistaOpenSGTextForeground>,
    FieldContainer::attempt_to_create_CoredNodePtr_on_non_NodeCore_FC>::_IRet
    VistaOpenSGTextForegroundNodePtr;

typedef RefPtr<VistaOpenSGTextForegroundPtr> VistaOpenSGTextForegroundRefPtr;

OSG_END_NAMESPACE

#endif
