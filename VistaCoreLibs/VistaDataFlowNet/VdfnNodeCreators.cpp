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


#include "VdfnNodeCreators.h"
#include "VdfnPortFactory.h"
#include "VdfnPersistence.h"
#include "VdfnGraph.h"

#include <VistaBase/VistaVectorMath.h>
#include <VistaAspects/VistaConversion.h>


#include "VdfnShallowNode.h"
#include "VdfnTimerNode.h"
#include "VdfnUpdateThresholdNode.h"

#include "VdfnGetTransformNode.h"
#include "VdfnSetTransformNode.h"
#include "VdfnApplyTransformNode.h"

#include "VdfnLoggerNode.h"
#include "VdfnOutstreamNode.h"
#include "VdfnCompositeNode.h"
#include "VdfnToggleNode.h"

#include "VdfnNodeFactory.h"
#include "VdfnHistoryPort.h"
#include "VdfnActionObject.h"
#include "VdfnActionNode.h"
#include "VdfnSamplerNode.h"
#include "VdfnCompositeNode.h"
#include "VdfnBinaryOpNode.h"
#include "VdfnAxisRotateNode.h"
#include "VdfnProjectVectorNode.h"
#include "VdfnForceFeedbackNode.h"
#include "Vdfn3DNormalizeNode.h"
#include "VdfnReadWorkspaceNode.h"
#include "VdfnInvertNode.h"
#include "VdfnNegateNode.h"
#include "VdfnValueToTriggerNode.h"
#include "VdfnConditionalRouteNode.h"
#include "VdfnDumpHistoryNode.h"
#include "VdfnDelayNode.h"
#include "VdfnMatrixComposeNode.h"
#include "VdfnLatestUpdateNode.h"
#include "VdfnCompose3DVectorNode.h"
#include "VdfnDecompose3DVectorNode.h"
#include "VdfnComposeQuaternionFromDirectionsNode.h"
#include "VdfnComposeQuaternionNode.h"
#include "VdfnDecomposeQuaternionNode.h"
#include "VdfnComposeTransformMatrixNode.h"
#include "VdfnDecomposeTransformMatrixNode.h"
#include "VdfnQuaternionSlerpNode.h"
#include "VdfnMultiplexNode.h"
#include "VdfnDemultiplexNode.h"
#include "VdfnAbsoluteNode.h"
#include "VdfnConstantValueNode.h"
#include "VdfnDifferenceNode.h"
#include "VdfnTrackingRedundancyNode.h"

#include "VdfnTypeConvertNodeCreator.h"


#include "VdfnHistoryProjectNode.h"
#include "VdfnDriverSensorNode.h"
#include "VdfnObjectRegistry.h"

#include "VdfnConstantValueNode.h"

#include <VistaDeviceDriversBase/VistaDriverMap.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>

#include <VistaTools/VistaEnvironment.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
IVdfnNode *VdfnShallowNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	VdfnShallowNode *pShallowNode = new VdfnShallowNode;
	try
	{
		VdfnPortFactory *pFac = VdfnPortFactory::GetSingleton();
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		if(subs.HasProperty("inports"))
		{
			const VistaPropertyList &oInports = subs.GetPropertyConstRef("inports").GetPropertyListConstRef();

			for(VistaPropertyList::const_iterator cit = oInports.begin();
				cit != oInports.end(); ++cit)
			{
				std::string strName = (*cit).first;
				std::string strValue = (*cit).second.GetValue();

				VdfnPortFactory::CPortAccess *pAccess = pFac->GetPortAccess( strValue );
				if(pAccess)
				{
					IVdfnPort *pPort = pAccess->m_pCreationMethod->CreatePort();
					pShallowNode->SetInPort( strName, pPort );
				}
			}
		}

		if(subs.HasProperty("outports"))
		{
			const VistaPropertyList &oOutports = subs.GetPropertyConstRef("outports").GetPropertyListConstRef();

			for(VistaPropertyList::const_iterator cit = oOutports.begin();
				cit != oOutports.end(); ++cit)
			{
				std::string strName = (*cit).first;
				std::string strValue = (*cit).second.GetValue();

				VdfnPortFactory::CPortAccess *pAccess = pFac->GetPortAccess( strValue );
				if(pAccess)
				{
					IVdfnPort *pPort = pAccess->m_pCreationMethod->CreatePort();
					pShallowNode->SetOutPort( strName, pPort );
				}
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return pShallowNode;
}


// #############################################################################

IVdfnNode *VdfnTimerNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList &oSubs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

	bool reset = false;
	oSubs.GetValue( "reset_on_activate", reset );

	return new VdfnTimerNode( new CTimerGetTime, reset );
}

// #############################################################################

IVdfnNode *VdfnTickTimerNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	return new VdfnTickTimerNode( new CTimerGetTime );
}

// #############################################################################

IVdfnNode *VdfnUpdateThresholdNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

	double dThreshold = 0.0;
	subs.GetValue( "threshold", dThreshold );

	return new VdfnUpdateThresholdNode( dThreshold );
}


// #############################################################################

VdfnGetTransformNodeCreate::VdfnGetTransformNodeCreate(VdfnObjectRegistry *pReg)
: m_pReg(pReg)
{
}

IVdfnNode *VdfnGetTransformNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strObj;
		if( subs.GetValue("object", strObj) == false )
		{
			vstr::warnp() << "VdfnGetTransformNodeCreate::CreateNode() -- "
				<< "Required parameter [object] has not been specified!" << std::endl;
		}
		IVistaTransformable *pTrans = m_pReg->GetObjectTransform( strObj );

		std::string strMode = subs.GetValueOrDefault<std::string>( "mode", "RELATIVE" );

        VdfnGetTransformNode *pNode;
		if(pTrans)
        {
			pNode = new VdfnGetTransformNode(pTrans);
        }
		else
			pNode = new VdfnGetTransformNode( m_pReg, strObj );

        if(VistaAspectsComparisonStuff::StringEquals(strMode, "WORLD", false))
            pNode->SetTransformGetMode( VdfnGetTransformNode::TRANS_WORLD );
        else
            pNode->SetTransformGetMode( VdfnGetTransformNode::TRANS_RELATIVE );

        return pNode;
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

VdfnSetTransformNodeCreate::VdfnSetTransformNodeCreate(VdfnObjectRegistry *pReg)
: m_pReg(pReg)
{
}

IVdfnNode *VdfnSetTransformNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strObj;
		if( subs.GetValue( "object", strObj ) == false )
		{
			vstr::warnp() << "VdfnSetTransformNodeCreate::CreateNode() -- "
				<< "Required parameter [object] has not been specified!" << std::endl;
		}
		IVistaTransformable *pTrans = m_pReg->GetObjectTransform( strObj );
		if(pTrans)
			return new VdfnSetTransformNode(pTrans);
		else
			return new VdfnSetTransformNode(m_pReg, strObj);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

VdfnApplyTransformNodeCreate::VdfnApplyTransformNodeCreate(VdfnObjectRegistry *pReg)
: m_pReg(pReg)
{
}

