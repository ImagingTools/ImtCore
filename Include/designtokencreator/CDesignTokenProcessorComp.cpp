#include <designtokencreator/CDesignTokenProcessorComp.h>
#include <designtokencreator/CDesignTokenArgumentsParser.h>


namespace designtokencreator
{

int CDesignTokenProcessorComp::Exec()
{
	if(!m_replaceableColor.length()){
		qCritical() << "Replaceable color set not";
		return this->ShowHelp();
	}

	if(!m_normalColor.length()
				&& !m_offNormalColor.length()
				&& !m_offDisabledColor.length()
				&& !m_offActiveColor.length()
				&& !m_offSelectedColor.length()
				&& !m_onNormalColor.length()
				&& !m_onDisabledColor.length()
				&& !m_onActiveColor.length()
				&& !m_onSelectedColor.length()){
		qCritical() << "Any state color set not";
		return this->ShowHelp();
	}

	if(!m_inputFileName.length() ){
		qCritical() << "Input file set not";
		return this->ShowHelp();
	}
	if(!m_inputDirName.length() ){
		qCritical() << "Input dir set not";
		return this->ShowHelp();
	}
	if(!m_outputDirName.length()){
		qCritical() << "Output dir set not";
		return this->ShowHelp();
	}

	if(m_inputFileName.length() && m_inputDirName.length()){
		this->SetColorForAllModeState(m_inputFileName, m_outputDirName);
		return 0;
	}
	else if(m_inputDirName.length() && m_outputDirName.length()){
		this->SetColorAllFilesInDir(m_inputDirName, m_outputDirName);
		return 0;
	}
	return -1;
}


void CDesignTokenProcessorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	using L_ATTR = CDesignTokenArgumentsParser::AvailableArguments;

	auto L_SetParam = [&](L_ATTR propId, QByteArray& classProp){
		QByteArray newProp;
		m_paramSetAttrPtr->GetParameter(QByteArray::number(int(propId)));
		classProp = newProp;
	};

	this->Exec();
}

int CDesignTokenProcessorComp::ShowHelp() const
{
	QString helpText = "Invalid arguments \n \t\tAnd other help text\n\n";
	qCritical() << helpText.toLocal8Bit().constData();
	return -1;
}


void CDesignTokenProcessorComp::SetColor(const QByteArray& fileName, const QByteArray& outputFileName, const QByteArray& replacedColor) const
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

void CDesignTokenProcessorComp::SetColorForAllModeState(const QByteArray& fileName, const QByteArray& outputDirName) const
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


void CDesignTokenProcessorComp::SetColorAllFilesInDir(const QByteArray& inputDirName, const QByteArray& outputDirName) const
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



} // namespace designtokencreator


