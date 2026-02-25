// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtddl/CDdlCodeCreatorArgumentsParserComp.h>


namespace imtddl
{


bool CDdlCodeCreatorArgumentsParserComp::CheckArgument(AvailableArguments argument) const
{
	bool retval = false;

	QByteArrayList args = m_commands.values(argument);

	for(const QByteArray& arg: std::as_const(args)){
		if(m_arguments.contains(arg)){
			retval = true;
			break;
		}
	}

	return retval;
}


QByteArray CDdlCodeCreatorArgumentsParserComp::GetArgumentKey(AvailableArguments argument) const
{
	QByteArray retval;

	QByteArrayList args = m_commands.values(argument);
	QByteArray argKey;

	for(const QByteArray& arg: std::as_const(args)){
		if(m_arguments.contains(arg)){
			argKey = arg;
			break;
		}
	}

	qsizetype indexOfArg = m_arguments.indexOf(argKey);

	if(indexOfArg > -1 && m_arguments.size() > indexOfArg){
		retval = m_arguments[indexOfArg];
	}

	return retval;
}


QByteArray CDdlCodeCreatorArgumentsParserComp::GetArgumentValue(AvailableArguments argument) const
{
	QByteArray retval;
	QByteArray AC_key = this->GetArgumentKey(argument);
	qsizetype indexOfArg = m_arguments.indexOf(AC_key);

	if(indexOfArg > -1 && m_arguments.size() > ++indexOfArg){
		if(!m_arguments[indexOfArg].startsWith('-')){
			retval = m_arguments[indexOfArg];
		}
	}

	return retval;
}



// reimplemented (IDdlCodeCreatorArgumentsParser)
void CDdlCodeCreatorArgumentsParserComp::SetArguments(int argc, char** argv)
{
	for(int i = 0; i < argc; ++i){
		m_arguments << argv[i];
	}

	m_commands.insert(AA_HELP, "--help");
	m_commands.insert(AA_HELP, "-h");
	m_commands.insert(AA_HELP, "/?");
	m_commands.insert(AA_TEMPLATE_DIR_PATH, "--template-dir");
	m_commands.insert(AA_TEMPLATE_DIR_PATH, "-TD");
	m_commands.insert(AA_OUTPUT_CPP_DIR_PATH, "--output-cpp-dir");
	m_commands.insert(AA_OUTPUT_CPP_DIR_PATH, "-CD");
	m_commands.insert(AA_OUTPUT_QML_DIR_PATH, "--output-qml-dir");
	m_commands.insert(AA_OUTPUT_QML_DIR_PATH, "-QD");
	m_commands.insert(AA_MODULE_CPP_NAME, "--module-cpp-name");
	m_commands.insert(AA_MODULE_CPP_NAME, "-MODC");
	m_commands.insert(AA_MODULE_QML_NAME, "--module-qml-name");
	m_commands.insert(AA_MODULE_QML_NAME, "-MODQ");
}


QByteArray CDdlCodeCreatorArgumentsParserComp::GetTemplateDirPath() const
{
	return this->GetArgumentValue(AA_TEMPLATE_DIR_PATH);
}


QByteArray CDdlCodeCreatorArgumentsParserComp::GetCppDirPath() const
{
	return this->GetArgumentValue(AA_OUTPUT_CPP_DIR_PATH);
}


QByteArray CDdlCodeCreatorArgumentsParserComp::GetQmlDirPath() const
{
	return this->GetArgumentValue(AA_OUTPUT_QML_DIR_PATH);
}


QString CDdlCodeCreatorArgumentsParserComp::GetModuleCppName() const
{
	return this->GetArgumentValue(AA_MODULE_CPP_NAME);
}


QString CDdlCodeCreatorArgumentsParserComp::GetModuleQmlName() const
{
	return this->GetArgumentValue(AA_MODULE_QML_NAME);
}


} // namespace imtddl


