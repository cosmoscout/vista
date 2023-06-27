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

#ifndef _VISTA_OPENSG_GLOVERLAYFOREGROUND_BASE_H_
#define _VISTA_OPENSG_GLOVERLAYFOREGROUND_BASE_H_

#ifdef __sgi
#pragma once
#endif

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/OpenSG/VistaOpenSGGLOverlayForegroundFields.h>

#if defined(WIN32)
// diable warnings from OpenSG includes
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

#if defined(WIN32)
#pragma warning(pop)
#endif

OSG_BEGIN_NAMESPACE

class VistaOpenSGGLOverlayForeground;

class VISTAKERNELAPI VistaOpenSGGLOverlayForegroundBase : public Foreground {
 private:
  typedef Foreground Inherited;

  /*==========================  PUBLIC  =================================*/
 public:
  typedef VistaOpenSGGLOverlayForegroundPtr Ptr;

  enum { GLOverlaysFieldId = Inherited::NextFieldId, NextFieldId = GLOverlaysFieldId + 1 };

  static const BitVector GLOverlaysFieldMask;
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

  MField<void*>* getMFGLOverlays(void);

  void*                getGLOverlays(const UInt32 index);
  MField<void*>&       getGLOverlays(void);
  const MField<void*>& getGLOverlays(void) const;

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

  static VistaOpenSGGLOverlayForegroundPtr create(void);
  static VistaOpenSGGLOverlayForegroundPtr createEmpty(void);

  /*---------------------------------------------------------------------*/
  /*                              Copy                                   */
  /*---------------------------------------------------------------------*/

  virtual FieldContainerPtr shallowCopy(void) const;

  /*=========================  PROTECTED  ===============================*/
 protected:
  /*---------------------------------------------------------------------*/
  /*                             Fields                                  */
  /*---------------------------------------------------------------------*/

  MField<void*> m_mfGLOverlays;

  /*---------------------------------------------------------------------*/
  /*                          Constructors                               */
  /*---------------------------------------------------------------------*/

  VistaOpenSGGLOverlayForegroundBase(void);
  VistaOpenSGGLOverlayForegroundBase(const VistaOpenSGGLOverlayForegroundBase& source);

  /*---------------------------------------------------------------------*/
  /*                          Destructors                                */
  /*---------------------------------------------------------------------*/

  virtual ~VistaOpenSGGLOverlayForegroundBase(void);

  /*---------------------------------------------------------------------*/
  /*                              Sync                                   */
  /*---------------------------------------------------------------------*/
  //#if !defined(OSG_FIXED_MFIELDSYNC)
  void executeSyncImpl(VistaOpenSGGLOverlayForegroundBase* pOther, const BitVector& whichField);

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
  void operator=(const VistaOpenSGGLOverlayForegroundBase& source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

typedef VistaOpenSGGLOverlayForegroundBase* VistaOpenSGGLOverlayForegroundBaseP;

typedef osgIF<VistaOpenSGGLOverlayForegroundBase::isNodeCore,
    CoredNodePtr<VistaOpenSGGLOverlayForeground>,
    FieldContainer::attempt_to_create_CoredNodePtr_on_non_NodeCore_FC>::_IRet
    VistaOpenSGGLOverlayForegroundNodePtr;

typedef RefPtr<VistaOpenSGGLOverlayForegroundPtr> VistaOpenSGGLOverlayForegroundRefPtr;

OSG_END_NAMESPACE

#endif
