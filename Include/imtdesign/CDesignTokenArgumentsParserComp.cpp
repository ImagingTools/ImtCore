#include <imtdesign/CDesignTokenArgumentsParserComp.h>


namespace imtdesign
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

	if(indexOfArg > -1 && m_arguments.size() > indexOfArg){
		retval = m_arguments[indexOfArg];
	}

	return retval;
}


QByteArray CDesignTokenArgumentsParserComp::GetArgumentValue(AvailableArguments argument) const
{
	QByteArray retval;
	QByteArray AC_key = this->GetArgumentKey(argument);
	int indexOfArg = m_arguments.indexOf(AC_key);

	if(indexOfArg > -1 && m_arguments.size() > ++indexOfArg){
		if(!m_arguments[indexOfArg].startsWith('-')){
			retval = m_arguments[indexOfArg];
		}
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

	// qDebug() << m_arguments;

	while(indexOfArg > -1 && m_arguments.size() > indexOfArg + 1){
		++indexOfArg;
		if(m_arguments[indexOfArg].startsWith('-') || m_arguments[indexOfArg].startsWith("/?")){
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

	m_commands.insert(AA_HELP, "--help");
	m_commands.insert(AA_HELP, "-h");
	m_commands.insert(AA_HELP, "/?");
	m_commands.insert(AA_DESIGN_TOKEN_FILE_PATH, "--file");
	m_commands.insert(AA_DESIGN_TOKEN_FILE_PATH, "-F");
	m_commands.insert(AA_OUTPUT_DIRECTORY_PATH, "--output-dir");
	m_commands.insert(AA_OUTPUT_DIRECTORY_PATH, "-OD");
	m_commands.insert(AA_OUTPUT_FILE_PATH, "--output-file");
	m_commands.insert(AA_OUTPUT_FILE_PATH, "-OF");
	m_commands.insert(AA_IMAGES_INPUT_DIR_PATH, "-I");
	m_commands.insert(AA_IMAGES_INPUT_DIR_PATH, "--images-input-dir");
	m_commands.insert(AA_STYLE_SHEETS_INPUT_DIR_PATH, "-S");
	m_commands.insert(AA_STYLE_SHEETS_INPUT_DIR_PATH, "--style-sheet-input-dir");
	m_commands.insert(AA_PROJECT_NAME, "-P");
	m_commands.insert(AA_SPLIT_FILE_MODE, "--split");
	m_commands.insert(AA_COPY_DESIGN_TOKEN_FILE, "-C");
	m_commands.insert(AA_COPY_DESIGN_TOKEN_FILE, "--copy");
	m_commands.insert(AA_ERRORS_IGNORE_MODE, "--ignore-errors");
	m_commands.insert(AA_WEB, "-W");
	m_commands.insert(AA_QML, "--qml");
	m_commands.insert(AA_COMMON, "--common");
}


bool CDesignTokenArgumentsParserComp::IsHelpRequested() const
{
	return this->CheckArgument(AA_HELP);
}


bool CDesignTokenArgumentsParserComp::IsErrorsIgnoreRequested() const
{
	return this->CheckArgument(AA_ERRORS_IGNORE_MODE);
}


bool CDesignTokenArgumentsParserComp::IsSplitModeRequired() const
{
	return this->CheckArgument(AA_SPLIT_FILE_MODE);
}


bool CDesignTokenArgumentsParserComp::IsCopyDesignTokenFileRequired() const
{
	return this->CheckArgument(AA_COPY_DESIGN_TOKEN_FILE);
}


bool CDesignTokenArgumentsParserComp::IsWebGenerateResourceRequired() const
{
	return this->CheckArgument(AA_WEB);
}


bool CDesignTokenArgumentsParserComp::IsCommonGenerateResourceRequired() const
{
	return this->CheckArgument(AA_COMMON);
}


bool CDesignTokenArgumentsParserComp::IsQmlGenerateRequired() const
{
	return this->CheckArgument(AA_QML);
}


QByteArray CDesignTokenArgumentsParserComp::GetImagesInputDirectoryPath() const
{
	return this->GetArgumentValue(AA_IMAGES_INPUT_DIR_PATH);
}


QByteArrayList CDesignTokenArgumentsParserComp::GetImagesInputDirectoryMultiPath() const
{
	return this->GetArgumentValueMulti(AA_IMAGES_INPUT_DIR_PATH);
}


QByteArray CDesignTokenArgumentsParserComp::GetStyleSheetsInputDirectoryPath() const
{
	return this->GetArgumentValue(AA_STYLE_SHEETS_INPUT_DIR_PATH);
}


QByteArray CDesignTokenArgumentsParserComp::GetDesignTokenFilePath() const
{
	return this->GetArgumentValue(AA_DESIGN_TOKEN_FILE_PATH);
}


QByteArrayList CDesignTokenArgumentsParserComp::GetDesignTokenFileMultiPath() const
{
	return this->GetArgumentValueMulti(AA_DESIGN_TOKEN_FILE_PATH);
}


QByteArray CDesignTokenArgumentsParserComp::GetProjectName() const
{
	return this->GetArgumentValue(AA_PROJECT_NAME);
}


QByteArray CDesignTokenArgumentsParserComp::GetOutputDirectoryPath() const
{
	return this->GetArgumentValue(AA_OUTPUT_DIRECTORY_PATH);
}


QByteArray CDesignTokenArgumentsParserComp::GetOutputFilePath() const
{
	return this->GetArgumentValue(AA_OUTPUT_FILE_PATH);
}


} // namespace imtdesign


