#include <imtstyle/CDesignTokenStyleSheetProcessorComp.h>

// Acf includes
#include <iprm/IParamsSet.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <ibase/IApplication.h>
#include <ifile/IFileNameParam.h>

//ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtstyle/CDesignTokenFileParserComp.h>


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
		return 0;
	}

	m_designTokenFileParserAttrPtr->SetFile(m_argumentParserAttrPtr->GetDesignTokenFilePath());
	m_designTokenFileParserAttrPtr->ParseFile();

	QVector<QByteArray> styles = m_designTokenFileParserAttrPtr->GetDesignSchemaList().GetElementIds();
	m_outputDirName = m_argumentParserAttrPtr->GetOutputDirectoryPath();
	m_inputDirName = m_argumentParserAttrPtr->GetStyleSheetsInputDirectoryPath();

	for (const QByteArray& styleName: ::qAsConst(styles)){

		m_currentTheme = styleName;
		QPalette palette;
		m_designTokenFileParserAttrPtr->GetColorPalette(styleName, palette);
		QByteArray outputDirName = m_outputDirName + QDir::separator().toLatin1() + QByteArray("Resources") + QDir::separator().toLatin1() + QByteArray("Styles") + QDir::separator().toLatin1() + styleName.constData();
		this->SetColorAllFilesInDir(m_inputDirName, outputDirName, palette);
	}
	return 0;
}



void CDesignTokenStyleSheetProcessorComp::SetColorAllFilesInDir(const QByteArray& inputDirName, const QByteArray& outputDirName, const QPalette& palette) const
{
	QDir outputDir(outputDirName);
	if(!outputDir.exists()){
		Q_ASSERT(outputDir.mkpath(outputDirName));
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

		this->SetColor(inputFile.absoluteFilePath().toLocal8Bit(), QByteArray(outputDirName + dirSeparator + inputFileBaseName + inputFileSuffix), palette);
	}
}


void CDesignTokenStyleSheetProcessorComp::SetColor(const QByteArray& fileName, const QByteArray& outputFileName, const QPalette& palette) const
{
	QByteArray fileData;

	QFile originalImageFile(fileName);
	Q_ASSERT(originalImageFile.open(QFile::ReadOnly));
	fileData = originalImageFile.readAll();
	originalImageFile.close();
	Q_ASSERT(fileData.length());

	while (this->SetVariableColor(fileData, palette));

	QFile outputImageFile(outputFileName);
	Q_ASSERT(outputImageFile.open(QFile::WriteOnly));
	Q_ASSERT(outputImageFile.write(fileData));
	outputImageFile.flush();
	outputImageFile.close();
}

bool CDesignTokenStyleSheetProcessorComp::SetVariableColor(QByteArray& data, const QPalette& palette) const
{
	/// points to '$' symbol
	int indexOfBeginVariable = -1;
	/// points to '}' symbol
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

			if(m_designTokenFileParserAttrPtr->GetColorRoleGroup(colorName, colorGroup, colorRole)){
				QColor color = palette.color(colorGroup, colorRole);
				QByteArray colorHex;
				if(color.name() == "#000000"){
				}
				else {
					colorHex = color.name().toUtf8();
				}
				data.replace(indexOfBeginVariable, lengthOfVariable+3, colorHex);
			}
			return true;
		}
	}
	return false;
}


} // namespace imtstyle


