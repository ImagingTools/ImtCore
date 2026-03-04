// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