IVdfnNode *VdfnApplyTransformNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strObj;
		if( subs.GetValue( "object", strObj ) == false )
		{
			vstr::warnp() << "VdfnApplyTransformNodeCreate::CreateNode() -- "
				<< "Required parameter [object] has not been specified!" << std::endl;
		}
		bool applyLocal = subs.GetValueOrDefault<bool>( "local", false );
		IVistaTransformable *pTrans = m_pReg->GetObjectTransform( strObj );
		if(pTrans)
			return new VdfnApplyTransformNode(pTrans, applyLocal);
		else
			return new VdfnApplyTransformNode(m_pReg, strObj, applyLocal);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}


// #############################################################################


IVdfnNode *VdfnHistoryProjectNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::list<std::string> liProjects;
		subs.GetValue<std::list<std::string> >("project", liProjects );

		std::string sMode("LAZY");
		if( subs.HasProperty("mode") )
			sMode = subs.GetValue<std::string>("mode");

		VdfnHistoryProjectNode::eMode eMode = VdfnHistoryProjectNode::MD_LAZY;
		if( sMode == "HOT" )
			eMode = VdfnHistoryProjectNode::MD_HOT;
		else if( sMode == "ITERATE" )
			eMode = VdfnHistoryProjectNode::MD_ITERATE;
		else if( sMode == "INDEXED" )
			eMode = VdfnHistoryProjectNode::MD_INDEXED;

		if( liProjects.empty() )
			liProjects.push_back( std::string("*") );

		return new VdfnHistoryProjectNode(liProjects, eMode);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}





// #############################################################################

//IVdfnNode *VdfnDriverSensorNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
//{
//	try
//	{
//		VistaDeviceSensor* pSensor = NULL;
//		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
//
//		std::string sDriver;
//		if( subs.GetValue( "driver", sDriver ) == false )
//		{
//			vstr::warnp() << "[SensorNodeCreate]: - "
//					<< "No parameter [driver] has been specified" << std::endl;
//			return NULL;
//		}
//		int nSensorID = -1;
//		std::string sSensorName;
//		
//		const bool bHasSensorIndex = subs.GetValue( "sensor_index", nSensorID );
//		const bool bHasSensorName = subs.GetValue( "sensor_name", sSensorName );
//
//		IVistaDeviceDriver *pDriver = m_pMap->GetDeviceDriver( sDriver );
//		if( pDriver == NULL )
//		{
//			vstr::warnp() << "[DriverSensorNodeCreate]: Driver [" << sDriver << "] not found in map\n"
//						<< vstr::indent << " Available Drivers:" << std::endl;
//			vstr::IndentObject oIndent;
//			for(VistaDriverMap::const_iterator cit = m_pMap->begin();
//				cit != m_pMap->end(); ++cit)
//			{
//				vstr::warni()  << (*cit).first << std::endl;
//			}
//			return NULL;
//		}
//
//		// check for a sensor mapping aspect		
//		VistaDriverSensorMappingAspect *pMappingAspect =
//				dynamic_cast<VistaDriverSensorMappingAspect*>(pDriver->GetAspectById(
//				VistaDriverSensorMappingAspect::GetAspectId() ) );
//
//		if( pMappingAspect )
//		{
//			std::string sSensorType = subs.GetValueOrDefault<std::string>( "type", "" );
//			unsigned int nType = pMappingAspect->GetTypeId( sSensorType );
//			if( nType == VistaDriverSensorMappingAspect::INVALID_TYPE )
//			{
//				vstr::warnp() << "[SensorNodeCreate]: - "
//						<< "Driver [" << sDriver << "] has a sensor mapping aspect, "
//						<< "but no sensor type with name [" << sSensorType << "].\n"
//						<< vstr::indent << "Type names are: " << std::endl;
//				std::list<std::string> liTypes = pMappingAspect->GetTypeNames();
//				vstr::IndentObject oIndent;
//				for(std::list<std::string>::const_iterator it = liTypes.begin();
//					it != liTypes.end(); ++it )
//				{
//					vstr::warni() << "[" << *it << "]" << std::endl;
//				}
//				return NULL;
//			}
//
//			// drivers with sensor mapping always need specific sensor targets
//			// Exception: There is exactly one type and one sensor
//			if( !bHasSensorIndex && !bHasSensorName )
//			{
//				if( pMappingAspect->GetNumberOfRegisteredTypes() != 1
//					&& pMappingAspect->GetNumRegisteredSensorsForType( nType ) > 1 )
//				{
//					vstr::warnp() << "[SensorNodeCreate]: - Driver ["
//							<< sDriver << "] requires a sensor to be specified!"
//							<< " Use \"sensor_name\" or \"sensor_index\"" << std::endl;
//					return NULL;
//				}
//				unsigned int nId = pMappingAspect->GetSensorId( nType, 0 );
//				pSensor = pDriver->GetSensorByIndex( nId );
//			}			
//			else if( bHasSensorIndex )
//			{
//				unsigned int nId = pMappingAspect->GetSensorId( nType, nSensorID );
//				pSensor = pDriver->GetSensorByIndex( nId );
//			}
//			else if( bHasSensorName )
//			{
//				pSensor = pDriver->GetSensorByName( sSensorName );
//			}
//
//			if( pSensor == NULL )
//			{
//				if( bHasSensorName )
//				{
//					vstr::warnp() << "[SensorNodeCreate]: - "
//						<< "Driver [" << sDriver << "] has no sensor with name [" << sSensorName
//						<< "] for type [" << sSensorType << "].\n";
//				}
//				else
//				{
//					vstr::warnp() << "[SensorNodeCreate]: - "
//						<< "Driver [" << sDriver << "] has no sensor with Id [" << nSensorID
//						<< "] for type [" << sSensorType << "].\n";
//				}
//			}
//		}
//		else
//		{
//			// no SensorMappingAspect
//			if( bHasSensorIndex )
//			{
//				pSensor = pDriver->GetSensorByIndex( nSensorID );
//			}
//			else if( bHasSensorName )
//			{
//				pSensor = pDriver->GetSensorByName( sSensorName );
//			}
//			else if( pDriver->GetNumberOfSensors() == 1 )
//			{
//				// we only have one sensors, so we'll take this one
//				pSensor = pDriver->GetSensorByIndex( 0 );
//			}
//			else
//			{
//				vstr::warnp() << "[SensorNodeCreate]: - Driver ["
//						<< sDriver << "] requires a sensor to be specified! "
//						<< "Use \"sensor_name\" or \"sensor_index\"" << std::endl;
//				return NULL;
//			}
//
//			if( pSensor == NULL )
//			{
//				if( bHasSensorName )
//				{
//					vstr::warnp() << "[SensorNodeCreate]: - "
//						<< "Driver [" << sDriver << "] has no sensor with name [" << sSensorName << "].\n";
//				}
//				else
//				{
//					vstr::warnp() << "[SensorNodeCreate]: - "
//						<< "Driver [" << sDriver << "] has no sensor with Id [" << nSensorID << "].\n";					
//				}
//				vstr::warni() << "Available sensors are: \n";
//				vstr::IndentObject oIndent;
//				for( int i = 0; i < (int)pDriver->GetNumberOfSensors(); ++i  )
//				{
//					vstr::warni() << "Id: " << i 
//							<<  "\tName: " << pDriver->GetSensorByIndex(i)->GetSensorName() 
//							<< std::endl;
//				}
//				return NULL;
//			}
//		}
//
//		if( pSensor )
//		{
//			return new VdfnDriverSensorNode(pSensor);
//		}
//		else
//		{
//			vstr::warnp() << "[DriverSensorNodeCreate]: Could not create SensorNode\n";
//			vstr::IndentObject oIndent;
//			vstr::warni() << "DRIVER       : [" << sDriver << "]\n";
//			vstr::warni() << "SENSOR NAME  : [" << sSensorName << "]\n";
//			vstr::warni() << "SENSOR INDEX : [" << nSensorID << "]" << std::endl;
//		}
//
//	}
//	catch(VistaExceptionBase &x)
//	{
//		x.PrintException();
//	}
//
//	return NULL;
//}
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


