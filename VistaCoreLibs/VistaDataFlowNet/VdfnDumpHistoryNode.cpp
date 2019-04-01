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


#include "VdfnDumpHistoryNode.h"
#include <VistaBase/VistaExceptionBase.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnDumpHistoryNode::VdfnDumpHistoryNode(const std::string &strFileName)
: IVdfnNode()
,  m_pHistory(NULL)
,  m_ofstream()
{
	m_ofstream.open(strFileName.c_str(), std::ios::out);

	RegisterInPortPrototype( "history", new HistoryPortCompare );
}

bool VdfnDumpHistoryNode::PrepareEvaluationRun()
{
	m_pHistory = dynamic_cast<HistoryPort*>( GetInPort( "history") );
	return GetIsValid();
}

bool VdfnDumpHistoryNode::DoEvalNode()
{
	VdfnHistoryPortData       *pData  = m_pHistory->GetValue();
	const VistaMeasureHistory &pHistory = pData->m_oHistory;

	unsigned int nUpd = m_pHistory->GetUpdateCounter();
	VistaType::uint64 nDc = pData->m_pSensor->GetMeasureCount();

	m_ofstream << std::fixed << "##############################################"
			   << std::endl
			   << "upd\t\t: " << nUpd
			   << std::endl
			   << "dc\t\t: "  << nDc
			   << std::endl;

	m_ofstream << "new measures\t: " << pData->m_nNewMeasures << std::endl
			   << "unskimmed\t: " << pData->m_nRealNewMeasures << std::endl
			   << "measure count\t: " << pData->m_nMeasureCount << std::endl
			   //<< "upd ts\t: " << pHistory.m_nUpdateTs << std::endl
//			   << "snap wh\t\t: " << pHistory.m_nSnapshotWriteHead << std::endl
			   << "history size\t: " << pHistory.m_rbHistory.GetBufferSize() << std::endl
//			   << "sensor upd\t: " << pData->m_pSensor->GetUpdateTimeStamp() << std::endl
			   << "client rs\t: " << pHistory.m_nClientReadSize << std::endl
			   << "driver ws\t:" << pHistory.m_nDriverWriteSize << std::endl;

	m_ofstream <<  "##############################################"
			   << std::endl
			   << "buffer"
			   << std::endl;

	m_ofstream << "idx\tmIdx\t\tswap\t\tmTs\t\tdTs" << std::endl;
	for( unsigned int bfIdx = 0; bfIdx < pHistory.m_rbHistory.GetBufferSize(); ++bfIdx )
	{
		m_ofstream << bfIdx
				   << "\t"
				   << (*pHistory.m_rbHistory.index(bfIdx)).m_nMeasureIdx
				   << "\t\t"
				   << (*pHistory.m_rbHistory.index(bfIdx)).m_nSwapTime
				   << "\t\t"
				   << (*pHistory.m_rbHistory.index(bfIdx)).m_nMeasureTs
				   << std::endl;

//			m_ofstream << std::endl << "buffer" << std::endl;
//			m_ofstream.setf(std::ios::hex);
//			for( int nIdx = 0; nIdx < (*pHistory.m_rbHistory.index(bfIdx)).m_vecMeasures.size(); ++nIdx )
//			{
//
//				m_ofstream << (char)(*pHistory.m_rbHistory.index(bfIdx)).m_vecMeasures[nIdx];
//				if( (nIdx % 16 == 0) )
//					m_ofstream << std::endl;
//				else
//					m_ofstream << " ";
//			}
//			m_ofstream.setf(std::ios::dec);
//			m_ofstream << std::endl;
	}
	m_ofstream << std::endl;
	return true;
}

// #############################################################################

VdfnDumpHistoryDefaultCreate::VdfnDumpHistoryDefaultCreate()
: VdfnNodeFactory::IVdfnNodeCreator()
{

}

IVdfnNode *VdfnDumpHistoryDefaultCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strFileName;
		if( subs.GetValue( "file", strFileName ) == false || strFileName.empty() )
			return NULL;

		return new VdfnDumpHistoryNode(strFileName);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

