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


#ifndef _VISTATHREADIMP_H
#define _VISTATHREADIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <string>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaPriority;
class VistaThread;
class IVistaThreadImpFactory;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI IVistaThreadImp
{
public:
	virtual ~IVistaThreadImp();

	virtual bool Run() = 0;
	virtual bool Suspend() = 0;
	virtual bool Resume() = 0;
	virtual bool Join() = 0;
	virtual bool Abort() = 0;
	virtual bool SetPriority( const VistaPriority & ) = 0;

	virtual void GetPriority   (VistaPriority &) const = 0;
	static IVistaThreadImp *CreateThreadImp(const VistaThread &);

	virtual void YieldThread() = 0;

	virtual void SetCancelAbility(const bool bOkToCancel) = 0;
	virtual bool CanBeCancelled() const = 0;

	/**
	 * Method that is to be performed BEFORE departed fork starts execution
	 */
	virtual void PreRun() = 0;

	/**
	 * Method that is to be performed AFTER forked work is done
	 */
	virtual void PostRun() = 0;

	virtual bool Equals(const IVistaThreadImp &oImp) const;

	/**
	 * nails the thread to CPU with id iProcessorNum.
	 * @return false when this could not be done.
	 */
	virtual bool SetProcessorAffinity(int iProcessorNum);


	/**
	 * returns the ID of the CPU on which the calling thread is running
	 * on the time of call.
	 * @return -1 when something goes wrong (i.e. not implemented in subclass)
	 */
	virtual int  GetCpu() const;

	virtual bool SetThreadName(const std::string &sName);
	virtual std::string GetThreadName() const;

	virtual long GetThreadIdentity() const = 0;

	/**
	 * returns the Id of the calling threat (not a specific VistaThread Instance)
	 * Note that this Id may vary from the Id format that an Instance's GetThreadIdentity()
	 * may return
	 * @todo: make sure GetCallingThreadIdentity and GetThreadIdentity return same Id type
	 */
	static long GetCallingThreadIdentity( bool bBypassFactory = false );

	static bool SetCallingThreadPriority( const VistaPriority& oPrio, bool bBypassFactory = false );
	static bool GetCallingThreadPriority( VistaPriority& oPrio, bool bBypassFactory = false );

	/**
	 * class for the creation of custom thread imps
	 * subclass and register
	 */
	class VISTAINTERPROCCOMMAPI IVistaThreadImpFactory
	{
	public:
		virtual ~IVistaThreadImpFactory();
		virtual IVistaThreadImp* CreateThread(const VistaThread &) = 0;

		virtual long GetCallingThreadIdentity() const = 0;
		virtual bool SetCallingThreadPriority( const VistaPriority& oPrio ) const = 0;
		virtual bool GetCallingThreadPriority( VistaPriority& oPrio ) const = 0;
	protected:
		IVistaThreadImpFactory();
	};

	static void RegisterThreadImpFactory(IVistaThreadImpFactory *);
	static IVistaThreadImpFactory *GetThreadImpFactory();
	static void DeleteThreadImpFactory();


protected:
	IVistaThreadImp();
	void DoSetThreadName(const std::string &sName);
private:
	std::string m_sThreadName; //<** we cache the thread name */
	static IVistaThreadImpFactory *m_pSImpFactory;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

