#include <imtpy/CPythonOutputRedirector.h>


namespace imtpy
{


// public methods

CPythonOutputRedirector::CPythonOutputRedirector()
{
	auto sysModule = pybind11::module::import("sys");
	m_stdout = sysModule.attr("stdout");
	m_stderr = sysModule.attr("stderr");

	auto stringIoModule = pybind11::module::import("io").attr("StringIO");
	m_stdoutBuffer = stringIoModule();
	m_stderrBuffer = stringIoModule();
	sysModule.attr("stdout") = m_stdoutBuffer;
	sysModule.attr("stderr") = m_stderrBuffer;
}


CPythonOutputRedirector::~CPythonOutputRedirector()
{
	auto sysModule = pybind11::module::import("sys");
	sysModule.attr("stdout") = m_stdout;
	sysModule.attr("stderr") = m_stderr;
}


std::string CPythonOutputRedirector::GetStdOutput() const
{
	m_stdoutBuffer.attr("seek")(0);

	return pybind11::str(m_stdoutBuffer.attr("read")());
}


std::string CPythonOutputRedirector::GetErrorOutput() const
{
	m_stderrBuffer.attr("seek")(0);

	return pybind11::str(m_stderrBuffer.attr("read")());
}


} // namespace imtpy


