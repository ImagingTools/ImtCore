#include <imtstyle/CDesignTokenProcessorComp.h>

// Acf includes
#include <iprm/IParamsSet.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <ibase/IApplication.h>
#include <ifile/IFileNameParam.h>



namespace imtstyle
{

int CDesignTokenProcessorComp::Exec()
{
	if(!m_templateIconColor.length()){
		qCritical() << "Replaceable color set not";
		return this->ShowHelp();
	}

	if(!m_inputFileName.length() ){
		qCritical() << "Input file set not";
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
	else {
		qCritical() << "Unexpected error";
		return this->ShowHelp();
	}
	return -1;
}


void CDesignTokenProcessorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_paramSetAttrPtr.IsValid()){
		{
			QByteArray paramId = "HelpParam";

			iprm::IEnableableParam* templateIconColorParamPtr = dynamic_cast<iprm::IEnableableParam*>(m_paramSetAttrPtr->GetEditableParameter(paramId));

			if (templateIconColorParamPtr->IsEnabled()) {
				this->ShowHelp();
				::exit(0);
			}
		}
		{
			QByteArray paramId = "TemplateIconColor";

			iprm::IIdParam* templateIconColorParamPtr = dynamic_cast<iprm::IIdParam*>(m_paramSetAttrPtr->GetEditableParameter(paramId));
			Q_ASSERT(templateIconColorParamPtr != nullptr);

			m_templateIconColor = templateIconColorParamPtr->GetId();
		}

		{
			QByteArray paramId = "TemplatePrimaryColor";

			iprm::IIdParam* templateIconColorParamPtr = dynamic_cast<iprm::IIdParam*>(m_paramSetAttrPtr->GetEditableParameter(paramId));
			Q_ASSERT(templateIconColorParamPtr != nullptr);

			m_templateIconColor = templateIconColorParamPtr->GetId();
		}
		{
			QByteArray paramId = "TemplateSecondaryColor";

			iprm::IIdParam* templateIconColorParamPtr = dynamic_cast<iprm::IIdParam*>(m_paramSetAttrPtr->GetEditableParameter(paramId));
			Q_ASSERT(templateIconColorParamPtr != nullptr);

			m_templateIconColor = templateIconColorParamPtr->GetId();
		}
		{
			QByteArray paramId = "DesignTokenFilePath";

			ifile::IFileNameParam* designTokenFilePathParamPtr = dynamic_cast<ifile::IFileNameParam*>(m_paramSetAttrPtr->GetEditableParameter(paramId));
			Q_ASSERT(designTokenFilePathParamPtr != nullptr);

			m_designTokenFileInfo = QFileInfo(designTokenFilePathParamPtr->GetPath());
		}
	}
	else {
		m_designTokenFileParserAttrPtr->SetFile(m_argumentParserAttrPtr->GetDesignTokenFilePath());
		m_designTokenFileParserAttrPtr->ParseFile();
		QStringList styles = m_designTokenFileParserAttrPtr->GetStyleNames();

		for (const QString& styleName: ::qAsConst(styles)){
			m_templateIconColor = m_designTokenFileParserAttrPtr->GetTemplateIconColor(styleName);
			m_offNormalColor =  m_designTokenFileParserAttrPtr->GetOffNormalColor(styleName);
			m_offDisabledColor =  m_designTokenFileParserAttrPtr->GetOffDisabledColor(styleName);
			m_offActiveColor =  m_designTokenFileParserAttrPtr->GetOffActiveColor(styleName);
			m_offSelectedColor =  m_designTokenFileParserAttrPtr->GetOffSelectedColor(styleName);
			m_onNormalColor =  m_designTokenFileParserAttrPtr->GetOnNormalColor(styleName);
			m_onDisabledColor =  m_designTokenFileParserAttrPtr->GetOnDisabledColor(styleName);
			m_onActiveColor =  m_designTokenFileParserAttrPtr->GetOnActiveColor(styleName);
			m_onSelectedColor =  m_designTokenFileParserAttrPtr->GetOnSelectedColor(styleName);

			QByteArray outputDirName = m_outputDirName + QDir::separator().toLatin1() + styleName.toLocal8Bit().constData();
			this->SetColorAllFilesInDir(m_inputDirName, outputDirName);
		}
	}
}

int CDesignTokenProcessorComp::ShowHelp() const
{
	QString helpText = "Invalid arguments \n \t\tAnd other help text\n\n";
	qCritical() << helpText.toLocal8Bit().constData();
	return -1;
}


void CDesignTokenProcessorComp::SetColor(const QByteArray& fileName, const QByteArray& outputFileName, const QByteArray& replacedColor, const QByteArray& reolacebleColor) const
{
	QByteArray fileData;

	QFile originalImageFile(fileName);
	Q_ASSERT(originalImageFile.open(QFile::ReadOnly));
	fileData = originalImageFile.readAll();
	originalImageFile.close();
	Q_ASSERT(fileData.length());

	fileData.replace((reolacebleColor.length() ? reolacebleColor : m_templateIconColor), replacedColor);

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

	if (m_offNormalColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffNormal + inputFileSuffix), m_offNormalColor);
	}
	if (m_offDisabledColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffDisabled + inputFileSuffix), m_offDisabledColor);
	}
	if (m_offActiveColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffActive + inputFileSuffix), m_offActiveColor);
	}
	if (m_offSelectedColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffSelected + inputFileSuffix), m_offSelectedColor);
	}
	if (m_onNormalColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnNormal + inputFileSuffix), m_onNormalColor);
	}
	if (m_onDisabledColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnDisabled + inputFileSuffix), m_onDisabledColor);
	}
	if (m_onActiveColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnActive + inputFileSuffix), m_onActiveColor);
	}
	if (m_onSelectedColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnSelected + inputFileSuffix), m_onSelectedColor);
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
		if(!this->IgnoreFile(inputFile)){
			this->SetColorForAllModeState(inputFile.absoluteFilePath().toLocal8Bit(), outputDirName);
		}
	}
}

bool CDesignTokenProcessorComp::IgnoreFile(const QFileInfo& fileInfo) const
{
	bool retval = false;

	const QByteArray& fileName = fileInfo.baseName().toUtf8();

	retval = fileName.endsWith(s_suffixOffNormal)
			|| fileName.endsWith(s_suffixOffDisabled)
			|| fileName.endsWith(s_suffixOffActive)
			|| fileName.endsWith(s_suffixOffSelected)
			|| fileName.endsWith(s_suffixOnNormal)
			|| fileName.endsWith(s_suffixOnDisabled)
			|| fileName.endsWith(s_suffixOnActive)
			|| fileName.endsWith(s_suffixOnSelected)
			;


	return retval;
}



} // namespace imtstyle


