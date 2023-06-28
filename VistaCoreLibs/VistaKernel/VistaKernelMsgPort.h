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

#ifndef _VISTAKERNELMSGPORT_H
#define _VISTAKERNELMSGPORT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <map>
#include <string>

#include <VistaAspects/VistaSerializable.h>
#include <VistaInterProcComm/Connections/VistaMsg.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaKernelMsgPort;
class VistaMsgSource;
class VistaMsgSink;
class DLVistaActiveDataProducer;
class VistaThread;
class VistaThreadPool;
class DLVistaRamQueuePipe;
class VistaKernelMsgPacket;
class VistaAcceptWork;
class VistaSystem;
class IDLVistaPipeComponent;

class VistaByteBufferDeSerializer;
class VistaProgressMessage;

// for progress-messages
class VistaConnectionIP;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaKernelMsgType {
 public:
  enum {
    VKM_KERNEL_MSG       = -1,
    VKM_SCRIPTED_MSG     = -2,
    VKM_PROPERTYLIST_MSG = -3,
    VKM_APPLICATION_MSG  = 0,
    VKM_MSGLAST
  };
};

class VISTAKERNELAPI VistaKernelMsgPacket : public IDLVistaDataPacket {
 public:
  VistaKernelMsgPacket(IDLVistaPipeComponent*);
  virtual ~VistaKernelMsgPacket();

  virtual int         GetPacketSize() const;
  virtual void        SetDataSize(int);
  virtual int         GetDataSize() const;
  virtual std::string GetClName() const;

  virtual int Serialize(IVistaSerializer&) const;
  virtual int DeSerialize(IVistaDeSerializer&);

  VistaMsg* GetPacketMessage() const;
  void      SetPacketMessage(VistaMsg*);

  std::string GetSignature() const;

  virtual IDLVistaDataPacket* CreateInstance(IDLVistaPipeComponent*) const;

  int                 GetPacketNumber() const;
  void                SetPacketNumber(int i);
  VistaKernelMsgPort* GetSourcePort() const;
  void                SetSourcePort(VistaKernelMsgPort*);

 protected:
 private:
  VistaMsg*           m_pMessage;
  int                 m_iPacketNumber;
  VistaKernelMsgPort* m_pSourcePort;
};

class VISTAKERNELAPI VistaKernelMsgPort {
 public:
  // VistaKernelMsgPort(VistaSystem &, const std::string &sIniFile,
  //                    const std::string &sIniSection,
  //                    const std::string &sApplicationName,
  //		IDLVistaPipeComponent *pFrontPipe = NULL);

  VistaKernelMsgPort(VistaSystem* pVistaSystem, const std::string& sHost, int iPort,
      const std::string& sApplicationName, bool bCreateIndicator = false,
      const std::string& sIndicatorHost = "", int iIndicatorPort = 0,
      IDLVistaPipeComponent* pFrontPipe = NULL);
  virtual ~VistaKernelMsgPort();

  int HasMessage();

  VistaMsg* GetNextMsg();
  void      AnswerQuery(VistaMsg*);

  bool PutQuery(VistaMsg*);

  void Disconnect();
  bool GetIsConnected() const;

  void SetRescheduleFlag(bool b);
  bool GetRescheduleFlag() const;

  bool DispatchKernelMsg(VistaMsg& rMsg);

  enum eProgressType { PRG_SYSTEM = 0, PRG_APP, PRG_DONE, PRG_LAST };

  /**
   * @param eType its a system message, or it is an application message
   * @param iProg the progress indicator (0<=iProg<=100 for app messages)
   * @param sProgMessage the optional message for this progress
   */
  bool WriteProgress(eProgressType eType, int iProg, const std::string& sProgMessage = "");

 protected:
 private:
  bool DispatchInteractionMsg(int iMsgType, VistaMsg* pMsg, VistaByteBufferDeSerializer& dsSer);
  bool DispatchSystemMsg(int iMsgType, VistaMsg* pMsg, VistaByteBufferDeSerializer& dsSer);
  bool DispatchDisplayMsg(int iMsgType, VistaMsg* pMsg, VistaByteBufferDeSerializer& dsSer);
  bool DispatchInteractionDeviceMsg(
      int iMsgType, VistaMsg* pMsg, VistaByteBufferDeSerializer& dsSer);

  typedef std::map<VistaMsg*, VistaKernelMsgPacket*> MSGMAP;

  MSGMAP                     m_mpMsgMap;
  VistaMsgSource*            m_pSource;
  VistaMsgSink*              m_pSink;
  DLVistaActiveDataProducer* m_pProducer;
  // VistaThread               *m_pAcceptThread;
  DLVistaRamQueuePipe*  m_pQueue;
  VistaThreadPool*      m_pPool;
  VistaAcceptWork*      m_pAcceptWork;
  VistaSystem*          m_pSystem;
  VistaProgressMessage* m_pMsg;

  IDLVistaPipeComponent* m_pUserFront;

  VistaConnectionIP* m_pProgressConnection;

  std::string m_sApplicationName;
  bool        m_bRescheduleFlag;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAKERNELMSGPORT_H
