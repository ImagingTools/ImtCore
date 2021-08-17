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

	if(!m_paramSetAttrPtr.IsValid()){
		qCritical() << "paramSet invalid";
		return;
	}

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

	fileData.replace(m_templateIconColor, replacedColor);

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

	QByteArray normalColor = "#ff0000";
	QByteArray offNormalColor = "#00ff00";
	QByteArray offDisabledColor = "#0000ff";
	QByteArray offActiveColor = "#ff00ff";
	QByteArray offSelectedColor = "#ffff00";
	QByteArray onNormalColor = "#00ffff";
	QByteArray onDisabledColor = "#f0ffd0";
	QByteArray onActiveColor = "#dedede";
	QByteArray onSelectedColor = "#0f0f0f";

	if (normalColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileInfo.fileName().toLocal8Bit()), normalColor);
	}
	if (offNormalColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffNormal + inputFileSuffix), offNormalColor);
	}
	if (offDisabledColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffDisabled + inputFileSuffix), offDisabledColor);
	}
	if (offActiveColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffActive + inputFileSuffix), offActiveColor);
	}
	if (offSelectedColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffSelected + inputFileSuffix), offSelectedColor);
	}
	if (onNormalColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnNormal + inputFileSuffix), onNormalColor);
	}
	if (onDisabledColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnDisabled + inputFileSuffix), onDisabledColor);
	}
	if (onActiveColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnActive + inputFileSuffix), onActiveColor);
	}
	if (onSelectedColor.length()){
		this->SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnSelected + inputFileSuffix), onSelectedColor);
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


