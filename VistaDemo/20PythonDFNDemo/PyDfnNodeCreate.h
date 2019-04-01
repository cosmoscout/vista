#ifndef PYDFNNODECREATE_H
#define PYDFNNODECREATE_H

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>

#include <VistaAspects/VistaPropertyAwareable.h>

class PyDfnNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	PyDfnNodeCreate();
	virtual ~PyDfnNodeCreate();

	virtual IVdfnNode* CreateNode( const VistaPropertyList& oPropertyList ) const;
};

#endif
