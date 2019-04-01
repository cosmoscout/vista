#include "PyDfnNode.h"
#include <boost/python.hpp>
#include <cassert>
#include <cstdlib>
#include <Python.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* PyDfnNode                                                        */
/*============================================================================*/

namespace py = boost::python;


PyDfnNode::PyDfnNode(const std::string &path) : IVdfnNode()
{
	// std::cout << "creating PyDfnNode, path = " << path << std::endl;

	FILE *fd = fopen(path.c_str(), "r");

	assert(fd);

	if (!fd) {
		std::cerr << "PyDfnNode::PyDfnNode(): Unable to open script file " << path << std::endl;
		exit(-1);
	}

	try {
		py::object main_namespace = makeNamespace();
		py::handle<> ignored(PyRun_FileEx(fd, path.c_str(), Py_file_input, main_namespace.ptr(), main_namespace.ptr(), 1)); // closes fd after executing the script
	}
	catch (const py::error_already_set&) 
	{
		PyErr_Print();
		exit(-1);
	}

	/**
	 * We just create and register a single outport
	 */
	//	RegisterOutPort( "out", m_pOut );

	// evaluate on every graph traversal instead of only on changed input ports.
	SetEvaluationFlag( true );
}

PyDfnNode::~PyDfnNode()
{
}

py::object PyDfnNode::makeNamespace()
{
	py::object main_module((py::handle<>(py::borrowed(PyImport_AddModule("__main__")))));
	py::object main_namespace = main_module.attr("__dict__");

	injectClassDefinitionAndInstance(main_namespace);

	return main_namespace;
}

void PyDfnNode::injectClassDefinitionAndInstance(py::object main_namespace)
{
	try
	{
		main_namespace["DfnNode"] = py::class_<PyDfnNode>("DfnNode", py::no_init)
						.def("registerEvalCallback", &PyDfnNode::pyRegisterEvalCallback)
						.def("registerOutPortFloat", &PyDfnNode::pyRegisterOutPortFloat)
						.def("setOutPortValue", &PyDfnNode::pySetOutPortValue)
						.def("registerInPortFloat", &PyDfnNode::pyRegisterInPortFloat)
						.def("registerInPortInt", &PyDfnNode::pyRegisterInPortInt)
						.def("getInPortValue", &PyDfnNode::pyGetInPortValue);
		main_namespace["dfnNode"] = py::ptr(this);
	}
	catch (const py::error_already_set &)
	{
		PyErr_Print();
		exit(-1);
	}
}

/**
 * Different to the ColorCHangerDfnNode, we don't need to override GetIsValid()
 * and PrepareEvaluationRun() here, we just have one outport and are always
 * ready for work!
 * We do, however, overwrite the GetIsMasterSim routine to return true instead
 * of the usually false. This specifies that the node should be evaluated on the
 * master, and the values of the outport(s) are then transfered over network
 * to the clients, ensuring a deterministic synchronized state.
 */
bool PyDfnNode::GetIsMasterSim() const
{
	return false;
}

bool PyDfnNode::PrepareEvaluationRun()
{	
	// std::cout << "prep = " << GetInPort("in") << std::endl;
	return true;
}


/**
 * During Evaluation, we simply set the outport value to a random number
 * from the specified interval.
 */
bool PyDfnNode::DoEvalNode()
{
	// FIXME: switch thread state here
	try 
	{
		PyGILState_STATE gstate = PyGILState_Ensure();

		if (_evalCallback.is_none()) {
			std::cerr << "PyDfnNode::DoEvalNode(): Evaluation callback not registered!" << std::endl;
			exit(-1);
		} else {
			_evalCallback();
		}

		PyGILState_Release(gstate);
	}
	catch (const py::error_already_set &)
	{
		PyErr_Print();
		exit(-1);
	}

	return true;
	
	//float fValue = m_fMin + m_fRange * (float)m_pRand->GenerateDouble1();
	//m_pOut->SetValue( fValue, 
}

void PyDfnNode::pyRegisterEvalCallback(py::object evalCallback) {
	_evalCallback = evalCallback;
}

void PyDfnNode::pyRegisterOutPortFloat(const std::string &name) {
	assert(_outPorts.find(name) == _outPorts.end());

	TVdfnPort<float> *port = new TVdfnPort<float>;
	_outPorts.insert(std::make_pair(name, port));
	RegisterOutPort(name.c_str(), port);
}

void PyDfnNode::pyRegisterInPortFloat(const std::string &name) {
	registerInPort(name, new TVdfnPortTypeCompare<TVdfnPort<float> >);
}

void PyDfnNode::pyRegisterInPortInt(const std::string &name) {
	registerInPort(name, new TVdfnPortTypeCompare<TVdfnPort<int> >);
}

void PyDfnNode::registerInPort(const std::string &name, IVdfnPortTypeCompare *port) {
	assert(_inPorts.find(name) == _inPorts.end());
	RegisterInPortPrototype(name.c_str(), port);
	_inPorts.insert(std::make_pair(name, port));
}

void PyDfnNode::pySetOutPortValue(const std::string &portName, boost::python::object obj) {
	// find port by name
	std::map<std::string, IVdfnPort*>::iterator it = _outPorts.find(portName);
	
	if (it == _outPorts.end()) {
		std::cerr << "PyDfnNode::pySetOutPortValue(): Port name \"" << portName << "\" not registered!" << std::endl;
		exit(-1);
	}
	
	// check type of argument
	py::extract<float> get_float(obj);

	if (get_float.check()) {
		TVdfnPort<float> *floatPort = dynamic_cast<TVdfnPort<float> *>(it->second);

		if (!floatPort) {
			std::cerr << "PyDfnNode::pySetOutPortValue(): Type mismatch in setting value for port " << portName << std::endl;
			exit(-1);
		}
		
		floatPort->SetValue(get_float(), GetUpdateTimeStamp());

		return;
	}

	std::cerr << "PyDfnNode::pySetOutPortValue(): Unsupported python value type for port " << portName << std::endl;
	exit(-1);
}

py::object PyDfnNode::pyGetInPortValue(const std::string &portName) {
	// find port by name
	std::map<std::string, IVdfnPortTypeCompare*>::iterator it = _inPorts.find(portName);
	
	if (it == _inPorts.end()) {
		std::cerr << "PyDfnNode::pyGetInPortValue(): Port name \"" << portName << "\" not registered!" << std::endl;
		exit(-1);
	}

	IVdfnPort *port = GetInPort(portName.c_str());

	if (!port) {
		std::cerr << "PyDfnNode::pyGetInPortValue(): Port " << portName << " not found!" << std::endl;
		exit(-1);
	}

	TVdfnPort<float> *floatPort = dynamic_cast<TVdfnPort<float>*>(port);
	if (floatPort)
		return py::object(floatPort->GetValueConstRef());

	TVdfnPort<int> *intPort = dynamic_cast<TVdfnPort<int>*>(port);
	if (intPort)
		return py::object(intPort->GetValueConstRef());

	std::cerr << "PyDfnNode::pyGetInPortValue(): Unsupported port type for port " << portName << std::endl;
	exit(-1);
}


