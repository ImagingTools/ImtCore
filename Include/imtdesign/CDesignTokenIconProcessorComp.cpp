// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesign/CDesignTokenIconProcessorComp.h>


// ACF includes
#include <istd/CSystem.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtdesign/CDesignTokenStyleUtils.h>


namespace imtdesign
{



// reimplemented (IDesignTokenProcessor)

QByteArray CDesignTokenIconProcessorComp::GetHelpString() const
{
	QString helpText = "Invalid arguments \n \t\tAnd other help text\n\n";
	QByteArray retVal = helpText.toLocal8Bit().constData();

	return retVal;
}


int CDesignTokenIconProcessorComp::Exec()
{
	if (!m_argumentParserCompPtr.IsValid()){
		return -1;
	}

	QString imageInputDirectoryPath = m_argumentParserCompPtr->GetImagesInputDirectoryPath();
	if(imageInputDirectoryPath.isEmpty()){
		qDebug() << "Path to the folder containg image data was not defined. Image processing has been skipped";

		return 0;
	}

	if(!QDir(imageInputDirectoryPath).isReadable()){
		return -1;
	}

	QByteArrayList designTokenFileMultiPath = m_argumentParserCompPtr->GetDesignTokenFileMultiPath();

	for (const QByteArray& designTokenFilePath: designTokenFileMultiPath){
		m_designTokenFileParserCompPtr->SetFile(designTokenFilePath);
		m_designTokenFileParserCompPtr->ParseFile();

		QVector<QByteArray> styles = m_designTokenFileParserCompPtr->GetDesignSchemaList().GetElementIds();
		m_outputDirName = m_argumentParserCompPtr->GetOutputDirectoryPath();
		m_inputDirName = imageInputDirectoryPath;
		m_projectName = m_argumentParserCompPtr->GetProjectName();
		if (m_inputDirName.isEmpty()){
			qInfo() << "Icons directory was not set. Skipping...";

			return 0;
		}

		for (const QByteArray& styleName : ::std::as_const(styles)){
			m_templateIconColorList = m_designTokenFileParserCompPtr->GetTemplateIconColorList(styleName);
			for (const QByteArray& templateColor: m_templateIconColorList){
				m_normalColor [templateColor] = m_designTokenFileParserCompPtr->GetIconColor(styleName, IDesignTokenFileParser::IS_NORMAL, templateColor);
				m_offNormalColor[templateColor] = m_designTokenFileParserCompPtr->GetIconColor(styleName, IDesignTokenFileParser::IS_OFF_NORMAL, templateColor);
				m_offDisabledColor[templateColor] = m_designTokenFileParserCompPtr->GetIconColor(styleName, IDesignTokenFileParser::IS_OFF_DISABLED, templateColor);
				m_offActiveColor[templateColor] = m_designTokenFileParserCompPtr->GetIconColor(styleName, IDesignTokenFileParser::IS_OFF_ACTIVE, templateColor);
				m_offSelectedColor[templateColor] = m_designTokenFileParserCompPtr->GetIconColor(styleName, IDesignTokenFileParser::IS_OFF_SELECTED, templateColor);
				m_onNormalColor[templateColor] = m_designTokenFileParserCompPtr->GetIconColor(styleName, IDesignTokenFileParser::IS_ON_NORMAL, templateColor);
				m_onDisabledColor[templateColor] = m_designTokenFileParserCompPtr->GetIconColor(styleName, IDesignTokenFileParser::IS_ON_DISABLED, templateColor);
				m_onActiveColor[templateColor] = m_designTokenFileParserCompPtr->GetIconColor(styleName, IDesignTokenFileParser::IS_ON_ACTIVE, templateColor);
				m_onSelectedColor[templateColor] = m_designTokenFileParserCompPtr->GetIconColor(styleName, IDesignTokenFileParser::IS_ON_SELECTED, templateColor);
			}

			QString outputDirName = m_outputDirName + QDir::separator().toLatin1() + QByteArray("Resources") + QDir::separator().toLatin1() + QByteArray("Icons") + QDir::separator().toLatin1() + styleName.constData();

			QDir resourceDir(m_inputDirName);
			QDir colorResourceDir(m_inputDirName);
			colorResourceDir.cdUp();
			for (const QFileInfo& possibleColorResourceDir : colorResourceDir.entryInfoList({(QString('*').append(styleName).append('*'))}, QDir::Dirs)){
				if (possibleColorResourceDir.isDir()){
					istd::CSystem::CopyDirectory(possibleColorResourceDir.absoluteFilePath(), outputDirName);
					break;
				}
			}

			QDir outputDir(outputDirName);
			QFileInfoList outputDirEntries = outputDir.entryInfoList(QDir::Files);

			for (const QFileInfo& outputDirEntry : ::std::as_const(outputDirEntries)){
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


QByteArray CDesignTokenIconProcessorComp::GetFileNameForState(const QString& fileName, IDesignTokenFileParser::IconState iconState) const
{
	QFileInfo fileInfo(fileName);

	QString templateName = fileInfo.completeBaseName();
	QString iconStatesDir = fileInfo.path() + QDir::separator() + templateName;
	if (QDir(iconStatesDir).exists()){
		QString iconStateFileName;

		switch (iconState){
		case IDesignTokenFileParser::IS_OFF_NORMAL:
			iconStateFileName = CDesignTokenStyleUtils::s_offNormalColorParamName + QString(".") + fileInfo.suffix();
			break;
		case IDesignTokenFileParser::IS_OFF_DISABLED:
			iconStateFileName = CDesignTokenStyleUtils::s_offDisabledColorParamName + QString(".") + fileInfo.suffix();
			break;
		case IDesignTokenFileParser::IS_OFF_ACTIVE:
			iconStateFileName = CDesignTokenStyleUtils::s_offActiveColorParamName + QString(".") + fileInfo.suffix();
			break;
		case IDesignTokenFileParser::IS_OFF_SELECTED:
			iconStateFileName = CDesignTokenStyleUtils::s_offSelectedColorParamName + QString(".") + fileInfo.suffix();
			break;
		case IDesignTokenFileParser::IS_ON_NORMAL:
			iconStateFileName = CDesignTokenStyleUtils::s_onNormalColorParamName + QString(".") + fileInfo.suffix();
			break;
		case IDesignTokenFileParser::IS_ON_DISABLED:
			iconStateFileName = CDesignTokenStyleUtils::s_onDisabledColorParamName + QString(".") + fileInfo.suffix();
			break;
		case IDesignTokenFileParser::IS_ON_ACTIVE:
			iconStateFileName = CDesignTokenStyleUtils::s_onActiveColorParamName + QString(".") + fileInfo.suffix();
			break;
		case IDesignTokenFileParser::IS_ON_SELECTED:
			iconStateFileName = CDesignTokenStyleUtils::s_onSelectedColorParamName + QString(".") + fileInfo.suffix();
			break;
		default:
			break;
		}

		if (!iconStateFileName.isEmpty()){
			QString iconStateFilePath = iconStatesDir + QDir::separator() + iconStateFileName;
			if (QFileInfo::exists(iconStateFilePath)){
				return iconStateFilePath.toLocal8Bit();
			}
		}
	}

	return QByteArray();
}


bool CDesignTokenIconProcessorComp::SetColor(const QString& fileName, const QString& outputFileName, const QByteArray& replacedColor, const QByteArray& replacebleColor) const
{
	QString readFilePath = QFile::exists(outputFileName) ? outputFileName : fileName;

	QFile originalImageFile(readFilePath);
	bool openInputImageFile = originalImageFile.open(QFile::ReadOnly);
	if (!openInputImageFile){
		qCritical() << "Cannot open image file" << readFilePath;

		return false;
	}

	QByteArray fileData = originalImageFile.readAll();
	originalImageFile.close();
	if (fileData.length() < 1){
		qWarning() << "Skpiipng empty file" << readFilePath;

		return true;
	}

	QRegularExpression groupRegEx(replacebleColor, QRegularExpression::PatternOption::CaseInsensitiveOption);
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


bool CDesignTokenIconProcessorComp::SetColorForAllModeState(const QString& fileName, const QString& outputDirName) const
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

	QByteArray inputFileBaseName = inputFileInfo.completeBaseName().toLocal8Bit();
	if (inputFileBaseName.endsWith('.')){
		inputFileBaseName.chop(1);
	}

	QByteArray inputFileSuffix = inputFileInfo.suffix().toLocal8Bit();
	if (!inputFileSuffix.startsWith('.')){
		inputFileSuffix.prepend('.');
	}

	for (const QByteArray& templateColor: std::as_const(m_templateIconColorList)){
		if (!m_normalColor[templateColor].isEmpty()){
			QString stateFileName = GetFileNameForState(fileName, IDesignTokenFileParser::IS_NORMAL);
			stateFileName = stateFileName.isEmpty() ? fileName : stateFileName;
			if (!SetColor(stateFileName, QString(outputDirName + dirSeparator + inputFileBaseName + inputFileSuffix), m_normalColor[templateColor], templateColor)){
				return false;
			}
		}

		if (!m_offNormalColor[templateColor].isEmpty()){
			QString stateFileName = GetFileNameForState(fileName, IDesignTokenFileParser::IS_OFF_NORMAL);
			stateFileName = stateFileName.isEmpty() ? fileName : stateFileName;
			if (!SetColor(stateFileName, QString(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffNormal + inputFileSuffix), m_offNormalColor[templateColor], templateColor)){
				return false;
			}
		}

		if (!m_offDisabledColor[templateColor].isEmpty()){
			QString stateFileName = GetFileNameForState(fileName, IDesignTokenFileParser::IS_OFF_DISABLED);
			stateFileName = stateFileName.isEmpty() ? fileName : stateFileName;
			if (!SetColor(stateFileName, QString(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffDisabled + inputFileSuffix), m_offDisabledColor[templateColor], templateColor)){
				return false;
			}
		}

		if (!m_offActiveColor[templateColor].isEmpty()){
			QString stateFileName = GetFileNameForState(fileName, IDesignTokenFileParser::IS_OFF_ACTIVE);
			stateFileName = stateFileName.isEmpty() ? fileName : stateFileName;
			if (!SetColor(stateFileName, QString(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffActive + inputFileSuffix), m_offActiveColor[templateColor], templateColor)){
				return false;
			}
		}

		if (!m_offSelectedColor[templateColor].isEmpty()){
			QString stateFileName = GetFileNameForState(fileName, IDesignTokenFileParser::IS_OFF_SELECTED);
			stateFileName = stateFileName.isEmpty() ? fileName : stateFileName;
			if (!SetColor(stateFileName, QString(outputDirName + dirSeparator + inputFileBaseName + s_suffixOffSelected + inputFileSuffix), m_offSelectedColor[templateColor], templateColor)){
				return false;
			}
		}

		if (!m_onNormalColor[templateColor].isEmpty()){
			QString stateFileName = GetFileNameForState(fileName, IDesignTokenFileParser::IS_ON_NORMAL);
			stateFileName = stateFileName.isEmpty() ? fileName : stateFileName;
			if (!SetColor(stateFileName, QString(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnNormal + inputFileSuffix), m_onNormalColor[templateColor], templateColor)){
				return false;
			}
		}

		if (!m_onDisabledColor[templateColor].isEmpty()){
			QString stateFileName = GetFileNameForState(fileName, IDesignTokenFileParser::IS_ON_DISABLED);
			stateFileName = stateFileName.isEmpty() ? fileName : stateFileName;
			if (!SetColor(stateFileName, QString(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnDisabled + inputFileSuffix), m_onDisabledColor[templateColor], templateColor)){
				return false;
			}
		}

		if (!m_onActiveColor[templateColor].isEmpty()){
			QString stateFileName = GetFileNameForState(fileName, IDesignTokenFileParser::IS_ON_ACTIVE);
			stateFileName = stateFileName.isEmpty() ? fileName : stateFileName;
			if (!SetColor(stateFileName, QString(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnActive + inputFileSuffix), m_onActiveColor[templateColor], templateColor)){
				return false;
			}
		}

		if (!m_onSelectedColor[templateColor].isEmpty()){
			QString stateFileName = GetFileNameForState(fileName, IDesignTokenFileParser::IS_ON_SELECTED);
			stateFileName = stateFileName.isEmpty() ? fileName : stateFileName;
			if (!SetColor(stateFileName, QString(outputDirName + dirSeparator + inputFileBaseName + s_suffixOnSelected + inputFileSuffix), m_onSelectedColor[templateColor], templateColor)){
				return false;
			}
		}
	}

	return true;
}


bool CDesignTokenIconProcessorComp::SetColorAllFilesInDir(const QString& inputDirName, const QString& outputDirName) const
{
	QDir outputDir(outputDirName);
	if (!outputDir.exists()){
		bool createOutputDir = istd::CSystem::EnsurePathExists(outputDirName);
		if (!createOutputDir){
			qCritical() << "Cannot create output directory: " << outputDirName;
			return false;
		}
	}

	QDir inputDir(inputDirName);
	QStringList nameFilters = {"*.svg"};
	QDir::Filters filters = QDir::Filter::Files | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList inputFiles = inputDir.entryInfoList(nameFilters, filters, sort);

	for (const QFileInfo& inputFile : ::std::as_const(inputFiles)){
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
	const QString& fileName = fileInfo.completeBaseName();

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


} // namespace imtdesign