IVdfnNode *VdfnLoggerNodeCreate::CreateNode( const VistaPropertyList &oConstParams ) const
{
	VistaPropertyList oParams = oConstParams;
	VistaPropertyList &subs = oParams.GetPropertyRef("param").GetPropertyListRef();

	if(!subs.HasProperty("prefix"))
		return NULL;

	bool bWriteHeader = subs.GetValueOrDefault<bool>( "writeheader", false );
	bool bWriteTime = subs.GetValueOrDefault<bool>( "writetime", false );
	bool bWriteDiff = subs.GetValueOrDefault<bool>( "writediff", false );
	bool bLogToConsole = subs.GetValueOrDefault<bool>( "logtoconsole", false );

	std::list<std::string> liPorts;
	subs.GetValue( "sortlist", liPorts );

	std::list<std::string> liTriggers;
	subs.GetValue( "triggerlist", liTriggers );

	// @todo: remove this in future releases
	// copying the oConstParams to oParams at the beginning of this
	// method can be removed as well at that point.
	if( subs.HasProperty( "separator" ) )
	{
#ifdef DEBUG
		vstr::err() << "WARNING: LoggerNode uses separator property, which is now called seperator!"
					<< std::endl;
		vstr::err() << "Please update your xml files accordingly."
					<< std::endl;
#endif
		subs.SetValue( "seperator", subs.GetValue<std::string>( "separator" ));
	}

	std::string strSep = subs.GetValueOrDefault<std::string>( "seperator", " " );

	if(!bWriteTime)
		bWriteDiff = false; // override user settings

	return new VdfnLoggerNode( subs.GetValueOrDefault<std::string>( "prefix", "" ),
		                         bWriteHeader,
								 bWriteTime,
								 bWriteDiff,
								 bLogToConsole,
								 liPorts,
								 liTriggers, strSep );
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IVdfnNode *VdfnCompositeNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		if(!subs.HasProperty("graph"))
			return NULL;

		std::string strGraphFileName;
		if( subs.GetValue( "graph", strGraphFileName ) == false )
		{
			vstr::warnp() << "VdfnCompositeNodeCreate() -- no value for [graph] has been given!"
					<< std::endl;
			return NULL;
		}
		std::string strNodeName;
		oParams.GetValue( "name", strNodeName );
		std::string strParameterFile;
		oParams.GetValue( "parameterfile", strNodeName );


		VdfnGraph *pGraph = VdfnPersistence::LoadGraph( strGraphFileName, strNodeName, false, true, strParameterFile );
		if(!pGraph)
		{
			vstr::warnp() << "[VdfnCompositeNodeCreate]: -- file: ["
						<< strGraphFileName
						<< "] could not be loaded"
						 << std::endl;
			return NULL;
		}

		if( oParams.HasProperty("dumpparams")
			&& oParams.GetPropertyConstRef("dumpparams").GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST )
		{
			const VistaPropertyList &dumpParams = oParams.GetPropertyConstRef("dumpparams").GetPropertyListConstRef();
			bool bDump = dumpParams.GetValueOrDefault( "dump", false );

			if(bDump)
			{
				std::string strGraphDotFile;
				dumpParams.GetValue( "dotfilename", strGraphDotFile );
				bool bWritePorts = dumpParams.GetValueOrDefault<bool>( "writeports", false );
				bool bWriteTypes = dumpParams.GetValueOrDefault<bool>( "writetypes", false );

				VdfnPersistence::SaveAsDot( pGraph, strGraphDotFile, strNodeName, bWritePorts, bWriteTypes );
			}
		}

		return new VdfnCompositeNode( pGraph );
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}

// #############################################################################



IVdfnNode *VdfnEnvStringValueNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList &prams = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

	std::string strEnvVarName;
	std::string strEnvDefault;
	prams.GetValue( "varname", strEnvVarName );
	prams.GetValue( "default", strEnvDefault );

	bool bIsStrict = prams.GetValueOrDefault<bool>( "strict", false );

	std::string strEnvVarValue = VistaEnvironment::GetEnv(strEnvVarName);

	// is it strict? then we do not accept empty vars
	if(bIsStrict && strEnvVarValue.empty())
		return NULL;

	// it is not strict, but empty... did the user give a default value?
	if( strEnvVarValue.empty() && !strEnvDefault.empty() )
		strEnvVarValue = strEnvDefault;
	// else: no, probably an empty string is ok, too

	TVdfnConstantValueNode<std::string> *pEnvNode = new TVdfnConstantValueNode<std::string>;
	pEnvNode->SetValue(strEnvVarValue);
	return pEnvNode;
}

// #############################################################################

IVdfnNode* VdfnToggleNodeCreate::CreateNode( const VistaPropertyList& oParams ) const
{
	const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );
	bool bInitState = oSubs.GetValueOrDefault<bool>( "initial_state", false );
	std::string sMode;
	VdfnToggleNode::ToggleMode eMode = VdfnToggleNode::TM_ON_CHANGE_TO_TRUE;
	if( oSubs.GetValue( "mode", sMode ) )
	{
		if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sMode, "on_any_update" ) )
			eMode = VdfnToggleNode::TM_ON_ANY_UPDATE;
		else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sMode, "on_update_if_false" )  )
			eMode = VdfnToggleNode::TM_ON_UPDATE_IF_FALSE;
		else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sMode, "on_update_if_true" )  )
			eMode = VdfnToggleNode::TM_ON_UPDATE_IF_TRUE;
		else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sMode, "on_any_change" )  )
			eMode = VdfnToggleNode::TM_ON_ANY_CHANGE;
		else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sMode, "on_change_to_true" )  )
			eMode = VdfnToggleNode::TM_ON_CHANGE_TO_TRUE;
		else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sMode, "on_change_to_false" )  )
			eMode = VdfnToggleNode::TM_ON_CHANGE_TO_FALSE;
		else
		{
			vstr::warnp() << "[VdfnToggleNodeCreate]: Unknown mode ["
				<< sMode << std::endl;
			return NULL;
		}
	}
	return new VdfnToggleNode( eMode, bInitState );
}

// #############################################################################

