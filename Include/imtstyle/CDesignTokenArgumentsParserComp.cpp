#include <imtstyle/CDesignTokenArgumentsParserComp.h>



namespace imtstyle
{


bool CDesignTokenArgumentsParserComp::CheckArgument(AvailableArguments argument) const
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


QByteArray CDesignTokenArgumentsParserComp::GetArgumentKey(AvailableArguments argument) const
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

	if(m_arguments.size() > indexOfArg){
		retval = m_arguments[indexOfArg];
	}

	return retval;
}


QByteArray CDesignTokenArgumentsParserComp::GetArgumentValue(AvailableArguments argument) const
{
	QByteArray retval;
	QByteArray AC_key = this->GetArgumentKey(argument);
	int indexOfArg = m_arguments.indexOf(AC_key);

	if(m_arguments.size() > ++indexOfArg){
		retval = m_arguments[indexOfArg];
	}

	return retval;
}


QByteArrayList CDesignTokenArgumentsParserComp::GetArgumentValueMulti(AvailableArguments argument) const
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

	while(m_arguments.size() > indexOfArg + 1){
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

void CDesignTokenArgumentsParserComp::SetArguments(int argc, char** argv)
{
	for(int i = 0; i < argc; ++i){
		m_arguments << argv[i];
	}

	//=====================================================================================Commands init
	m_commands.insert(AA_HELP, "--help");
	m_commands.insert(AA_HELP, "-h");
	m_commands.insert(AA_HELP, "/?");

	//----------------------------------------------- files & dirs
	m_commands.insert(AA_DESIGN_TOKEN_FILE_PATH, "--file");
	m_commands.insert(AA_DESIGN_TOKEN_FILE_PATH, "-F");

	m_commands.insert(AA_INPUT_DIRECTORY_PATH, "--input-dir");
	m_commands.insert(AA_INPUT_DIRECTORY_PATH, "-ID");

	m_commands.insert(AA_OUTPUT_DIRECTORY_PATH, "--output-dir");
	m_commands.insert(AA_OUTPUT_DIRECTORY_PATH, "-OD");

	//-----------------------------------------------control options
	m_commands.insert(AA_IMAGE_MODE, "-I");
	m_commands.insert(AA_IMAGE_MODE, "--image");

	m_commands.insert(AA_STYLE_SHEET_MODE, "-S");
	m_commands.insert(AA_STYLE_SHEET_MODE, "--style-sheet");

	//-----------------------------------------------

	//=====================================================================================
}


bool CDesignTokenArgumentsParserComp::IsHelpRequested() const
{
	return this->CheckArgument(AA_HELP);
}


bool imtstyle::CDesignTokenArgumentsParserComp::IsStyleSheetModeRequired() const
{
	return this->CheckArgument(AA_STYLE_SHEET_MODE);
}


bool imtstyle::CDesignTokenArgumentsParserComp::IsImageModeRequired() const
{
	return this->CheckArgument(AA_IMAGE_MODE);
}


QByteArray CDesignTokenArgumentsParserComp::GetDesignTokenFilePath() const
{
	return this->GetArgumentValue(AA_DESIGN_TOKEN_FILE_PATH);
}


QByteArray imtstyle::CDesignTokenArgumentsParserComp::GetInputDirectoryPath() const
{
	return this->GetArgumentValue(AA_INPUT_DIRECTORY_PATH);
}


QByteArray imtstyle::CDesignTokenArgumentsParserComp::GetOutputDirectoryPath() const
{
	return this->GetArgumentValue(AA_OUTPUT_DIRECTORY_PATH);
}


} // namespace imtstyle








