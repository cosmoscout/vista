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


#ifndef _VDFNNODECREATORS_H
#define _VDFNNODECREATORS_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>
#include "VdfnSerializer.h"

#include "VdfnNodeFactory.h"

#include "VdfnConstantValueNode.h"
#include "VdfnChangeDetectNode.h"
#include "VdfnAggregateNode.h"
#include "VdfnVectorDecomposeNode.h"
#include "VdfnCounterNode.h"
#include "VdfnModuloCounterNode.h"
#include "VdfnThresholdNode.h"
#include "VdfnGetElementNode.h"
#include "VdfnRangeCheckNode.h"
#include "VdfnDemultiplexNode.h"
#include "VdfnVariableNode.h"

#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*
 * this file is meant to contain node creators. In the DataFlowNet, node
 * creation is separated from the nodes on purpose most of the time.
 * Different applications may need to have different means of creating nodes.
 * All node creators expect a proplist with one sub-list, called 'params'.
 * This is in term evaluated further.
 */

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverMap;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

namespace VdfnNodeCreators
{
	/**
	 * management API, registers the basically available node creators for the
	 * available node types. Is called by InitVdfn(), so usually there is no need
	 * to call this methods on your own, use InitVdfn() instead. Nodes creators will
	 * register themselves in the VdfnNodeFactory singleton.
	 * @param a non-null pointer to the NodeFactory
	 * @param a non-null pointer to a valid driver map which must outlive the whole
	          application life cycle.
	 * @param pObjRegistry the object registry to use for nodes searching entities.
	          the registry must outlive the whole application life cycle.
	 * @return true
	 */
	VISTADFNAPI bool RegisterNodeCreators( VdfnNodeFactory* pFac, VistaDriverMap* pDrivers, VdfnObjectRegistry* pObjRegistry );
}

/**
 * creates instances of type VdfnShallowNode.
 * reads in oParams for
 * - inports: pairs <name, value> of inports to create and assign
              to the shallow node. needs a registration of port access structures
              in the VdfnPortFactory in order to work properly.
 * - outports: pairs of <name,value> of outports to create and assign
 *
 * inports and outports are optional arguments
 */
class VISTADFNAPI VdfnShallowNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    /**
     * inports: subtree of name=type lists
     * outports: subtree of name=type lists
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

/**
 * creates a VdfnTimerNode.
 */
class VISTADFNAPI VdfnTimerNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    /**
     * single argument:
     * - reset_on_activate: type bool, controls whether the timer will be reset
                            on a call to activate on the node.
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

/**
 * creates VdfnTickTimerNode
 */
class VdfnTickTimerNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    /**
     * no arguments
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

/**
 * creates VdfnUpdateThresholdNode
 */
class VdfnUpdateThresholdNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

/**
 * creates VdfnThresholdNode
 */

template<class T>
class VdfnThresholdNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	 /**
     * arguments
     * - threshold: [number,optional] - the name of the driver that contains the sensor of desire
     * - compare_absolute_value: [bool,optional] - sets wether or not the absolute should be tested
     * - mode: [string,optional] - sets how values below the threshold are handles. options are:
	 *                             block | output_zero | output_threshold | output_last_value
     */
    virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const
	{
		
		int iMode = VdfnThresholdNode<T>::BTHM_BLOCK;
		const VistaPropertyList &subs 
				= oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		double dThreshold = 0.0;
		subs.GetValue( "threshold", dThreshold );

		bool bUseAbsoluteValue = subs.GetValueOrDefault<bool>( "compare_absolute_value", false );
		bool bSubtractThreshold = subs.GetValueOrDefault<bool>( "subtract_threshold", false );

		std::string sModeName;		
		if( subs.GetValue( "mode", sModeName ) )
		{			
			if( sModeName == "block" )
				iMode = VdfnThresholdNode<T>::BTHM_BLOCK;
			else if( sModeName == "output_zero" )
				iMode = VdfnThresholdNode<T>::BTHM_OUTPUT_ZERO;
			else if( sModeName == "output_threshold" )
				iMode = VdfnThresholdNode<T>::BTHM_OUTPUT_THRESHOLD;
			else if( sModeName == "output_last_value" )
				iMode = VdfnThresholdNode<T>::BTHM_OUTPUT_LAST_VALID_VALUE;
		}


		return new VdfnThresholdNode<T>( static_cast<T>(dThreshold), bUseAbsoluteValue, bSubtractThreshold, iMode );
	}
};

/**
 * creates VdfnGetTransformNode, needs a pointer to VdfnObjectRegistry to pass to the node in
 * case the target object is not found during a call to CreateNode()
 */
