// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesign/CDesignTokenStyleSheetProcessorComp.h>


// ACF includes
#include <istd/CSystem.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtdesign/CDesignTokenFileParserComp.h>
#include <imtdesign/CDesignTokenStyleUtils.h>


namespace imtdesign
{


// reimplemented (IDesignTokenProcessor)

QByteArray CDesignTokenStyleSheetProcessorComp::GetHelpString() const
{
	QString helpText = "Invalid arguments \n \t\tAnd other help text\n\n";
	QByteArray retval = helpText.toLocal8Bit().constData();

	return retval;
}


int CDesignTokenStyleSheetProcessorComp::Exec()
{
	if(m_argumentParserCompPtr->GetStyleSheetsInputDirectoryPath().isEmpty()){
		qDebug() << "Folder containing style sheets was not set, processing skipped";
		return 0;
	}

	if(!QDir(m_argumentParserCompPtr->GetStyleSheetsInputDirectoryPath()).isReadable()){
		return -1;
	}

	QByteArrayList designTokenFileMultiPath = m_argumentParserCompPtr->GetDesignTokenFileMultiPath();

	for (const QByteArray& designTokenFilePath: designTokenFileMultiPath){
		m_designTokenFileParserCompPtr->SetFile(designTokenFilePath);
		m_designTokenFileParserCompPtr->ParseFile();
	}

	QVector<QByteArray> styles = m_designTokenFileParserCompPtr->GetDesignSchemaList().GetElementIds();
	m_outputDirName = m_argumentParserCompPtr->GetOutputDirectoryPath();
	m_inputDirName = m_argumentParserCompPtr->GetStyleSheetsInputDirectoryPath();
	if(m_inputDirName.isEmpty()){
		qInfo() << "Folder containing styles was not set, processing skipping...";
		return 0;
	}

	for (const QByteArray& styleName: ::std::as_const(styles)){
		m_currentTheme = styleName;
		QVariantMap palette;
		QVariantMap currentBasePalette;
		m_designTokenFileParserCompPtr->GetStyleSheetColorPalette(styleName, palette);
		m_designTokenFileParserCompPtr->GetBasePalette(styleName, currentBasePalette);
		QByteArray outputDirName = m_outputDirName + QDir::separator().toLatin1() + QByteArray("Resources") + QDir::separator().toLatin1() + QByteArray("Styles") + QDir::separator().toLatin1() + styleName.constData();

		m_currentPalette = palette;
		m_currentBasePalette = currentBasePalette;

		m_currentFontsCss.clear();
		QVector<QByteArray> fonts = m_designTokenFileParserCompPtr->GetFontList(styleName).GetElementIds();
		for(const QByteArray& fontName: ::std::as_const(fonts)){
			QFont font;
			QByteArray cssFont;
			m_designTokenFileParserCompPtr->GetFont(styleName, fontName,font);
			CDesignTokenStyleUtils::CreateCssFont(cssFont, font);
			m_currentFontsCss[fontName] = cssFont;
		}
		if(!this->ProcessAllCssFilesInDir(m_inputDirName, outputDirName)){
			return -1;
		}
	}

	return 0;
}



bool CDesignTokenStyleSheetProcessorComp::ProcessAllCssFilesInDir(const QByteArray& inputDirName, const QByteArray& outputDirName) const
{
	QDir outputDir(outputDirName);
	if(!outputDir.exists()){
		bool createOutputDir = istd::CSystem::EnsurePathExists(outputDirName);
		if (!createOutputDir){
			qCritical() << "Cannot create output dir" << outputDirName;
			return false;
		}
	}

	QByteArray dirSeparator(1, QDir::separator().toLatin1());

	QDir inputDir(inputDirName);
	QStringList nameFilters = {"*.css"};
	QDir::Filters filters = QDir::Filter::Files | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList inputFiles = inputDir.entryInfoList(nameFilters, filters, sort);

	for (const QFileInfo& inputFile: ::std::as_const(inputFiles)){

		QByteArray inputFileBaseName = inputFile.baseName().toLocal8Bit();
		if(inputFileBaseName.endsWith('.')){
			inputFileBaseName.chop(1);
		}
		QByteArray inputFileSuffix = inputFile.completeSuffix().toLocal8Bit();
		if(!inputFileSuffix.startsWith('.')){
			inputFileSuffix.prepend('.');
		}
		if(!this->ProcesCssFile(inputFile.absoluteFilePath().toLocal8Bit(), QByteArray(outputDirName + dirSeparator + inputFileBaseName + inputFileSuffix))){
			return false;
		}
	}
	return true;
}


bool CDesignTokenStyleSheetProcessorComp::ProcesCssFile(const QByteArray& fileName, const QByteArray& outputFileName) const
{
	QByteArray fileData;

	QFile originalImageFile(fileName);
	bool openFile = originalImageFile.open(QFile::ReadOnly);
	if(!openFile){
		qCritical() << "Cannot open file" << originalImageFile.fileName();
		return false;
	}
	fileData = originalImageFile.readAll();
	originalImageFile.close();
	if(fileData.length() < 1){
		qWarning() << "Skipping empty file" << fileName;
		return true;
	}

	CDesignTokenStyleUtils::SetVariablesFromDualVariable(fileData, '$', '{', '}', m_currentPalette);
	CDesignTokenStyleUtils::SetVariables(fileData, '$', '{', '}', m_currentBasePalette);
	CDesignTokenStyleUtils::SetVariables(fileData, '$', '(', ')', m_currentFontsCss);
	CDesignTokenStyleUtils::SetVariables(fileData, '$', '{', '}', QVariantMap({ std::make_pair("Scheme", m_currentTheme) }));

	// ensure, all variables is setted
	bool hasUnsettedValues = false;
	static QRegularExpression veriableRegEx(R"(\$\{.{0,}\})");
	QRegularExpressionMatchIterator variableGlobalMatch = veriableRegEx.globalMatch(fileData);
	while (variableGlobalMatch.hasNext()){
		QRegularExpressionMatch variableMatch = variableGlobalMatch.next();
		SendErrorMessage(0,
						 QString("The unsettled variable remains at: '%1' value : '%2'")
							 .arg(QString::number(variableMatch.capturedStart()), variableMatch.captured()));

		hasUnsettedValues = true;
	}
	if (hasUnsettedValues){
		return false;
	}

	QFile outputImageFile(outputFileName);
	bool openOutputFile = outputImageFile.open(QFile::WriteOnly);
	if(!openOutputFile){
		outputImageFile.close();
		return false;
	}
	const qint64& writtenBytes = outputImageFile.write(fileData);
	if(writtenBytes <= 0){
		outputImageFile.close();
		return false;
	}
	outputImageFile.flush();
	outputImageFile.close();
	return true;
}

bool CDesignTokenStyleSheetProcessorComp::SetVariableColor(QByteArray& data, const QPalette& palette) const
{
	int indexOfBeginVariable = -1;
	int indexOfEndVariable = -1;

	for(int i = 0; i < data.length(); ++i){

		QChar symbol(data.at(i));
		if(symbol == QChar('$') && i+1 < data.length()){

			QChar nextSymbol = data.at(i+1);
			if(nextSymbol == QChar('{')){
				indexOfBeginVariable = i;

				for(int j = i+1; j < data.length(); ++j){
					QChar endSymbol = data.at(j);

					if(endSymbol == QChar('}')){
						indexOfEndVariable = j;
						break;
					}
				}
			}
		}

		if(indexOfBeginVariable > -1 && indexOfEndVariable > indexOfBeginVariable){

			int lengthOfVariable = indexOfEndVariable - indexOfBeginVariable - 2;
			QString colorName = data.mid(indexOfBeginVariable+2, lengthOfVariable);

			QPalette::ColorGroup colorGroup;
			QPalette::ColorRole colorRole;

			if(CDesignTokenStyleUtils::GetColorRoleGroup(colorName, colorGroup, colorRole)){
				QColor color = palette.color(colorGroup, colorRole);
				QByteArray colorHex;
				if(color.name() == "#000000"){
					colorHex = m_designTokenFileParserCompPtr->GetRawColor(m_currentTheme, colorGroup, colorRole);
					if(colorHex.isEmpty()){
						colorHex = "#000000";
					}
				}
				else {
					colorHex = color.name().toUtf8();
				}
				data.replace(indexOfBeginVariable, lengthOfVariable+3, colorHex);
				return true;
			}
		}
	}
	return false;
}


} // namespace imtdesign


