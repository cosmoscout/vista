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


#include "VdfnPort.h" 

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

IVdfnPort::IVdfnPort()
: m_nLastUpdate(0),
  m_nUpdateCounter(0),
  m_pAdapter(NULL)
{
}

IVdfnPort::~IVdfnPort()
{
	delete m_pAdapter;
}


IVdfnPortTypeCompare::IVdfnPortTypeCompare()
{
}

IVdfnPortTypeCompare::IVdfnPortTypeCompare(const IVdfnPortTypeCompare &)
{
}

IVdfnPortTypeCompare& IVdfnPortTypeCompare::operator=(const IVdfnPortTypeCompare &)
{
	return *this;
}

double IVdfnPort::GetLastUpdate() const
{
	return m_nLastUpdate;
}

void IVdfnPort::SetUpdateTimeStamp(double nTs)
{
	m_nLastUpdate = nTs;
}

double IVdfnPort::GetUpdateTimeStamp() const
{
    return m_nLastUpdate;
}

void IVdfnPort::IncUpdateCounter()
{
	++m_nUpdateCounter;
}

unsigned int IVdfnPort::GetUpdateCounter() const
{
	return m_nUpdateCounter;
}

VdfnPortSerializeAdapter *IVdfnPort::GetSerializeAdapter()
{
	return m_pAdapter;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VdfnPortSerializeAdapter::VdfnPortSerializeAdapter(IVdfnPort *pPort)
: m_pPort(pPort)
{
}

VdfnPortSerializeAdapter::~VdfnPortSerializeAdapter()
{
}

int VdfnPortSerializeAdapter::Serialize(IVistaSerializer &oSer) const
{
	if(m_pPort == NULL)
		return 0;

	int nRet = 0;
	nRet += oSer.WriteDouble( (*m_pPort).m_nLastUpdate );
	nRet += oSer.WriteInt32( (VistaType::sint32)(*m_pPort).m_nUpdateCounter );
	return nRet;
}

int VdfnPortSerializeAdapter::DeSerialize(IVistaDeSerializer &oDeSer)
{
	if(m_pPort == NULL)
		return 0;

	int nRet = 0;

	VistaType::sint32 updCnt=0;
	nRet += oDeSer.ReadDouble( (*m_pPort).m_nLastUpdate );
	nRet += oDeSer.ReadInt32( updCnt );

	(*m_pPort).m_nUpdateCounter = updCnt;

	return nRet;
}

std::string VdfnPortSerializeAdapter::GetSignature() const
{
	return "IVdfnPort";
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