class VISTADFNAPI VdfnGetTransformNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    /**
     * the object registry must outlive this node creator.
     * @param pReg non NULL and persistent pointer to the object registry
     */
    VdfnGetTransformNodeCreate(VdfnObjectRegistry *pReg);

    /**
     * arguments
     * - object: [string] the name of an object transform (case-sensitive) to lookup in the
                 object registry
     * - mode: [string] RELATIVE evaluates local transform, WORLD evaluates global transforms
               default is RELATIVE (case-insensitive)
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;

private:
    VdfnObjectRegistry *m_pReg;
};

/**
 * creates an VdfnSetTransformNode, needs a pointer to VdfnObjectRegistry to pass to the node in
 * case the target object is not found during a call to CreateNode()
 */
class VISTADFNAPI VdfnSetTransformNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    /**
     * the object registry must outlive this node creator.
     * @param pReg non NULL and persistent pointer to the object registry
     */
    VdfnSetTransformNodeCreate(VdfnObjectRegistry *pReg);

    /**
     * arguments
     * - object: [string] the name of an object transform (case-sensitive) to lookup in the
                 object registry
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
private:
    VdfnObjectRegistry *m_pReg;
};

/**
 * creates an VdfnApplyTransformNode, needs a pointer to VdfnObjectRegistry to pass to the node in
 * case the target object is not found during a call to CreateNode()
 */
class VISTADFNAPI VdfnApplyTransformNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    /**
     * the object registry must outlive this node creator.
     * @param pReg non NULL and persistent pointer to the object registry
     */
    VdfnApplyTransformNodeCreate(VdfnObjectRegistry *pReg);

    /**
     * arguments
     * - object: [string] the name of an object transform (case-sensitive) to lookup in the
                object registry
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
private:
    VdfnObjectRegistry *m_pReg;
};

/**
 * creates a VdfnHistoryProjectNode.
 */
class VISTADFNAPI VdfnHistoryProjectNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    /**
     * single argument:
     * - project: [list-of-strings] determines the order and names of ports to project from
                  the history to the outport of the nodes.
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

///**
// * creates a VdfnDriverSensorNode
// */
//class VISTADFNAPI VdfnDriverSensorNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
//{
//public:
//
//    /**
//     * the VistaDriverMap must outlive the node creator
//     * @param pMap a pointer to the driver map to query for node creation
//     */
//    VdfnDriverSensorNodeCreate(VistaDriverMap *pMap)
//    : m_pMap(pMap)
//	{
//    }
//
//    /**
//     * arguments
//     * - driver: [string,mandatory] - the name of the driver that contains the sensor of desire
//     * - sensor_index: [number,optional] - the sensor <i>index</i> to query from the driver.
//                 this number may be hard to get, as it is the raw id, OR the sensor mapped id
//                 if the driver supports a sensor mapping.
//     * - sensor_name: [string,optional] - the sensor <i>name</i> to query from the driver.
//                 the sensor must be given after the creation of the sensor using the
//                 VistaDeviceSensor::SetSensorName() API.
//     */
//    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
//
//private:
//    VistaDriverMap *m_pMap;
//};

/**
 * template to create an instance of a VdfnAggregateNode<>.
 */
template<class T>
class VdfnAggregateNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:

    /**
     *  one argument
     *  - in: [string] - determining the property name to aggregate,
              the property must match the type T, but this is checked later (in the node-code)
     *  - order: [string] - either "newest_entry_first" oder "newest_entry_last"
	 *        determines the order in which new, defaults ro newest_entry_last
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const
	{
        try
		{
			assert( oParams.HasSubList("param" ) );
            const VistaPropertyList &oSubs = oParams.GetSubListConstRef("param");
			std::string sProp;
			oSubs.GetValue( "in", sProp );
			bool bStoreNewestEntryLast = true;
			std::string sOrder;
			if( oSubs.GetValue( "order", sOrder ) && sOrder == "newest_entry_first" )
				bStoreNewestEntryLast = false;
            return new TVdfnAggregateNode<T>( sProp, bStoreNewestEntryLast );
        }
		catch (VistaExceptionBase &x)
		{
            x.PrintException();
        }
        return NULL;
    }

};

/**
 * creates a TVdfnChangeDetectNode<>
 */
template<class T>
class TVdfnChangeDetectNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:

    /**
     * no argument.
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const
	{
        return new TVdfnChangeDetectNode<T>;
    }
};

/**
 * template code: creates a TVdfnConstantValueNode<> for type T
 * @todo check error case... memory cleanup?
 */
