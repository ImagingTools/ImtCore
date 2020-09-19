#include <imtpy/CPythonEnvironmentComp.h>


// Qt includes
#include <QtCore/QCoreApplication>

// PyBind includes
#undef slots
#include <pybind11/embed.h>


namespace imtpy
{


istd::TDelPtr<pybind11::scoped_interpreter> CPythonEnvironmentComp::s_pythonInterpreter;


// reimplemented (icomp::CComponentBase)

void CPythonEnvironmentComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_pythonInstallDirectoryCompPtr.IsValid()){
		return;
	}

	QString installDirectory = m_pythonInstallDirectoryCompPtr->GetPath();

	std::wstring pythonHome =installDirectory.toStdWString();
	std::wstring pythonPaths = pythonHome + L";" + pythonHome + L"\\Lib;" + pythonHome + L"\\libs;" + pythonHome + L"\\DLLs;" + pythonHome + L"\\Scripts;";

	// Initialize python
	Py_OptimizeFlag = 1;
	Py_SetPath(pythonPaths.c_str());
	Py_SetPythonHome(pythonHome.c_str());

	//Initialized the PythonInterpreter once globally
	if (!s_pythonInterpreter.IsValid()){
		pybind11::scoped_interpreter* interpreterPtr = new pybind11::scoped_interpreter;

		Py_SetProgramName(L"ImtPyApplication");

		// Disable build of __pycache__ folders
		pybind11::exec(R"(
			import
			sys sys.dont_write_bytecode = True)");

		s_pythonInterpreter.SetPtr(interpreterPtr);
	}
}


} // namespace imtpy


