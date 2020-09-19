#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ifile/IFileNameParam.h>


namespace pybind11
{
	class scoped_interpreter;
}


namespace imtpy
{


class CPythonEnvironmentComp: public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPythonEnvironmentComp);
		I_ASSIGN(m_pythonInstallDirectoryCompPtr, "PythonInstallDirectory", "Directory where the python interpreter was installed", true, "PythonInstallDirectory");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private:
	static istd::TDelPtr<pybind11::scoped_interpreter> s_pythonInterpreter;

	I_REF(ifile::IFileNameParam, m_pythonInstallDirectoryCompPtr);
};


} // namespace imtpy


