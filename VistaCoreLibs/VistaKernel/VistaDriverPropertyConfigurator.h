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


#ifndef _VISTADRIVERPROPERTYCONFIGURATOR_H
#define _VISTADRIVERPROPERTYCONFIGURATOR_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaAspects/VistaReflectionable.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaDeviceDriver;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaDriverPropertyConfigurator
{
public:
	VistaDriverPropertyConfigurator();
	virtual ~VistaDriverPropertyConfigurator();

	class VISTAKERNELAPI IConfigurator : public IVistaReflectionable
	{
	public:
		virtual ~IConfigurator();
		virtual bool Configure( IVistaDeviceDriver* pDriver,
								const VistaPropertyList& oDriverSection,
								const VistaPropertyList& oConfig );

		REFL_INLINEIMP( VistaDriverPropertyConfigurator::IConfigurator, IVistaReflectionable );
	protected:
		IConfigurator();
		IVistaDeviceDriver *m_pDriver;
	private:
		static std::string SsReflectionType;
	};


	void RegisterConfigurator(const std::string &sTriggerKey,
							  IConfigurator *pConf,
							  const std::list<std::string> &liDepends = std::list<std::string>(),
							  int nPrio = 0);
	int GetPrioForTrigger(const std::string &sTrigger) const;
	std::list<std::string> GetDependsForTrigger(const std::string &sTrigger) const;

	IConfigurator *RetrieveConfigurator(const std::string &sTriggerKey) const;

private:
	struct _sHlp
	{
		_sHlp()
			: m_pConf(NULL),
			  m_nPrio(0)
		{}

		_sHlp( IConfigurator *pConf,
			   int nPrio,
			   const std::list<std::string> &liDepends )
			   : m_pConf(pConf),
			     m_nPrio(nPrio),
				 m_liDependsOn(liDepends)
		{
		}

		IConfigurator *m_pConf;
		int m_nPrio;
		std::list<std::string> m_liDependsOn;
	};
	//typedef std::pair<IConfigurator*, int> CONFIG;
	typedef _sHlp CONFIG;

	typedef std::map<std::string, CONFIG > CONFIGMAP;
	CONFIGMAP m_mpConfigurators;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERPROPERTYCONFIGURATOR_H


