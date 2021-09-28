#include <imtstyle/CDesignTokenIconProcessorComp.h>


// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <ibase/IApplication.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtstyle/CImtStyleUtils.h>


namespace imtstyle
{



// reimplemented (IDesignTokenProcessor)

QByteArray CDesignTokenIconProcessorComp::GetHelpString() const
{
	QByteArray retVal;
	QString helpText = "Invalid arguments \n \t\tAnd other help text\n\n";

	retVal = helpText.toLocal8Bit().constData();

	return retVal;
}


int CDesignTokenIconProcessorComp::Exec()
{
	if(!QDir(m_argumentParserAttrPtr->GetImagesInputDirectoryPath()).isReadable()){
		return -1;
	}

	if (m_paramSetAttrPtr.IsValid()){
		{
			QByteArray paramId = "HelpParam";

			iprm::IEnableableParam* templateIconColorParamPtr = dynamic_cast<iprm::IEnableableParam*>(m_paramSetAttrPtr->GetEditableParameter(paramId));
			Q_UNUSED(templateIconColorParamPtr);
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
		if(!m_templateIconColor.length()){
			qCritical() << "Replaceable color set not";
			return -1;
		}

		if(!m_inputFileName.length() ){
			qCritical() << "Input file set not";
			return -1;
		}
		if(!m_outputDirName.length()){
			qCritical() << "Output dir set not";
			return -1;
		}

		if(m_inputFileName.length() && m_inputDirName.length()){
			SetColorForAllModeState(m_inputFileName, m_outputDirName);
			return 0;
		}
		else if (m_inputDirName.length() && m_outputDirName.length()){
			if (!SetColorAllFilesInDir(m_inputDirName, m_outputDirName)){
				return -1;
			}
			else{
				return 0;
			}
		}
		else{
			qCritical() << "Unexpected error";
			return -1;
		}
		return -1;
	}
	else{
		m_designTokenFileParserAttrPtr->SetFile(m_argumentParserAttrPtr->GetDesignTokenFilePath());
		if (!m_designTokenFileParserAttrPtr->ParseFile()){
			return -1;
		}

		QVector<QByteArray> styles = m_designTokenFileParserAttrPtr->GetDesignSchemaList().GetElementIds();
		m_outputDirName = m_argumentParserAttrPtr->GetOutputDirectoryPath();
		m_inputDirName = m_argumentParserAttrPtr->GetImagesInputDirectoryPath();
		m_projectName = m_argumentParserAttrPtr->GetProjectName();
		if (!m_inputDirName.length()){
			qInfo() << "Icons directory was not set. Skipping...";
			return 0;
		}

		for (const QByteArray& styleName : ::qAsConst(styles)){
			m_templateIconColor = m_designTokenFileParserAttrPtr->GetTemplateIconColor(styleName);
			m_normalColor = m_designTokenFileParserAttrPtr->GetNormalColor(styleName);
			m_offNormalColor = m_designTokenFileParserAttrPtr->GetOffNormalColor(styleName);
			m_offDisabledColor = m_designTokenFileParserAttrPtr->GetOffDisabledColor(styleName);
			m_offActiveColor = m_designTokenFileParserAttrPtr->GetOffActiveColor(styleName);
			m_offSelectedColor = m_designTokenFileParserAttrPtr->GetOffSelectedColor(styleName);
			m_onNormalColor = m_designTokenFileParserAttrPtr->GetOnNormalColor(styleName);
			m_onDisabledColor = m_designTokenFileParserAttrPtr->GetOnDisabledColor(styleName);
			m_onActiveColor = m_designTokenFileParserAttrPtr->GetOnActiveColor(styleName);
			m_onSelectedColor = m_designTokenFileParserAttrPtr->GetOnSelectedColor(styleName);

			QByteArray outputDirName = m_outputDirName + QDir::separator().toLatin1() + QByteArray("Resources") + QDir::separator().toLatin1() + QByteArray("Icons") + QDir::separator().toLatin1() + styleName.constData();

			QDir resourceDir(m_inputDirName);
			resourceDir.cdUp();
			QDir colorResourceDir(m_inputDirName);
			for (const QFileInfo& possibleColorResourceDir : resourceDir.entryInfoList({(QString('*').append(styleName).append('*'))}, QDir::Dirs)){
				if (possibleColorResourceDir.isDir()){
					colorResourceDir.setPath(possibleColorResourceDir.absoluteFilePath());
					break;
				}
			}
			istd::CSystem::CopyDirectory(colorResourceDir.absolutePath().toUtf8(), outputDirName);

			QDir outputDir(outputDirName);
			QFileInfoList outputDirEntries = outputDir.entryInfoList(QDir::Files);

			for (const QFileInfo& outputDirEntry : ::qAsConst(outputDirEntries)){
				if (outputDirEntry.isFile()){
					QFile file(outputDirEntry.absoluteFilePath());
					file.setPermissions(
								file.permissions() |
								QFileDevice::WriteOwner |
								QFileDevice::WriteUser |
								QFileDevice::WriteGroup |
								QFileDevice::WriteOther);
				}
			}

			if (!SetColorAllFilesInDir(m_inputDirName, outputDirName)){
				return -1;
			}
		}
	}

	return 0;
}


bool CDesignTokenIconProcessorComp::SetColor(const QByteArray& fileName, const QByteArray& outputFileName, const QByteArray& replacedColor, const QByteArray& reolacebleColor) const
{
	QByteArray fileData;

	QFile originalImageFile(fileName);
	bool openInputImageFile = originalImageFile.open(QFile::ReadOnly);
	if (!openInputImageFile){
		qCritical() << "Cannot open image file" << fileName;

		return false;
	}

	fileData = originalImageFile.readAll();
	originalImageFile.close();
	if (fileData.length() < 1){
		qWarning() << "Skpiipng empty file" << fileName;

		return true;
	}

	QRegularExpression groupRegEx(QByteArray(reolacebleColor.length() ? reolacebleColor : m_templateIconColor), QRegularExpression::PatternOption::CaseInsensitiveOption);
	QRegularExpressionMatchIterator globalMatch = groupRegEx.globalMatch(fileData);

	while (globalMatch.hasNext()){
		QRegularExpressionMatch groupRegExMatch;
		groupRegExMatch = globalMatch.next();
		fileData.replace(groupRegExMatch.capturedStart(), groupRegExMatch.capturedLength(), replacedColor);
	}

	QFile outputImageFile(outputFileName);
	bool openOutputImageFile = outputImageFile.open(QFile::WriteOnly);
	if (!openOutputImageFile){
		qCritical() << "Cannot open output file" << outputFileName;

		outputImageFile.close();

		return false;
	}

	const qint64& writeBytes = outputImageFile.write(fileData);
	if (writeBytes <= 0){
		qCritical() << "Unable to write file" << outputFileName;

		outputImageFile.close();

		return false;
	}

	outputImageFile.flush();

	outputImageFile.close();

	return true;
}


bool CDesignTokenIconProcessorComp::SetColorForAllModeState(const QByteArray& fileName, const QByteArray& outputDirName) const
{
	QDir outputDir(outputDirName);
	if (!outputDir.exists()){
		bool createOutputDir = istd::CSystem::EnsurePathExists(outputDirName);
		if (!createOutputDir){
			qCritical() << "Cannot create output dir" << outputDirName;

			return false;
		}
	}

	QByteArray dirSeparator(1, QDir::separator().toLatin1());
	QFileInfo inputFileInfo(fileName);

	QByteArray inputFileBaseName = inputFileInfo.baseName().toLocal8Bit();
	if (inputFileBaseName.endsWith('.')){
		inputFileBaseName.chop(1);
	}

	QByteArray inputFileSuffix = inputFileInfo.completeSuffix().toLocal8Bit();
	if (!inputFileSuffix.startsWith('.')){
		inputFileSuffix.prepend('.');
	}

	if (m_normalColor.length()){
		if (!SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + inputFileSuffix), m_normalColor)){
			return false;
		}
	}

	if (m_offNormalColor.length()){
		if (!SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffNormal + inputFileSuffix), m_offNormalColor)){
			return false;
		}
	}

	if (m_offDisabledColor.length()){
		if (!SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffDisabled + inputFileSuffix), m_offDisabledColor)){
			return false;
		}
	}

	if (m_offActiveColor.length()){
		if (!SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffActive + inputFileSuffix), m_offActiveColor)){
			return false;
		}
	}

	if (m_offSelectedColor.length()){
		if (!SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffSelected + inputFileSuffix), m_offSelectedColor)){
			return false;
		}
	}

	if (m_onNormalColor.length()){
		if (!SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnNormal + inputFileSuffix), m_onNormalColor)){
			return false;
		}
	}

	if (m_onDisabledColor.length()){
		if (!SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnDisabled + inputFileSuffix), m_onDisabledColor)){
			return false;
		}
	}

	if (m_onActiveColor.length()){
		if (!SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnActive + inputFileSuffix), m_onActiveColor)){
			return false;
		}
	}

	if (m_onSelectedColor.length()){
		if (!SetColor(fileName, QByteArray(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnSelected + inputFileSuffix), m_onSelectedColor)){
			return false;
		}
	}
	return true;
}


bool CDesignTokenIconProcessorComp::SetColorAllFilesInDir(const QByteArray& inputDirName, const QByteArray& outputDirName) const
{
	QDir outputDir(outputDirName);
	if (!outputDir.exists()){
		bool createOutputDir = istd::CSystem::EnsurePathExists(outputDirName);
		if (!createOutputDir){
			qCritical() << "Cannot create output dir" << outputDirName;
			return false;
		}
	}

	QDir inputDir(inputDirName);
	QStringList nameFilters = {"*.svg"};
	QDir::Filters filters = QDir::Filter::Files | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList inputFiles = inputDir.entryInfoList(nameFilters, filters, sort);

	for (const QFileInfo& inputFile : ::qAsConst(inputFiles)){
		if (!IgnoreFile(inputFile)){
			if (!SetColorForAllModeState(inputFile.absoluteFilePath().toLocal8Bit(), outputDirName)){
				return false;
			}
		}
	}

	return true;
}


bool CDesignTokenIconProcessorComp::IgnoreFile(const QFileInfo& fileInfo) const
{
	const QByteArray& fileName = fileInfo.baseName().toUtf8();

	bool retVal =
				fileName.endsWith(s_suffixOffNormal) ||
				fileName.endsWith(s_suffixOffDisabled) ||
				fileName.endsWith(s_suffixOffActive) ||
				fileName.endsWith(s_suffixOffSelected) ||
				fileName.endsWith(s_suffixOnNormal) ||
				fileName.endsWith(s_suffixOnDisabled) ||
				fileName.endsWith(s_suffixOnActive) ||
				fileName.endsWith(s_suffixOnSelected);

	return retVal;
}


} // namespace imtstyle


