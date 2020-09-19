// PyBind includes
#undef slots
#include <pybind11/embed.h>


namespace imtpy
{


class CPythonOutputRedirector
{
public:
	CPythonOutputRedirector();
	~CPythonOutputRedirector();

	std::string GetStdOutput() const;
	std::string GetErrorOutput() const;

private:
	pybind11::object m_stdout;
	pybind11::object m_stderr;
	pybind11::object m_stdoutBuffer;
	pybind11::object m_stderrBuffer;
};


} // namespace imtpy


