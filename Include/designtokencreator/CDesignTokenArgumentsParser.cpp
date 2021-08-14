#include <designtokencreator/CDesignTokenArgumentsParser.h>



namespace designtokencreator
{


CDesignTokenArgumentsParser::CDesignTokenArgumentsParser(int argc, char** argv)
{
	//=====================================================================================

	//=====================================================================================Commands init
	m_commands.insert(AvailableArguments::AA_HELP, "--help");
	m_commands.insert(AvailableArguments::AA_HELP, "-h");
	m_commands.insert(AvailableArguments::AA_HELP, "/?");


	//=====================================================================================Reading arguments
}


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





} // namespace designtokencreator


