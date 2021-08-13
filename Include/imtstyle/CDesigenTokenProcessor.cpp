#include "CDesigenTokenProcessor.h"

CDesigenTokenProcessor::CDesigenTokenProcessor(int argc, char** argv, char** env)
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
	for (int i = 0; i < argc; ++i){
		m_arguments << argv[i];
	}

	//=====================================================================================Props init
	//-----------------------------------colors
	m_normalColor = this->GetArgumentValue(AvailableCommands::NORMAL_COLOR);
	m_replaceableColor = this->GetArgumentValue(AvailableCommands::REPLACEBLE_COLOR);
	m_offNormalColor = this->GetArgumentValue(AvailableCommands::OFF_NORMAL_COLOR);
	m_offDisabledColor = this->GetArgumentValue(AvailableCommands::OFF_DISABLED_COLOR);
	m_offActiveColor = this->GetArgumentValue(AvailableCommands::OFF_ACTIVE_COLOR);
	m_offSelectedColor = this->GetArgumentValue(AvailableCommands::OFF_SELECTED_COLOR);
	m_onNormalColor = this->GetArgumentValue(AvailableCommands::ON_NORMAL_COLOR);
	m_onDisabledColor = this->GetArgumentValue(AvailableCommands::ON_DISABLED_COLOR);
	m_onActiveColor = this->GetArgumentValue(AvailableCommands::ON_ACTIVE_COLOR);
	m_onSelectedColor = this->GetArgumentValue(AvailableCommands::ON_SELECTED_COLOR);

	//=====================================================================================

	m_replaceableColor = "#a6a6a6";
	m_normalColor =		 "#000000";
	m_offNormalColor =	 "#8103ff";
	m_offDisabledColor = "#0381ff";
	m_offActiveColor =	 "#03f7ff";
	m_offSelectedColor = "#03ff6c";
	m_onNormalColor =	 "#d1ff03";
	m_onDisabledColor =	 "#ff9a03";
	m_onActiveColor =	 "#d199c7";
	m_onSelectedColor =	 "#a32a2a";

}

int CDesigenTokenProcessor::Exec()
{
	if(m_arguments.size() < 2 || this->CheckArgument(AvailableCommands::HELP)){
		return this->ShowHelp();
	}


	return -1;
}


//=============================================================Working methods i.e. workers
int CDesigenTokenProcessor::ShowHelp() const
{
	QString helpText = "Invalid arguments \n \t\tAnd other help text\n\n";
	std::cerr << helpText.toLocal8Bit().constData();
	return -1;
}


void CDesigenTokenProcessor::SetColor(const QByteArray& fileName, const QByteArray& outputFileName, const QByteArray& replacedColor) const
{
	QByteArray fileData;

	QFile originalImageFile(fileName);
	Q_ASSERT(originalImageFile.open(QFile::ReadOnly));
	fileData = originalImageFile.readAll();
	originalImageFile.close();
	Q_ASSERT(fileData.length());

	fileData.replace(m_replaceableColor, replacedColor);

	QFile outputImageFile(outputFileName);
	Q_ASSERT(outputImageFile.open(QFile::WriteOnly));
	Q_ASSERT(outputImageFile.write(fileData));
	outputImageFile.flush();
	outputImageFile.close();
}

void CDesigenTokenProcessor::SetColorForAllModeState(const QByteArray& fileName, const QByteArray& outputDirName) const
{
	QDir outputDir(outputDirName);
	if(!outputDir.exists()){
		Q_ASSERT(outputDir.mkdir(outputDirName));
	}

	QByteArray dirSeparator(1, QDir::separator().toLatin1());
	QFileInfo inputFileInfo(fileName);
	QByteArray inputFileBaseName = inputFileInfo.baseName().toLocal8Bit();
	if(inputFileBaseName.endsWith('.')){
		inputFileBaseName.chop(1);
	}
	QByteArray inputFileSuffix = inputFileInfo.completeSuffix().toLocal8Bit();
	if(!inputFileSuffix.startsWith('.')){
		inputFileSuffix.prepend('.');
	}

	if (m_normalColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileInfo.fileName().toLocal8Bit()), m_normalColor);
	}
	if (m_offNormalColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + SUFFIX_OFF_NORMAL + inputFileSuffix), m_offNormalColor);
	}
	if (m_offDisabledColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + SUFFIX_OFF_DISABLED + inputFileSuffix), m_offDisabledColor);
	}
	if (m_offActiveColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + SUFFIX_OFF_ACTIVE + inputFileSuffix), m_offActiveColor);
	}
	if (m_offSelectedColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + SUFFIX_OFF_SELECTED + inputFileSuffix), m_offSelectedColor);
	}
	if (m_onNormalColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + SUFFIX_ON_NORMAL + inputFileSuffix), m_onNormalColor);
	}
	if (m_onDisabledColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + SUFFIX_ON_DISABLED + inputFileSuffix), m_onDisabledColor);
	}
	if (m_onActiveColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + SUFFIX_ON_ACTIVE + inputFileSuffix), m_onActiveColor);
	}
	if (m_onSelectedColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + SUFFIX_ON_SELECTED + inputFileSuffix), m_onSelectedColor);
	}

}


void CDesigenTokenProcessor::SetColorAllFilesInDir(const QByteArray& inputDirName, const QByteArray& outputDirName) const
{
	QDir outputDir(outputDirName);
	if(!outputDir.exists()){
		Q_ASSERT(outputDir.mkdir(outputDirName));
	}

	QDir inputDir(inputDirName);
	QStringList nameFilters = {"*.svg"};
	QDir::Filters filters = QDir::Filter::Files | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList inputFiles = inputDir.entryInfoList(nameFilters, filters, sort);

	for (const QFileInfo& inputFile: ::qAsConst(inputFiles)){
		this->SetColorForAllModeState(inputFile.absoluteFilePath().toLocal8Bit(), outputDirName);
	}

}



//=============================================================Private assistants
bool CDesigenTokenProcessor::CheckArgument(AvailableCommands AC) const
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


QByteArray CDesigenTokenProcessor::	GetArgumentKey(AvailableCommands AC) const
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


QByteArray CDesigenTokenProcessor::GetArgumentValue(AvailableCommands AC) const
{
	QByteArray retval;
	QByteArray AC_key = this->GetArgumentKey(AC);
	int indexOfArg = m_arguments.indexOf(AC_key);

	if(m_arguments.size() < indexOfArg){
		retval = m_arguments[++indexOfArg];
	}

	return retval;
}


QByteArrayList CDesigenTokenProcessor::GetArgumentValueMulti(AvailableCommands AC) const
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