template<class T>
class VdfnConstantValueNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    typedef bool (*CConvFunc)(const std::string &, T& );

    /**
     * pass a conversion function from string (proplist input) to
     * type T.
     */
    VdfnConstantValueNodeCreate( CConvFunc pfConvert = NULL )
    : m_pfConvert( pfConvert )
	{
		if( m_pfConvert == NULL )
			m_pfConvert = &VistaConversion::FromString<T>;
    }

    /**
     * one argument
     * - value: [type T as string]
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const
	{
        TVdfnConstantValueNode<T> *pNode = new TVdfnConstantValueNode<T>;
        try
		{
            const VistaPropertyList &prams = oParams.GetSubListConstRef("param");
			std::string sValue;
            if( prams.GetValue( "value", sValue ) )
			{
				T oVal;
				if( m_pfConvert( sValue, oVal ) )
				{
					pNode->SetValue( oVal );
				}
				else
				{
					vstr::warnp() << "[VdfnConstantValueNodeCreate]: Default value ["
						<< sValue << "] could not be converted to " 
						<< VistaConversion::GetTypeName<T>() << std::endl;
				}
            }
			else
			{
				vstr::warnp() << "[VdfnConstantValueNodeCreate]: No parameter [value] specified"
							<< std::endl;
			}
        }
		catch (VistaExceptionBase &x)
		{
            x.PrintException();
        }
        return pNode;
    }
private:
    CConvFunc m_pfConvert;
};

/**
 * creates a VdfnLoggerNode.
 */