IVdfnNode * VdfnOutstreamNodeCreate::CreateNode( const VistaPropertyList& oParams ) const
{
	const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );

	std::ostream* pStream;

	std::string sStreamName = oSubs.GetValueOrDefault<std::string>( "stream", "out" );
	VistaAspectsComparisonStuff::StringCompareObject oCmp( false );
	if( oCmp( sStreamName, "out" ) )
		pStream = &vstr::out();
	else if( oCmp( sStreamName, "warn" ) )
		pStream = &vstr::warn();
	else if( oCmp( sStreamName, "err" ) )
		pStream = &vstr::err();
	else if( oCmp( sStreamName, "debug" ) )
		pStream = &vstr::debug();
	else if( oCmp( sStreamName, "cout" ) )
		pStream = &std::cout;
	else if( oCmp( sStreamName, "cerr" ) )
		pStream = &std::cerr;
	else if( oCmp( sStreamName, "clog" ) )
		pStream = &std::clog;
	else
	{
		if( vstr::GetStreamManager()->GetHasStream( sStreamName ) == false )
		{
			vstr::warnp() << "[VdfnOutstreamNodeCreate] no stream ["
							<< sStreamName << " defined" << std::endl;
			return NULL;
		}
		pStream = &vstr::Stream( sStreamName );
	}

	VdfnOutstreamNode* pNode = new VdfnOutstreamNode( *pStream );
	pNode->SetPrefix( oSubs.GetValueOrDefault<std::string>( "prefix", "" ) );
	pNode->SetPostfix( oSubs.GetValueOrDefault<std::string>( "postfix", "\n" ) );
	return pNode;
}

