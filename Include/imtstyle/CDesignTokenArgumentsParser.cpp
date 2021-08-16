#include <imtstyle/CDesignTokenArgumentsParser.h>



namespace imtstyle
{


bool CDesignTokenArgumentsParser::CheckArgument(AvailableArguments argument) const
{
	bool retval = false;

	QByteArrayList args = m_commands.values(argument);

	for(const QByteArray &arg : ::qAsConst(args)){
		if(m_arguments.contains(arg)){
			retval = true;
			break;
		}
	}

	return retval;
}


QByteArray CDesignTokenArgumentsParser::GetArgumentKey(AvailableArguments argument) const
{
	QByteArray retval;

	QByteArrayList args = m_commands.values(argument);
	QByteArray argKey;

	for(const QByteArray &arg : ::qAsConst(args)){
		if(m_arguments.contains(arg)){
			argKey = arg;
			break;
		}
	}

	int indexOfArg = m_arguments.indexOf(argKey);

	if(m_arguments.size() < indexOfArg){
		retval = m_arguments[indexOfArg];
	}

	return retval;
}


QByteArray CDesignTokenArgumentsParser::GetArgumentValue(AvailableArguments argument) const
{
	QByteArray retval;
	QByteArray AC_key = this->GetArgumentKey(argument);
	int indexOfArg = m_arguments.indexOf(AC_key);

	if(m_arguments.size() < indexOfArg){
		retval = m_arguments[++indexOfArg];
	}

	return retval;
}


QByteArrayList CDesignTokenArgumentsParser::GetArgumentValueMulti(AvailableArguments argument) const
{
	QByteArrayList retval;
	QByteArrayList args = m_commands.values(argument);
	QByteArray argValue;

	for(const QByteArray &arg : ::qAsConst(args)){
		if(m_arguments.contains(arg)){
			argValue = arg;
			break;
		}
	}

	int indexOfArg = m_arguments.indexOf(argValue);

	while(m_arguments.size() < indexOfArg + 1){
		++indexOfArg;
		if(m_arguments[indexOfArg].startsWith('-') || m_arguments[indexOfArg].startsWith('/')){
			break;
		}

		QByteArray arg = m_arguments[indexOfArg];
		retval << arg;
	}
	return retval;
}


// reimplemented (IDesignTokenArgumentsParser)

void CDesignTokenArgumentsParser::SetArguments(int argc, char** argv)
{
	//=====================================================================================Commands init
	m_commands.insert(AA_HELP, "--help");
	m_commands.insert(AA_HELP, "-h");
	m_commands.insert(AA_HELP, "/?");

	//-----------------------------------------------colors
	m_commands.insert(AA_TEMPLATE_ICON_COLOR, "--template-icon-color");
	m_commands.insert(AA_TEMPLATE_ICON_COLOR, "-TIC");
	m_commands.insert(AA_TEMPLATE_PRIMARY_COLOR, "--template-primary-color");
	m_commands.insert(AA_TEMPLATE_PRIMARY_COLOR, "-TPC");
	m_commands.insert(AA_TEMPLATE_SECONDARY_COLOR, "--template-secondary-color");
	m_commands.insert(AA_TEMPLATE_SECONDARY_COLOR, "-TSC");

	//----------------------------------------------- files & dirs
	m_commands.insert(AA_DESIGN_TOKEN_FILE_PATH, "--file");
	m_commands.insert(AA_DESIGN_TOKEN_FILE_PATH, "-F");

	//-----------------------------------------------

	//=====================================================================================
}


bool CDesignTokenArgumentsParser::IsHelpRequested() const
{
	return this->CheckArgument(AA_HELP);
}


QByteArray CDesignTokenArgumentsParser::GetDesignTokenFilePath() const
{
	return this->GetArgumentValue(AA_DESIGN_TOKEN_FILE_PATH);
}


QByteArray CDesignTokenArgumentsParser::GetTemplateIconColor() const
{
	return this->GetArgumentValue(AA_TEMPLATE_ICON_COLOR);
}


QByteArray CDesignTokenArgumentsParser::GetTemplatePrimaryColor() const
{
	return this->GetArgumentValue(AA_TEMPLATE_PRIMARY_COLOR);
}


QByteArray CDesignTokenArgumentsParser::GetTemplateSecondaryColor() const
{
	return this->GetArgumentValue(AA_TEMPLATE_SECONDARY_COLOR);
}



} // namespace imtstyle





