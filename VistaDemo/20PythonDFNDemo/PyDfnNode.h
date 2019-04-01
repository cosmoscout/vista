#ifndef PYDFNNODE_H
#define PYDFNNODE_H

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>

#include <VistaAspects/VistaPropertyAwareable.h>
#include <boost/python.hpp>
#include <map>

class PyDfnNode : public IVdfnNode
{
public:
	PyDfnNode( const std::string &path );
	~PyDfnNode();

	bool PrepareEvaluationRun();

	// the following public methods are exported to python

	void pyRegisterEvalCallback(boost::python::object evalCallback); // register a python function to be called on graph evaluation

	void pyRegisterInPortFloat(const std::string &name);
	void pyRegisterInPortInt(const std::string &name);
	boost::python::object pyGetInPortValue(const std::string &portName);

	void pyRegisterOutPortFloat(const std::string &name);
	void pySetOutPortValue(const std::string &portName, boost::python::object obj);


protected:
	virtual bool DoEvalNode();
	virtual bool GetIsMasterSim() const;

private:
	boost::python::object makeNamespace();
	void injectClassDefinitionAndInstance(boost::python::object main_namespace);
	void registerInPort(const std::string &name, IVdfnPortTypeCompare *port);

	boost::python::object _evalCallback; // python callback function to be called on graph evaluation (DoEvalNode())
	std::map<std::string, IVdfnPort*> _outPorts;
	std::map<std::string, IVdfnPortTypeCompare*> _inPorts;
};

#endif
