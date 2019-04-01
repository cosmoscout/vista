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


#ifndef _VDFNNODEFACTORY_H
#define _VDFNNODEFACTORY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include <string>
#include <map>
#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaPropertyList;
class IVdfnNode;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * the node factory is a central part of the DataFlowNet reflection layer.
 * There can only be one: get a hold at it using GetSingleton(), the factory
 * is created on first access, and there is currently no way to purge it.
 * It manages node creators, which can be set an unset, but memory management
 * is up to the user. The pointers provided have to outlive the factory, or
 * at least have to live as long as they are needed and not unregistered.
 * IVdfnNodeCreators try to construct a node given a VistaPropertyList, and usually
 * expect their parameters to live in a sub-tree called 'params'.
 * Nodes can be created by calling CreateNode(), given a type symbol and
 * the creation parameters.
 * All type symbols are evaluated as case-sensitive
 */
class VISTADFNAPI VdfnNodeFactory
{
public:

	/**
	 * @return retrieve the pointer to the node factory.
	 */
	static VdfnNodeFactory *GetSingleton();


	/**
	 * basic interface for node creators. subclass and define the CreateNode()
	 * method
	 */
	class IVdfnNodeCreator
	{
	public:
		virtual ~IVdfnNodeCreator() {}
		/**
		 * create a node given a VistaPropertyList
		 * @param oParams a VistaPropertyList that must match the need of a specific
		           node creator. This is rather fuzzy, so dig up the code here,
		           or read the node creators' documentation.
		   @return NULL in case a node could not be created, a node else
		 */
		virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const = 0;
	};

	/**
	 * create a node using a type symbol. the type symbol is determined by the
	 * mapping of a string to a IVdfnNodeCreator. The logic is simple:
	 * -# lookup whether a node creator was mapped to strTypeSymbol
	 * -# call IVdfnNodeCreator::CreateNode() with oParams
	 *
	 * @param strTypeSymbol the node type to create
	 * @param oParams parameters for construction
	 * @see SetNodeCreator()
	 */
	IVdfnNode *CreateNode( const std::string &strTypeSymbol,
						   const VistaPropertyList &oParams ) const;

	/**
	 * creates a mapping from strTypeSymbol to pCreator.
	 * Old assignments will be silently overwritten
	 * @param strTypeSymbol the type to register (case-sensitive)
	 * @param pCreate to creator for the type (must outlive the factory)
	 * @return true
	 *
	 * @see UnSetNodeCreator()
	 */
	bool SetNodeCreator( const std::string &strTypeSymbol,
						 IVdfnNodeCreator *pCreator );

	/**
	 * clear a binding from strTypeSymbol to a node creator.
	 * Can be used for unregistering node creators. Frees the memory
	 * on the node creator.
	 * @return true when the binding was found and dissolved
	 * @param strTypeSymbol the type symbol to unbind
	 * @see SetNodeCreator()
	 */
	bool UnSetNodeCreator( const std::string &strTypeSymbol );

	/**
	 * @return retrieve a list of available type symbols to create nodes for.
	 */
	std::list<std::string> GetNodeCreators() const;


	/**
	 * @return true when strCreatorName maps to an IVdfnNodeCreator
	 */
	bool GetHasCreator( const std::string &strCreatorName ) const;
protected:
private:
	VdfnNodeFactory();

	/**
	 * deletes all registered node creators.
	 */
	~VdfnNodeFactory();

	typedef std::map<std::string, IVdfnNodeCreator* > CRMAP;
	CRMAP m_mpCreators;
};

/**
 * default template for simple node creation, simply create a type T node,
 * without arguments and without inspecting the oParams
 */
template<class T>
class TVdfnDefaultNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	/**
	 * @return a new node of type T
	 */
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const
	{
		return new T;
	}
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNNODEFACTORY_H

