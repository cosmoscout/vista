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

#ifndef _VISTAOPENSGTHREADIMP_H
#define _VISTAOPENSGTHREADIMP_H

#if defined(WIN32)
#endif

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/Concurrency/Imp/VistaThreadImp.h>
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaThread;

namespace osg {
class Thread;
};

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Implements a thread using the OSG thresading system, creating threads
 * during run() with OSG::ThreadManager::the()->getThread(<name>).
 * This thread imp can be passed to a VistaThread during construction.
 * Two ways to use this thing.
 * - forget about the thread imp after construction.<br>
         class MyThread : public VistaThread
         {
          public:
           MyThread() : VistaThread(new VistaOSGThreadImp("unique-name", this, 0))
           {
           }
         }<br>
         This results in a warning during compile time, but works as long as the name
         given is really unique.
 * - create the tread imp first, pass it to the thread during construction,
         and then associated the OSG Thread with the ViSTA thread.
         VistaOSGThreadImp *pImp = new VistaOSGThreadImp("unique-name", NULL, 0);

         VistaThread *pT = new VistaThread(pImp);
         pImp->SetVistaThread(pT);

         pT->Run()

 */
class VISTAKERNELAPI VistaOSGThreadImp : public IVistaThreadImp {
 public:
  /**
   * @param sName the name is important for the OSG::ThreadManager, so it must
                                  not be empty
   * @param pThread can be NULL during construction, see SetVistaThread()
   * @param nAspect defines the OSG aspect that this thread is working on,
                    0 for the VistaKernel Aspect, 1 for threaded aspects. Use 0 to
                    get standard behavior as without using the OSG MT feature.
                    Use 1 when you know what you are doing.
   */
  VistaOSGThreadImp(const std::string& sName, VistaThread* pThread, int nAspect = 0);
  virtual ~VistaOSGThreadImp();

  virtual bool Run();
  virtual bool Suspend();
  virtual bool Resume();
  virtual bool Join();
  virtual bool Abort();

  virtual bool SetPriority(const VistaPriority&);

  virtual void GetPriority(VistaPriority&) const;

  virtual void YieldThread();

  virtual void SetCancelAbility(const bool bOkToCancel);
  virtual bool CanBeCancelled() const;

  /**
   * Method that is to be performed BEFORE departed fork starts execution
   */
  virtual void PreRun();

  /**
   * Method that is to be performed AFTER forked work is done
   */
  virtual void PostRun();

  virtual long GetThreadIdentity() const;

  VistaThread* GetVistaThread() const;

  void SetVistaThread(VistaThread* pThread);

  osg::Thread* GetOSGThread() const;

  bool ApplyAndClearChangeList();

  class VISTAKERNELAPI VistaOSGThreadImpFactory : public IVistaThreadImp::IVistaThreadImpFactory {
   public:
    VistaOSGThreadImpFactory();
    virtual ~VistaOSGThreadImpFactory();

    virtual IVistaThreadImp* CreateThread(const VistaThread&);
    virtual long             GetCallingThreadIdentity() const;
    virtual bool             SetCallingThreadPriority(const VistaPriority& oPrio) const;
    virtual bool             GetCallingThreadPriority(VistaPriority& oPrio) const;

   private:
    int m_nCount;
  };

 protected:
 private:
  struct _hlp {
    _hlp() {
      m_pVistaThread = NULL;
      m_pOsgThread   = NULL;
    }

    VistaThread*       m_pVistaThread;
    VistaOSGThreadImp* m_pOsgThread;
  } m_hlp;
  static void threadFct(void* vp);

 private:
  osg::Thread* m_pOSGThread;
  bool         m_bRunning;
  int          m_nAspect;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGTHREADIMP_H
