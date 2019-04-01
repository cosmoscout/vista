#include "PyDfnNodeCreate.h"
#include "PyDfnNode.h"

#include <cstdlib>

PyDfnNodeCreate::PyDfnNodeCreate()
: VdfnNodeFactory::IVdfnNodeCreator()
{
}

PyDfnNodeCreate::~PyDfnNodeCreate()
{
}

/**
 * The CreateNode routine should return a valid instance of the node, or
 * NULL if something went wrong (e.g. if mandatory parameters were not found).
 */
IVdfnNode* PyDfnNodeCreate::CreateNode( const VistaPropertyList& oParams ) const
{
	const VistaPropertyList& oSubParams = oParams.GetPropertyConstRef ("param" ).GetPropertyListConstRef();
	
	if (!oSubParams.HasProperty("path")) {
		std::cerr << "PyDfnNodeCreate::CreateNode(): xml-param 'path' missing!" << std::endl;
		exit(-1);
	}
	
	std::string path(oSubParams.GetValue<std::string>("path"));

	return new PyDfnNode(path);
}


