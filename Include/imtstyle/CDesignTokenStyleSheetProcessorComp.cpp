#include <imtstyle/CDesignTokenStyleSheetProcessorComp.h>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <ibase/IApplication.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtstyle/CDesignTokenFileParserComp.h>
#include <imtstyle/CImtStyleUtils.h>


namespace imtstyle
{


// reimplemented (IDesignTokenProcessor)

QByteArray CDesignTokenStyleSheetProcessorComp::GetHelpString() const
{
	QByteArray retval;
	QString helpText = "Invalid arguments \n \t\tAnd other help text\n\n";
	retval = helpText.toLocal8Bit().constData();
	return retval;
}


int CDesignTokenStyleSheetProcessorComp::Exec()
{
	if(!QDir(m_argumentParserAttrPtr->GetStyleSheetsInputDirectoryPath()).isReadable()){
		return -1;
	}

	m_designTokenFileParserAttrPtr->SetFile(m_argumentParserAttrPtr->GetDesignTokenFilePath());
	if(!m_designTokenFileParserAttrPtr->ParseFile()){
		return -1;
	}

	QVector<QByteArray> styles = m_designTokenFileParserAttrPtr->GetDesignSchemaList().GetElementIds();
	m_outputDirName = m_argumentParserAttrPtr->GetOutputDirectoryPath();
	m_inputDirName = m_argumentParserAttrPtr->GetStyleSheetsInputDirectoryPath();
	if(!m_inputDirName.length()){
		qInfo() << "Styles dir is not set skipping...";
		return 0;
	}

	for (const QByteArray& styleName: ::qAsConst(styles)){

		m_currentTheme = styleName;
		QVariantMap palette;
		m_designTokenFileParserAttrPtr->GetStyleSheetColorPalette(styleName, palette);
		QByteArray outputDirName = m_outputDirName + QDir::separator().toLatin1() + QByteArray("Resources") + QDir::separator().toLatin1() + QByteArray("Styles") + QDir::separator().toLatin1() + styleName.constData();

		m_currentPalette = palette;

		m_currentFontsCss.clear();
		QVector<QByteArray> fonts = m_designTokenFileParserAttrPtr->GetFontList(styleName).GetElementIds();
		for(const QByteArray& fontName: ::qAsConst(fonts)){
			QFont font;
			QByteArray cssFont;
			m_designTokenFileParserAttrPtr->GetFont(styleName, fontName,font);
			CImtStyleUtils::CreateCssFont(cssFont, font);
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

	for (const QFileInfo& inputFile: ::qAsConst(inputFiles)){

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

//	while (this->SetVariableColor(fileData, m_currentPalette));
	CImtStyleUtils::SetVariablesFromDualVariable(fileData, '$', '{', '}', m_currentPalette);

	CImtStyleUtils::SetVariables(fileData, '$', '(', ')', m_currentFontsCss);

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

			if(CImtStyleUtils::GetColorRoleGroup(colorName, colorGroup, colorRole)){
				QColor color = palette.color(colorGroup, colorRole);
				QByteArray colorHex;
				if(color.name() == "#000000"){
					colorHex = m_designTokenFileParserAttrPtr->GetRawColor(m_currentTheme, colorGroup, colorRole);
					if(!colorHex.length()){
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


} // namespace imtstyle


