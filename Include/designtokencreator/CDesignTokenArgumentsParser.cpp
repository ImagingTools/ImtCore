#include <designtokencreator/CDesignTokenArgumentsParser.h>



namespace designtokencreator
{


CDesignTokenArgumentsParser::CDesignTokenArgumentsParser()
{
	//=====================================================================================Commands init
	m_commands.insert(AvailableCommands::HELP, "--help");
	m_commands.insert(AvailableCommands::HELP, "-h");
	m_commands.insert(AvailableCommands::HELP, "/?");

	//-----------------------------------colors
	m_commands.insert(AvailableCommands::REPLACEBLE_COLOR, "--replaceble-color");
	m_commands.insert(AvailableCommands::OFF_NORMAL_COLOR, "--off-normal-color");
	m_commands.insert(AvailableCommands::OFF_DISABLED_COLOR, "--off-disabled-color");
	m_commands.insert(AvailableCommands::OFF_ACTIVE_COLOR, "--off-active-color");
	m_commands.insert(AvailableCommands::OFF_SELECTED_COLOR, "--off-selected-color");
	m_commands.insert(AvailableCommands::ON_NORMAL_COLOR, "--on-normal-color");
	m_commands.insert(AvailableCommands::ON_DISABLED_COLOR, "--on-disabled-color");
	m_commands.insert(AvailableCommands::ON_ACTIVE_COLOR, "--on-active-color");
	m_commands.insert(AvailableCommands::ON_SELECTED_COLOR, "--on-selected-color");

	//=====================================================================================Reading arguments
}


bool CDesignTokenArgumentsParser::CheckArgument(AvailableCommands AC) const
{
	bool retval = false;

	QByteArrayList AC_commands = m_commands.values(AC);

	for(const QByteArray &AC_command : ::qAsConst(AC_commands)){
		if(m_arguments.contains(AC_command)){
			retval = true;
			break;
		}
	}

	return retval;
}


QByteArray CDesignTokenArgumentsParser::GetArgumentKey(AvailableCommands AC) const
{
	QByteArray retval;

	QByteArrayList AC_commands = m_commands.values(AC);
	QByteArray AC_key;

	for(const QByteArray &AC_command : ::qAsConst(AC_commands)){
		if(m_arguments.contains(AC_command)){
			AC_key = AC_command;
			break;
		}
	}

	int indexOfArg = m_arguments.indexOf(AC_key);

	if(m_arguments.size() < indexOfArg){
		retval = m_arguments[indexOfArg];
	}

	return retval;
}


QByteArray CDesignTokenArgumentsParser::GetArgumentValue(AvailableCommands AC) const
{
	QByteArray retval;
	QByteArray AC_key = this->GetArgumentKey(AC);
	int indexOfArg = m_arguments.indexOf(AC_key);

	if(m_arguments.size() < indexOfArg){
		retval = m_arguments[++indexOfArg];
	}

	return retval;
}


QByteArrayList CDesignTokenArgumentsParser::GetArgumentValueMulti(AvailableCommands AC) const
{
	QByteArrayList retval;
	QByteArrayList AC_commands = m_commands.values(AC);
	QByteArray AC_key;

	for(const QByteArray &AC_command : ::qAsConst(AC_commands)){
		if(m_arguments.contains(AC_command)){
			AC_key = AC_command;
			break;
		}
	}

	int indexOfArg = m_arguments.indexOf(AC_key);

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


void CDesignTokenArgumentsParser::SetParams(int argc, char** argv, iprm::IParamsSet* paramSet)
{
	for (int i = 0; i < argc; ++i){
		m_arguments << argv[i];
	}
}



} // namespace designtokencreator