class VISTADFNAPI VdfnLoggerNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    /**
     * arguments
     * - prefix: [string], mandatory: the prefix of the node file
     * - writeheader: [bool], optional, true forces to write a header as first line,
                      default false
     * - writetime: [bool], optional, true writes a timestamp a first column,
                      default false
     * - writediff: [bool], optional, true writes the timespan since the last update,
                    default false
     * - logtoconsole: [bool], optional, true writes to file and console,
                      default false
     * - sortlist: [list-of-strings], optional, determines the order of columns,
                   not all columns must be listed, those not listed will be appended
                   without order, default: empty
     * - triggerlist: [list-of-strings], optional, the list of portnames that cause
                   a log line to be produced, default: empty
     * - seperator: [string], optional, default is space ' ', "\t" is escaped to tab
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

class VISTADFNAPI VdfnCompositeNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

template<class T>
class TVdfnVectorDecomposeNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:

    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const {
        try
		{
			assert( oParams.HasSubList("param" ) );
            const VistaPropertyList &prams = oParams.GetSubListConstRef("param");

            // defaults to false when parameter not there
            bool bInvertOrder = prams.GetValueOrDefault<bool>("InvertOrder", false);

            return new TVdfnVectorDecomposeNode<T> (bInvertOrder);
        } catch (VistaExceptionBase &x) {
            x.PrintException();
        }
        return NULL;
    }
};

template<class T>
class TVdfnCounterNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:

    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const
	{
        try
		{
			assert( oParams.HasSubList("param" ) );
            const VistaPropertyList &prams = oParams.GetSubListConstRef("param");

            T oInitialValue;
			prams.GetValue( "initial_value", oInitialValue );

            return new TVdfnCounterNode<T>( oInitialValue );
        }
		catch (VistaExceptionBase &x)
		{
            x.PrintException();
        }
        return NULL;
    }
};

template<class T>
class TVdfnModuloCounterNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:

    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const
	{
        try
		{
            const VistaPropertyList &oSubs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

			T oInitialValue = oSubs.GetValueOrDefault<T>( "initial_value" );
			T oModulo = 1;			
			if( oSubs.GetValue( "modulo", oModulo ) == false )
			{
				vstr::warnp() << "[TVdfnModuloCounterNode]: -- Could not create node "
							<< "- no [modulo] specified" << std::endl;
				return NULL;
			}

			return new TVdfnModuloCounterNode<T > (oInitialValue, oModulo);
        }
		catch (VistaExceptionBase &x)
		{
            x.PrintException();
        }
        return NULL;
    }
};


template<class T>
class TVdfnModuloNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:

    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const
	{
        try
		{
            const VistaPropertyList &oSubs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

			T oModulo = 1;			
			if( oSubs.GetValue( "modulo", oModulo ) == false )
			{
				vstr::warnp() << "[TVdfnModuloNode]: -- Could not create node "
							<< "- no [modulo] specified" << std::endl;
				return NULL;
			}

			return new TVdfnModuloNode<T>( oModulo );
        }
		catch (VistaExceptionBase &x)
		{
            x.PrintException();
        }
        return NULL;
    }
};


class VISTADFNAPI VdfnEnvStringValueNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
};


template<class Container, class Type>
class TVdfnGetElementNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator 
{
public:
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const 
	{
        try 
		{
            const VistaPropertyList &oSubs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

			int nIndex = oSubs.GetValueOrDefault<int>( "index", -1 );
            return new TVdfnGetElementNode<Container, Type>( nIndex );
        } 
		catch (VistaExceptionBase &x)
		{
            x.PrintException();
        }
        return NULL;
    }
};

template<class Container, class Type, int Size>
class TVdfnGetElementFromArrayNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator 
{
public:
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const 
	{
        try 
		{
            const VistaPropertyList &oSubs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

			int nIndex = oSubs.GetValueOrDefault<int>( "index", -1 );
            return new TVdfnGetElementFromArrayNode<Container, Type, Size>( nIndex );
        } 
		catch (VistaExceptionBase &x)
		{
            x.PrintException();
        }
        return NULL;
    }
};

template<class T>
class TVdfnRangeCheckNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator 
{
public:
    virtual IVdfnNode *CreateNode( const VistaPropertyList& oParams ) const 
	{
        try 
		{
            const VistaPropertyList &oSubs = oParams.GetPropertyConstRef( "param" ).GetPropertyListConstRef();

			T nMin = oSubs.GetValueOrDefault<T>( "min", -1 );
			T nMax = oSubs.GetValueOrDefault<T>( "max", -1 );
            return new TVdfnRangeCheckNode<T>( nMin, nMax );
        } 
		catch (VistaExceptionBase &x)
		{
            x.PrintException();
        }
        return NULL;
    }
};

template<class T>
class TVdfnDemultiplexNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator 
{
public:
    virtual IVdfnNode *CreateNode( const VistaPropertyList& oParams ) const 
	{
        try 
		{
            const VistaPropertyList &oSubs = oParams.GetPropertyConstRef( "param" ).GetPropertyListConstRef();

			int nNumPorts;
			if( oSubs.GetValue<int>( "num_outports", nNumPorts ) == false )
			{
				vstr::warnp() << "[TVdfnDemultiplexNodeCreate]: no or invalid \"num_outports\" specified" << std::endl;
				return NULL;
			}
            return new TVdfnDemultiplexNode<T>( nNumPorts );
        } 
		catch (VistaExceptionBase &x)
		{
            x.PrintException();
        }
        return NULL;
    }
};

class VISTADFNAPI VdfnToggleNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    virtual IVdfnNode *CreateNode( const VistaPropertyList& oParams ) const ;
};


template<typename T>
class TVdfnVariableNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator 
{	
	// @TODO: registry is somewhat hacky - proper registry management
public:
	enum NodeType
	{
		VN_GETTER,
		VN_SETTER
	};

	TVdfnVariableNodeCreate( NodeType eType )
	: m_eType( eType )
	{
	}

	~TVdfnVariableNodeCreate()
	{
	}

    virtual IVdfnNode *CreateNode( const VistaPropertyList& oParams ) const 
	{
        try 
		{
            const VistaPropertyList &oSubs = oParams.GetPropertyConstRef( "param" ).GetPropertyListConstRef();

			std::string sVariableName;
			if( oSubs.GetValue<std::string>( "variable", sVariableName ) == false )
			{
				vstr::warnp() << "[TVdfnVariableNodeCreate]: no parameter \"variable\" specified" << std::endl;
				return NULL;
			}

			typename TVdfnGetVariableNode<T>::Variable* pVar;
			T oInitial = VistaConversion::CreateInitializedDefaultObject<T>();
			if( oSubs.GetValue<T>( "initial_value", oInitial ) )
				pVar = m_pRegistry->GetOrCreateVariable( sVariableName, oInitial );
			else
				pVar = m_pRegistry->GetOrCreateVariable( sVariableName );
				
			switch( m_eType )
			{
				// @TODO: delay creation of getter node to check if var exists...
				case VN_GETTER:
					return new TVdfnGetVariableNode<T>( pVar );
				case VN_SETTER:
					return new TVdfnSetVariableNode<T>( pVar );
				default:
					VISTA_THROW( "TVdfnVariableNodeCreate - invalid mode", -1 );
			}
        } 
		catch( VistaExceptionBase& e )
		{
            e.PrintException();
        }
        return NULL;
    }

private:
	NodeType m_eType;
	static TVdfnTypedVariableRegistry<T>*	m_pRegistry;
};

template<typename T>
TVdfnTypedVariableRegistry<T>* TVdfnVariableNodeCreate<T>::m_pRegistry = new TVdfnTypedVariableRegistry<T>;

class VISTADFNAPI VdfnOutstreamNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
    virtual IVdfnNode *CreateNode( const VistaPropertyList& oParams ) const ;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNNODECREATORS_H