VISTADFNAPI bool VdfnNodeCreators::RegisterNodeCreators( VdfnNodeFactory* pFac, VistaDriverMap* pDrivers, VdfnObjectRegistry* pObjRegistry )
{
	VdfnTypeConvertNodeCreators::RegisterTypeConvertNodeCreators( pFac );

	pFac->SetNodeCreator( "Shallow", new VdfnShallowNodeCreate );
	pFac->SetNodeCreator( "Timer", new VdfnTimerNodeCreate );
	pFac->SetNodeCreator( "TickTimer", new VdfnTickTimerNodeCreate );

	pFac->SetNodeCreator( "DriverSensor", new VdfnDriverSensorNodeCreate( pDrivers, pObjRegistry ) );
	pFac->SetNodeCreator( "Action", new VdfnActionNodeCreate(pObjRegistry) );
	pFac->SetNodeCreator( "HistoryProject", new VdfnHistoryProjectNodeCreate );

	pFac->SetNodeCreator( "Composite", new VdfnCompositeNodeCreate );

	pFac->SetNodeCreator( "Sampler", new VdfnSamplerNodeCreate );

	pFac->SetNodeCreator( "UpdateThreshold", new VdfnUpdateThresholdNodeCreate );
	pFac->SetNodeCreator( "Threshold[int]", new VdfnThresholdNodeCreate<int> );
	pFac->SetNodeCreator( "Threshold[unsigned int]", new VdfnThresholdNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "Threshold[float]", new VdfnThresholdNodeCreate<float> );
	pFac->SetNodeCreator( "Threshold[double]", new VdfnThresholdNodeCreate<double> );

	pFac->SetNodeCreator( "Logger", new VdfnLoggerNodeCreate );
	pFac->SetNodeCreator( "DumpHistory", new VdfnDumpHistoryDefaultCreate );

	pFac->SetNodeCreator( "ConstantValue[unsigned int]", new VdfnConstantValueNodeCreate<unsigned int>() );
	pFac->SetNodeCreator( "ConstantValue[int]", new VdfnConstantValueNodeCreate<int>() );
	pFac->SetNodeCreator( "ConstantValue[float]", new VdfnConstantValueNodeCreate<float>() );
	pFac->SetNodeCreator( "ConstantValue[double]", new VdfnConstantValueNodeCreate<double>() );
	pFac->SetNodeCreator( "ConstantValue[bool]", new VdfnConstantValueNodeCreate<bool>() );
	pFac->SetNodeCreator( "ConstantValue[string]", new VdfnConstantValueNodeCreate<std::string>() );
	pFac->SetNodeCreator( "ConstantValue[VistaVector3D]", new VdfnConstantValueNodeCreate<VistaVector3D>() );
	pFac->SetNodeCreator( "ConstantValue[VistaQuaternion]", new VdfnConstantValueNodeCreate<VistaQuaternion>() );
	pFac->SetNodeCreator( "ConstantValue[VistaTransformMatrix]", new VdfnConstantValueNodeCreate<VistaTransformMatrix>() );
	pFac->SetNodeCreator( "EnvValue", new VdfnEnvStringValueNodeCreate );

	pFac->SetNodeCreator( "ChangeDetect[int]", new TVdfnChangeDetectNodeCreate<int> );
	pFac->SetNodeCreator( "ChangeDetect[unsigned int]", new TVdfnChangeDetectNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "ChangeDetect[int64]", new TVdfnChangeDetectNodeCreate<DLV_INT64> );
	pFac->SetNodeCreator( "ChangeDetect[VistaType::uint64]", new TVdfnChangeDetectNodeCreate<VistaType::uint64> );
	pFac->SetNodeCreator( "ChangeDetect[float]", new TVdfnChangeDetectNodeCreate<float> );
	pFac->SetNodeCreator( "ChangeDetect[double]", new TVdfnChangeDetectNodeCreate<double> );
	pFac->SetNodeCreator( "ChangeDetect[bool]", new TVdfnChangeDetectNodeCreate<bool> );
	pFac->SetNodeCreator( "ChangeDetect[VistaVector3D]", new TVdfnChangeDetectNodeCreate<VistaVector3D> );
	pFac->SetNodeCreator( "ChangeDetect[VistaQuaternion]", new TVdfnChangeDetectNodeCreate<VistaQuaternion> );

	//pFac->SetNodeCreator( "Aggregate[bool]", new VdfnAggregateNodeCreate<bool> );
	pFac->SetNodeCreator( "Aggregate[int]", new VdfnAggregateNodeCreate<int> );
	pFac->SetNodeCreator( "Aggregate[unsigned int]", new VdfnAggregateNodeCreate<int> );
	pFac->SetNodeCreator( "Aggregate[float]", new VdfnAggregateNodeCreate<float> );
	pFac->SetNodeCreator( "Aggregate[double]", new VdfnAggregateNodeCreate<double> );
	pFac->SetNodeCreator( "Aggregate[string]", new VdfnAggregateNodeCreate<std::string> );
	pFac->SetNodeCreator( "Aggregate[VistaVector3D]", new VdfnAggregateNodeCreate<VistaVector3D> );
	pFac->SetNodeCreator( "Aggregate[VistaQuaternion]", new VdfnAggregateNodeCreate<VistaQuaternion> );
	pFac->SetNodeCreator( "Aggregate[VistaTransformMatrix]", new VdfnAggregateNodeCreate<VistaTransformMatrix> );
		
	pFac->SetNodeCreator( "Add[string]", new TVdfnBinOpCreate<std::string,std::string,std::string>( new VdfnBinaryOps::AddOp<std::string,std::string,std::string>) );
	pFac->SetNodeCreator( "Add[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnBinaryOps::AddOp<int,int,int> ) );
	pFac->SetNodeCreator( "Add[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,unsigned int>( new VdfnBinaryOps::AddOp<unsigned int,unsigned int,unsigned int> ) );
	pFac->SetNodeCreator( "Add[float]", new TVdfnBinOpCreate<float,float,float>( new VdfnBinaryOps::AddOp<float,float,float> ) );
	pFac->SetNodeCreator( "Add[double]", new TVdfnBinOpCreate<double,double,double>( new VdfnBinaryOps::AddOp<double,double,double> ) );
	pFac->SetNodeCreator( "Add[VistaVector3D]", new TVdfnBinOpCreate<VistaVector3D,VistaVector3D,VistaVector3D>( new VdfnBinaryOps::AddOp<VistaVector3D,VistaVector3D,VistaVector3D>) );

	pFac->SetNodeCreator( "Substract[VistaVector3D]", new TVdfnBinOpCreate<VistaVector3D,VistaVector3D,VistaVector3D>( new VdfnBinaryOps::SubOp<VistaVector3D,VistaVector3D,VistaVector3D>) );
	pFac->SetNodeCreator( "Substract[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnBinaryOps::SubOp<int,int,int>) );
	pFac->SetNodeCreator( "Substract[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,unsigned int>( new VdfnBinaryOps::SubOp<unsigned int,unsigned int,unsigned int>) );
	pFac->SetNodeCreator( "Substract[float]", new TVdfnBinOpCreate<float,float,float>( new VdfnBinaryOps::SubOp<float,float,float>) );
	pFac->SetNodeCreator( "Substract[double]", new TVdfnBinOpCreate<double,double,double>( new VdfnBinaryOps::SubOp<double,double,double>) );

	pFac->SetNodeCreator( "Multiply[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnBinaryOps::MultOp<int,int,int> ) );
	pFac->SetNodeCreator( "Multiply[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,unsigned int>( new VdfnBinaryOps::MultOp<unsigned int,unsigned int,unsigned int> ) );
	pFac->SetNodeCreator( "Multiply[float]", new TVdfnBinOpCreate<float,float,float>( new VdfnBinaryOps::MultOp<float,float,float> ) );
	pFac->SetNodeCreator( "Multiply[double]", new TVdfnBinOpCreate<double,double,double>( new VdfnBinaryOps::MultOp<double,double,double> ) );

	pFac->SetNodeCreator( "Divide[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnBinaryOps::DivOp<int,int,int> ) );
	pFac->SetNodeCreator( "Divide[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,unsigned int>( new VdfnBinaryOps::DivOp<unsigned int,unsigned int,unsigned int> ) );
	pFac->SetNodeCreator( "Divide[float]", new TVdfnBinOpCreate<float,float,float>( new VdfnBinaryOps::DivOp<float,float,float> ) );
	pFac->SetNodeCreator( "Divide[double]", new TVdfnBinOpCreate<double,double,double>( new VdfnBinaryOps::DivOp<double,double,double> ) );

	pFac->SetNodeCreator( "Multiply[VistaTransformMatrix]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaTransformMatrix, VistaTransformMatrix>(
															  new VdfnBinaryOps::MultOp<VistaTransformMatrix, VistaTransformMatrix, VistaTransformMatrix> ) );
	pFac->SetNodeCreator( "Multiply[VistaQuaternion]", new TVdfnBinOpCreate<VistaQuaternion, VistaQuaternion, VistaQuaternion>(
															  new VdfnBinaryOps::MultOp<VistaQuaternion, VistaQuaternion, VistaQuaternion> ) );

	pFac->SetNodeCreator( "Multiply[int,VistaVector3D]", new TVdfnBinOpCreate<int, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::MultOp<int, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[unsigned int,VistaVector3D]", new TVdfnBinOpCreate<unsigned int, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::MultOp<unsigned int, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[float,VistaVector3D]", new TVdfnBinOpCreate<float, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::MultOp<float, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[double,VistaVector3D]", new TVdfnBinOpCreate<double, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::MultOp<double, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[VistaTransformMatrix,VistaVector3D]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::MultOp<VistaTransformMatrix, VistaVector3D, VistaVector3D> ) );

	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,VistaVector3D]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,VistaQuaternion]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaQuaternion, VistaQuaternion>(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, VistaQuaternion, VistaQuaternion> ) );
	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,VistaTransformMatrix]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaTransformMatrix, VistaTransformMatrix>(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, VistaTransformMatrix, VistaTransformMatrix> ) );
	pFac->SetNodeCreator( "Transform[VistaVector3D,VistaVector3D]", new TVdfnBinOpCreate<VistaVector3D, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::TransformOp<VistaVector3D, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Transform[VistaVector3D,VistaTransformMatrix]", new TVdfnBinOpCreate<VistaVector3D, VistaTransformMatrix, VistaTransformMatrix>(
															  new VdfnBinaryOps::TransformOp<VistaVector3D, VistaTransformMatrix, VistaTransformMatrix> ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,VistaVector3D]", new TVdfnBinOpCreate<VistaQuaternion, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,VistaQuaternion]", new TVdfnBinOpCreate<VistaQuaternion, VistaQuaternion, VistaQuaternion>(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, VistaQuaternion, VistaQuaternion> ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,VistaTransformMatrix]", new TVdfnBinOpCreate<VistaQuaternion, VistaTransformMatrix, VistaTransformMatrix>(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, VistaTransformMatrix, VistaTransformMatrix> ) );

	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,vector[VistaVector3D]]", new TVdfnBinOpCreate<VistaTransformMatrix, std::vector<VistaVector3D>, std::vector<VistaVector3D> >(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, std::vector<VistaVector3D>, std::vector<VistaVector3D> > ) );
	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,vector[VistaQuaternion]]", new TVdfnBinOpCreate<VistaTransformMatrix, std::vector<VistaQuaternion>, std::vector<VistaQuaternion> >(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, std::vector<VistaQuaternion>, std::vector<VistaQuaternion> > ) );
	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,vector[VistaTransformMatrix]]", new TVdfnBinOpCreate<VistaTransformMatrix, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> >(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> > ) );
	pFac->SetNodeCreator( "Transform[VistaVector3D,vector[VistaVector3D]]", new TVdfnBinOpCreate<VistaVector3D, std::vector<VistaVector3D>, std::vector<VistaVector3D> >(
															  new VdfnBinaryOps::TransformOp<VistaVector3D, std::vector<VistaVector3D>, std::vector<VistaVector3D> > ) );
	pFac->SetNodeCreator( "Transform[VistaVector3D,vector[VistaTransformMatrix]]", new TVdfnBinOpCreate<VistaVector3D, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> >(
															  new VdfnBinaryOps::TransformOp<VistaVector3D, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> > ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,vector[VistaVector3D]]", new TVdfnBinOpCreate<VistaQuaternion, std::vector<VistaVector3D>, std::vector<VistaVector3D> >(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, std::vector<VistaVector3D>, std::vector<VistaVector3D> > ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,vector[VistaQuaternion]]", new TVdfnBinOpCreate<VistaQuaternion, std::vector<VistaQuaternion>, std::vector<VistaQuaternion> >(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, std::vector<VistaQuaternion>, std::vector<VistaQuaternion> > ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,vector[VistaTransformMatrix]]", new TVdfnBinOpCreate<VistaQuaternion, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> >(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> > ) );

	pFac->SetNodeCreator( "Equals[string]", new TVdfnBinOpCreate<std::string,std::string,bool>( new VdfnBinaryOps::EqualsOp<std::string,std::string,bool> ) );
	pFac->SetNodeCreator( "Equals[bool]", new TVdfnBinOpCreate<bool,bool,bool>( new VdfnBinaryOps::EqualsOp<bool,bool,bool> ) );
	pFac->SetNodeCreator( "Equals[int]", new TVdfnBinOpCreate<int,int,bool>( new VdfnBinaryOps::EqualsOp<int,int,bool> ) );
	pFac->SetNodeCreator( "Equals[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,bool>( new VdfnBinaryOps::EqualsOp<unsigned int,unsigned int,bool> ) );
	pFac->SetNodeCreator( "Equals[float]", new TVdfnBinOpCreate<float,float,bool>( new VdfnBinaryOps::EqualsOp<float,float,bool> ) );
	pFac->SetNodeCreator( "Equals[double]", new TVdfnBinOpCreate<double,double,bool>( new VdfnBinaryOps::EqualsOp<double,double,bool> ) );
	pFac->SetNodeCreator( "Equals[VistaVector3D]", new TVdfnBinOpCreate<VistaVector3D,VistaVector3D,bool>( new VdfnBinaryOps::EqualsOp<VistaVector3D,VistaVector3D,bool> ) );
	pFac->SetNodeCreator( "Equals[VistaQuaternion]", new TVdfnBinOpCreate<VistaQuaternion,VistaQuaternion,bool>( new VdfnBinaryOps::EqualsOp<VistaQuaternion,VistaQuaternion,bool> ) );
	pFac->SetNodeCreator( "Equals[VistaTransformMatrix]", new TVdfnBinOpCreate<VistaTransformMatrix,VistaTransformMatrix,bool>( new VdfnBinaryOps::EqualsOp<VistaTransformMatrix,VistaTransformMatrix,bool> ) );

	pFac->SetNodeCreator( "Max[int]", new TVdfnBinOpCreate<int, int, int>( new VdfnBinaryOps::MaxOp<int> ) );
	pFac->SetNodeCreator( "Max[unsigned int]", new TVdfnBinOpCreate<unsigned int, unsigned int, unsigned int>( new VdfnBinaryOps::MaxOp<unsigned int> ) );
	pFac->SetNodeCreator( "Max[float]", new TVdfnBinOpCreate<float, float, float>( new VdfnBinaryOps::MaxOp<float> ) );
	pFac->SetNodeCreator( "Max[double]", new TVdfnBinOpCreate<double, double, double>( new VdfnBinaryOps::MaxOp<double> ) );

	pFac->SetNodeCreator( "Min[int]", new TVdfnBinOpCreate<int, int, int>( new VdfnBinaryOps::MinOp<int> ) );
	pFac->SetNodeCreator( "Min[unsigned int]", new TVdfnBinOpCreate<unsigned int, unsigned int, unsigned int>( new VdfnBinaryOps::MinOp<unsigned int> ) );
	pFac->SetNodeCreator( "Min[float]", new TVdfnBinOpCreate<float, float, float>( new VdfnBinaryOps::MinOp<float> ) );
	pFac->SetNodeCreator( "Min[double]", new TVdfnBinOpCreate<double, double, double>( new VdfnBinaryOps::MinOp<double> ) );

	pFac->SetNodeCreator( "Less[int]", new TVdfnBinOpCreate<int, int, bool>( new VdfnBinaryOps::LessOp<int> ) );
	pFac->SetNodeCreator( "Less[unsigned int]", new TVdfnBinOpCreate<unsigned int, unsigned int, bool>( new VdfnBinaryOps::LessOp<unsigned int> ) );
	pFac->SetNodeCreator( "Less[float]", new TVdfnBinOpCreate<float, float, bool>( new VdfnBinaryOps::LessOp<float> ) );
	pFac->SetNodeCreator( "Less[double]", new TVdfnBinOpCreate<double, double, bool>( new VdfnBinaryOps::LessOp<double> ) );

	pFac->SetNodeCreator( "LessEqual[int]", new TVdfnBinOpCreate<int, int, bool>( new VdfnBinaryOps::LessEqualOp<int> ) );
	pFac->SetNodeCreator( "LessEqual[unsigned int]", new TVdfnBinOpCreate<unsigned int, unsigned int, bool>( new VdfnBinaryOps::LessEqualOp<unsigned int> ) );
	pFac->SetNodeCreator( "LessEqual[float]", new TVdfnBinOpCreate<float, float, bool>( new VdfnBinaryOps::LessEqualOp<float> ) );
	pFac->SetNodeCreator( "LessEqual[double]", new TVdfnBinOpCreate<double, double, bool>( new VdfnBinaryOps::LessEqualOp<double> ) );

	pFac->SetNodeCreator( "Compose3DVector", new TVdfnDefaultNodeCreate< VdfnCompose3DVectorNode > );
	pFac->SetNodeCreator( "ComposeQuaternion", new TVdfnDefaultNodeCreate< VdfnComposeQuaternionNode > );
	pFac->SetNodeCreator( "ComposeQuaternionFromDirections", new TVdfnDefaultNodeCreate< VdfnComposeQuaternionFromDirectionsNode > );
	pFac->SetNodeCreator( "ComposeTransformMatrix", new TVdfnDefaultNodeCreate< VdfnComposeTransformMatrixNode > );
	pFac->SetNodeCreator( "Decompose3DVector", new TVdfnDefaultNodeCreate< VdfnDecompose3DVectorNode > );
	pFac->SetNodeCreator( "DecomposeQuaternion", new TVdfnDefaultNodeCreate< VdfnDecomposeQuaternionNode > );
	pFac->SetNodeCreator( "DecomposeTransformMatrix", new TVdfnDefaultNodeCreate< VdfnDecomposeTransformMatrixNode > );

	pFac->SetNodeCreator( "QuaternionSlerp", new TVdfnDefaultNodeCreate<VdfnQuaternionSlerpNode> );

	pFac->SetNodeCreator( "And[bool]", new TVdfnBinOpCreate<bool,bool,bool>( new VdfnBinaryOps::AndOp<bool,bool,bool> ) );
	pFac->SetNodeCreator( "Or[bool]", new TVdfnBinOpCreate<bool,bool,bool>( new VdfnBinaryOps::OrOp<bool,bool,bool> ) );
	pFac->SetNodeCreator( "Toggle", new VdfnToggleNodeCreate );

	pFac->SetNodeCreator( "ValueToTrigger[int]", new TVdfnValueToTriggerNodeCreate<int> );
	pFac->SetNodeCreator( "ValueToTrigger[unsigned int]", new TVdfnValueToTriggerNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "ConditionalRoute", new TVdfnDefaultNodeCreate<VdfnConditionalRouteNode> );

	pFac->SetNodeCreator( "Counter[int]", new TVdfnCounterNodeCreate<int> );
	pFac->SetNodeCreator( "Counter[unsigned int]", new TVdfnCounterNodeCreate<int> );
	pFac->SetNodeCreator( "Counter[float]", new TVdfnCounterNodeCreate<int> );
	pFac->SetNodeCreator( "Counter[double]", new TVdfnCounterNodeCreate<int> );
	pFac->SetNodeCreator( "ModuloCounter[int]", new TVdfnModuloCounterNodeCreate<int> );
	pFac->SetNodeCreator( "ModuloCounter[unsigned int]", new TVdfnModuloCounterNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "ModuloCounter[float]", new TVdfnModuloCounterNodeCreate<float> );
	pFac->SetNodeCreator( "ModuloCounter[double]", new TVdfnModuloCounterNodeCreate<double> );

	pFac->SetNodeCreator( "Modulo[int]", new TVdfnModuloNodeCreate<int> );
	pFac->SetNodeCreator( "Modulo[unsigned int]", new TVdfnModuloNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "Modulo[float]", new TVdfnModuloNodeCreate<float> );
	pFac->SetNodeCreator( "Modulo[double]", new TVdfnModuloNodeCreate<double> );
	
	pFac->SetNodeCreator( "Absolute[int]", new TVdfnDefaultNodeCreate<TVdfnAbsoluteNode<int> > );
	pFac->SetNodeCreator( "Absolute[float]", new TVdfnDefaultNodeCreate<TVdfnAbsoluteNode<float> > );
	pFac->SetNodeCreator( "Absolute[double]", new TVdfnDefaultNodeCreate<TVdfnAbsoluteNode<double> > );

	pFac->SetNodeCreator( "GetTransform", new VdfnGetTransformNodeCreate(pObjRegistry) );
	pFac->SetNodeCreator( "SetTransform", new VdfnSetTransformNodeCreate(pObjRegistry) );
	pFac->SetNodeCreator( "ApplyTransform", new VdfnApplyTransformNodeCreate(pObjRegistry) );

	pFac->SetNodeCreator( "3DNormalize", new Vista3DNormalizeNodeCreate );

	pFac->SetNodeCreator( "LatestUpdate[VistaVector3D]", new TVdfnDefaultNodeCreate<TVdfnLatestUpdateNode<VistaVector3D> > );
	pFac->SetNodeCreator( "LatestUpdate[bool]", new TVdfnDefaultNodeCreate<TVdfnLatestUpdateNode<bool> > );
	pFac->SetNodeCreator( "LatestUpdate[int]", new TVdfnDefaultNodeCreate<TVdfnLatestUpdateNode<int> > );

	pFac->SetNodeCreator( "AxisRotate", new TVdfnDefaultNodeCreate<VdfnAxisRotateNode> );
	pFac->SetNodeCreator( "MatrixCompose", new TVdfnDefaultNodeCreate<VdfnMatrixComposeNode> );
	pFac->SetNodeCreator( "ProjectVector", new VdfnProjectVectorNodeCreate );
	pFac->SetNodeCreator( "ForceFeedback", new VdfnForceFeedbackNodeCreate( pDrivers ) );
	pFac->SetNodeCreator( "ReadWorkspace", new VdfnReadWorkspaceNodeCreate(pDrivers) );

	pFac->SetNodeCreator( "GetElement[vector<int>]", new TVdfnGetElementNodeCreate<std::vector<int>, int> );
	pFac->SetNodeCreator( "GetElement[vector<usnigned int>]", new TVdfnGetElementNodeCreate<std::vector<unsigned int>, unsigned int> );
	pFac->SetNodeCreator( "GetElement[vector<float>]", new TVdfnGetElementNodeCreate<std::vector<float>, float> );
	pFac->SetNodeCreator( "GetElement[vector<double>]", new TVdfnGetElementNodeCreate<std::vector<double>, double> );
	pFac->SetNodeCreator( "GetElement[vector<VistaVector3D>]", new TVdfnGetElementNodeCreate<std::vector<VistaVector3D>, VistaVector3D> );
	pFac->SetNodeCreator( "GetElement[vector<VistaQuaternion>]", new TVdfnGetElementNodeCreate<std::vector<VistaQuaternion>, VistaQuaternion> );
	pFac->SetNodeCreator( "GetElement[vector<VistaTransformMatrix>]", new TVdfnGetElementNodeCreate<std::vector<VistaTransformMatrix>, VistaTransformMatrix> );
	pFac->SetNodeCreator( "GetElement[VistaVector3D]", new TVdfnGetElementFromArrayNodeCreate<VistaVector3D, float, 4> );
	pFac->SetNodeCreator( "GetElement[VistaQuaternion]", new TVdfnGetElementFromArrayNodeCreate<VistaQuaternion, float, 4> );
	
	pFac->SetNodeCreator( "Invert[VistaQuaternion]", new TVdfnDefaultNodeCreate<TVdfnInvertNode<VistaQuaternion> >);
	pFac->SetNodeCreator( "Invert[VistaTransformMatrix]", new TVdfnDefaultNodeCreate<TVdfnInvertNode<VistaTransformMatrix> >);
	pFac->SetNodeCreator( "Invert[bool]", new TVdfnDefaultNodeCreate<TVdfnInvertNode<bool> >);
	pFac->SetNodeCreator( "Invert[float]", new TVdfnDefaultNodeCreate<TVdfnInvertNode<float> >);
	pFac->SetNodeCreator( "Invert[double]", new TVdfnDefaultNodeCreate<TVdfnInvertNode<double> >);

	pFac->SetNodeCreator( "Negate[int]", new TVdfnDefaultNodeCreate<TVdfnNegateNode<int> >);
	pFac->SetNodeCreator( "Negate[VistaVector3D]", new TVdfnDefaultNodeCreate<TVdfnNegateNode<VistaVector3D> >);
	pFac->SetNodeCreator( "Negate[float]", new TVdfnDefaultNodeCreate<TVdfnNegateNode<float> >);
	pFac->SetNodeCreator( "Negate[double]", new TVdfnDefaultNodeCreate<TVdfnNegateNode<double> >);

	pFac->SetNodeCreator( "RangeCheck[int]", new TVdfnRangeCheckNodeCreate<int> );
	pFac->SetNodeCreator( "RangeCheck[unsigned int]", new TVdfnRangeCheckNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "RangeCheck[float]", new TVdfnRangeCheckNodeCreate<float> );
	pFac->SetNodeCreator( "RangeCheck[double]", new TVdfnRangeCheckNodeCreate<double> );

	pFac->SetNodeCreator( "Delay[bool]", new VdfnDelayNodeCreate<bool> );
	pFac->SetNodeCreator( "Delay[int]", new VdfnDelayNodeCreate<int> );
	pFac->SetNodeCreator( "Delay[unsigned int]", new VdfnDelayNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "Delay[float]", new VdfnDelayNodeCreate<float> );
	pFac->SetNodeCreator( "Delay[double]", new VdfnDelayNodeCreate<double> );
	pFac->SetNodeCreator( "Delay[string]", new VdfnDelayNodeCreate<std::string> );
	pFac->SetNodeCreator( "Delay[VistaVector3D]", new VdfnDelayNodeCreate<VistaVector3D> );
	pFac->SetNodeCreator( "Delay[VistaQuaternion]", new VdfnDelayNodeCreate<VistaQuaternion> );
	pFac->SetNodeCreator( "Delay[VistaTransformMatrix]", new VdfnDelayNodeCreate<VistaTransformMatrix> );

	pFac->SetNodeCreator( "Difference[int]", new TVdfnDefaultNodeCreate< TVdfnDifferenceNode<int> >() );
	pFac->SetNodeCreator( "Difference[unsigned int]", new TVdfnDefaultNodeCreate< TVdfnDifferenceNode<unsigned int> >() );
	pFac->SetNodeCreator( "Difference[float]", new TVdfnDefaultNodeCreate< TVdfnDifferenceNode<float> >() );
	pFac->SetNodeCreator( "Difference[double]", new TVdfnDefaultNodeCreate< TVdfnDifferenceNode<double> >() );

	pFac->SetNodeCreator( "Multiplex[int]", new TVdfnDefaultNodeCreate<TVdfnMultiplexNode<int> > );
	pFac->SetNodeCreator( "Multiplex[unsigned int]", new TVdfnDefaultNodeCreate<TVdfnMultiplexNode<unsigned int> > );
	pFac->SetNodeCreator( "Multiplex[bool]", new TVdfnDefaultNodeCreate<TVdfnMultiplexNode<bool> > );
	pFac->SetNodeCreator( "Multiplex[float]", new TVdfnDefaultNodeCreate<TVdfnMultiplexNode<float> > );
	pFac->SetNodeCreator( "Multiplex[double]", new TVdfnDefaultNodeCreate<TVdfnMultiplexNode<double> > );
	pFac->SetNodeCreator( "Multiplex[VistaVector3D]", new TVdfnDefaultNodeCreate<TVdfnMultiplexNode<VistaVector3D> > );
	pFac->SetNodeCreator( "Multiplex[VistaQuaternion]", new TVdfnDefaultNodeCreate<TVdfnMultiplexNode<VistaQuaternion> > );
	pFac->SetNodeCreator( "Multiplex[VistaTransformMatrix]", new TVdfnDefaultNodeCreate<TVdfnMultiplexNode<VistaTransformMatrix> > );
	pFac->SetNodeCreator( "Multiplex[string]", new TVdfnDefaultNodeCreate<TVdfnMultiplexNode<std::string> > );

	pFac->SetNodeCreator( "Demultiplex[int]", new TVdfnDemultiplexNodeCreate<int> );
	pFac->SetNodeCreator( "Demultiplex[unsigned int]", new TVdfnDemultiplexNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "Demultiplex[bool]", new TVdfnDemultiplexNodeCreate<bool> );
	pFac->SetNodeCreator( "Demultiplex[float]", new TVdfnDemultiplexNodeCreate<float> );
	pFac->SetNodeCreator( "Demultiplex[double]", new TVdfnDemultiplexNodeCreate<double> );
	pFac->SetNodeCreator( "Demultiplex[VistaVector3D]", new TVdfnDemultiplexNodeCreate<VistaVector3D> );
	pFac->SetNodeCreator( "Demultiplex[VistaQuaternion]", new TVdfnDemultiplexNodeCreate<VistaQuaternion> );
	pFac->SetNodeCreator( "Demultiplex[VistaTransformMatrix]", new TVdfnDemultiplexNodeCreate<VistaTransformMatrix> );
	pFac->SetNodeCreator( "Demultiplex[string]", new TVdfnDemultiplexNodeCreate<std::string> );


	pFac->SetNodeCreator( "SetVariable[int]", new TVdfnVariableNodeCreate<int>( TVdfnVariableNodeCreate<int>::VN_SETTER ) );
	pFac->SetNodeCreator( "SetVariable[unsigned int]", new TVdfnVariableNodeCreate<unsigned int>( TVdfnVariableNodeCreate<unsigned int>::VN_SETTER ) );
	pFac->SetNodeCreator( "SetVariable[bool]", new TVdfnVariableNodeCreate<bool>( TVdfnVariableNodeCreate<bool>::VN_SETTER ) );
	pFac->SetNodeCreator( "SetVariable[float]", new TVdfnVariableNodeCreate<float>( TVdfnVariableNodeCreate<float>::VN_SETTER ) );
	pFac->SetNodeCreator( "SetVariable[double]", new TVdfnVariableNodeCreate<double>( TVdfnVariableNodeCreate<double>::VN_SETTER ) );
	pFac->SetNodeCreator( "SetVariable[VistaVector3D]", new TVdfnVariableNodeCreate<VistaVector3D>( TVdfnVariableNodeCreate<VistaVector3D>::VN_SETTER ) );
	pFac->SetNodeCreator( "SetVariable[VistaQuaternion]", new TVdfnVariableNodeCreate<VistaQuaternion>( TVdfnVariableNodeCreate<VistaQuaternion>::VN_SETTER ) );
	pFac->SetNodeCreator( "SetVariable[VistaTransformMatrix]", new TVdfnVariableNodeCreate<VistaTransformMatrix>( TVdfnVariableNodeCreate<VistaTransformMatrix>::VN_SETTER ) );
	pFac->SetNodeCreator( "SetVariable[string]", new TVdfnVariableNodeCreate<std::string>( TVdfnVariableNodeCreate<std::string>::VN_SETTER ) );

	pFac->SetNodeCreator( "GetVariable[int]", new TVdfnVariableNodeCreate<int>( TVdfnVariableNodeCreate<int>::VN_GETTER ) );
	pFac->SetNodeCreator( "GetVariable[unsigned int]", new TVdfnVariableNodeCreate<unsigned int>( TVdfnVariableNodeCreate<unsigned int>::VN_GETTER ) );
	pFac->SetNodeCreator( "GetVariable[bool]", new TVdfnVariableNodeCreate<bool>( TVdfnVariableNodeCreate<bool>::VN_GETTER ) );
	pFac->SetNodeCreator( "GetVariable[float]", new TVdfnVariableNodeCreate<float>( TVdfnVariableNodeCreate<float>::VN_GETTER ) );
	pFac->SetNodeCreator( "GetVariable[double]", new TVdfnVariableNodeCreate<double>( TVdfnVariableNodeCreate<double>::VN_GETTER ) );
	pFac->SetNodeCreator( "GetVariable[VistaVector3D]", new TVdfnVariableNodeCreate<VistaVector3D>( TVdfnVariableNodeCreate<VistaVector3D>::VN_GETTER ) );
	pFac->SetNodeCreator( "GetVariable[VistaQuaternion]", new TVdfnVariableNodeCreate<VistaQuaternion>( TVdfnVariableNodeCreate<VistaQuaternion>::VN_GETTER ) );
	pFac->SetNodeCreator( "GetVariable[VistaTransformMatrix]", new TVdfnVariableNodeCreate<VistaTransformMatrix>( TVdfnVariableNodeCreate<VistaTransformMatrix>::VN_GETTER ) );
	pFac->SetNodeCreator( "GetVariable[string]", new TVdfnVariableNodeCreate<std::string>( TVdfnVariableNodeCreate<std::string>::VN_GETTER ) );

	pFac->SetNodeCreator( "Outstream", new VdfnOutstreamNodeCreate );


	pFac->SetNodeCreator( "TrackingRedundancy", new TVdfnDefaultNodeCreate<VdfnTrackingRedundancyNode> );

	return true;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


