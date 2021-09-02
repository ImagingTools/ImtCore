#include <imtstyle/CDesignTokenFileParserComp.h>

// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QtCore/QRegularExpressionMatchIterator>

// Imtcore includes
#include <imtstyle/CImtStyleUtils.h>


namespace imtstyle
{








QByteArray CDesignTokenFileParserComp::GetRawColor(const QByteArray& styleName, QPalette::ColorGroup group, QPalette::ColorRole role) const
{
    QList<RawColor> clolrs = m_styleSheetColors.values(styleName);
    for(const RawColor& color: ::qAsConst(m_styleSheetColors)){
        if (color.group == group && color.role == role){
            return color.value ;
        }
    }
    return QByteArray();
}


bool CDesignTokenFileParserComp::GetStyleSheetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const
{
	if(!designSchemaId.length() && m_styleSheetColors.size()) {
		palette = m_stylesPalettes.first();
	}
	else{
		palette = m_stylesPalettes[designSchemaId];
	}
	return true;
}




// reimplemented (IDesignTokenFileParser)
bool CDesignTokenFileParserComp::CDesignTokenFileParserComp::SetFile(const QByteArray& filePath)
{
	bool retval = false;

    QFileInfo designTokenFileInfo(filePath);

    retval = designTokenFileInfo.isReadable();

	if(retval){
		m_designTokenFileInfo = designTokenFileInfo;
	}

	return retval;
}


bool CDesignTokenFileParserComp::ParseFile()
{
	QFile designTokenFile;

	if (m_designTokenFileInfo.isReadable()){
		designTokenFile.setFileName(m_designTokenFileInfo.absoluteFilePath());
	}
	else {
		designTokenFile.setFileName(m_designTokenFilePathAttrPtr->GetPath());
	}

	if (!designTokenFile.open(QFile::ReadOnly)){
		qCritical() << "Cannot read file" << ::qPrintable(designTokenFile.fileName());
		Q_ASSERT(0);
		return false;
	}


	QJsonObject designTokenObject = QJsonDocument::fromJson(designTokenFile.readAll()).object();
	if(designTokenObject.isEmpty()) {
		qCritical() << "Cannot parse JSON";
		return false;
	}

	m_templateIconColor = designTokenObject["TemplateIconColor"].toString().toUtf8();

	QJsonArray designTokenStylesArray = designTokenObject["Styles"].toArray();
	if(designTokenStylesArray.isEmpty()) {
		qCritical() << "Cannot parse Styles";
		return false;
	}

	for (const QJsonValue& style: ::qAsConst(designTokenStylesArray)){

		QJsonObject styleEntry = style.toObject();
		QString styleName = styleEntry["StyleName"].toString();

		if(!styleName.length()){
			qInfo() << "Skipping invalid object";
			continue;
		}

		QJsonObject colorsObject = styleEntry["IconColor"].toObject();
		if(colorsObject.isEmpty()){
			qInfo() << "Skipping empty object";
		}

		QVariantMap colorsMap = colorsObject.toVariantMap();
		m_iconColors.insert(styleName, colorsMap);
		m_designSchemaList.InsertItem(styleName.toUtf8(), styleName,"");

		m_stylesPalettes.insert(styleName, CImtStyleUtils::GetPaletteFromEntry(styleEntry["StyleSheetColor"]));
		m_colorPalettes.insert(styleName, CImtStyleUtils::GetPaletteFromEntry(styleEntry["PaletteColor"]));

		imtbase::CCollectionInfo* themeFontsCollection = new imtbase::CCollectionInfo;
		QMap<QByteArray, QFont> fonts;

		CImtStyleUtils::GetFontsFromEntry(styleEntry["Fonts"], fonts, themeFontsCollection);
		m_fontsCollectionInfos.insert(styleName, istd::TSmartPtr<imtbase::ICollectionInfo>(themeFontsCollection));
		m_fonts.insert(styleName, fonts);
	}
	designTokenFile.close();
	return true;
}


bool CDesignTokenFileParserComp::SplitFile(const QString& outputDirPath, const QString& projectName)
{
	QFile designTokenFile;
	QDir outputDir(outputDirPath);
	if(!outputDir.exists()){
		Q_ASSERT(outputDir.mkpath(outputDirPath));
	}

	if (m_designTokenFileInfo.isReadable()){
		designTokenFile.setFileName(m_designTokenFileInfo.absoluteFilePath());
	}
	else {
		designTokenFile.setFileName(m_designTokenFilePathAttrPtr->GetPath());
	}

	if (!designTokenFile.open(QFile::ReadOnly)){
		qCritical() << "Cannot read file" << ::qPrintable(designTokenFile.fileName());
		Q_ASSERT(0);
		return false;
	}

	QFileInfo designTokenFileInfo(designTokenFile.fileName());

	QJsonObject designTokenObject = QJsonDocument::fromJson(designTokenFile.readAll()).object();
	if(designTokenObject.isEmpty()) {
		qCritical() << "Cannot parse JSON";
		return false;
	}

	QJsonArray designTokenStylesArray = designTokenObject["Styles"].toArray();
	if(designTokenStylesArray.isEmpty()) {
		qCritical() << "Cannot parse Styles";
		return false;
	}

	QJsonObject designTokenObjectSplitted = designTokenObject;

	for (const QJsonValue& style: ::qAsConst(designTokenStylesArray)){

		QJsonArray stylesArray;


		QJsonObject styleEntry = style.toObject();
		QString styleName = styleEntry["StyleName"].toString();
		stylesArray << styleEntry;
		designTokenObjectSplitted["Styles"] = stylesArray;

		QString outputSingleThemeFileName;
		if(outputDir.isEmpty()){
			outputSingleThemeFileName.append(designTokenFileInfo.absolutePath());
		}
		else{
			outputSingleThemeFileName.append(QDir::toNativeSeparators(outputDirPath));
		}
		if(!outputSingleThemeFileName.endsWith(QDir::separator())){
			outputSingleThemeFileName.append(QDir::separator());
		}
		outputSingleThemeFileName.append(projectName);
		outputSingleThemeFileName.append(styleName.toLower());
		outputSingleThemeFileName.append('.').append(designTokenFileInfo.completeSuffix());

		QFile outputSingleThemeFile(outputSingleThemeFileName);
		Q_ASSERT(outputSingleThemeFile.open(QFile::WriteOnly));
		outputSingleThemeFile.write(QJsonDocument(designTokenObjectSplitted).toJson());
		outputSingleThemeFile.close();
	}
	return true;
}


const imtbase::ICollectionInfo& CDesignTokenFileParserComp::GetDesignSchemaList() const
{
	return m_designSchemaList;
}


bool CDesignTokenFileParserComp::GetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const
{
	if(!designSchemaId.length() && m_colorPalettes.size()){
		palette = m_colorPalettes.first();
	}
	palette = m_colorPalettes[designSchemaId];
	return true;
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetTemplateIconColor(const QByteArray&) const
{
	return m_templateIconColor;
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetNormalColor(const QByteArray& styleName) const
{
    return m_iconColors[styleName].toMap()["Normal"].toByteArray();
}

QByteArray imtstyle::CDesignTokenFileParserComp::GetOffNormalColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[CImtStyleUtils::s_offNormalColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOffDisabledColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[CImtStyleUtils::s_offDisabledColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOffActiveColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[CImtStyleUtils::s_offActiveColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOffSelectedColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[CImtStyleUtils::s_offSelectedColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOnNormalColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[CImtStyleUtils::s_onNormalColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOnDisabledColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[CImtStyleUtils::s_onDisabledColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOnActiveColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[CImtStyleUtils::s_onActiveColorParamName].toByteArray();
}


QByteArray imtstyle::CDesignTokenFileParserComp::GetOnSelectedColor(const QByteArray& styleName) const
{
	return m_iconColors[styleName].toMap()[CImtStyleUtils::s_onSelectedColorParamName].toByteArray();
}


const imtbase::ICollectionInfo& CDesignTokenFileParserComp::GetFontList(const QByteArray& designSchemaId) const
{
	if (m_fonts.contains(designSchemaId) && m_fontsCollectionInfos[designSchemaId].GetPtr() != nullptr){
		const imtbase::ICollectionInfo& retval = *m_fontsCollectionInfos[designSchemaId];
		return retval;
	}
	return m_emptyCollectionInfo;
}


bool CDesignTokenFileParserComp::GetFont(const QByteArray& designSchemaId, const QByteArray& fontId, QFont& font) const
{
	if (m_fonts.contains(designSchemaId) && m_fonts[designSchemaId].contains(fontId)){
		font = m_fonts[designSchemaId][fontId];
		return true;
	}

	return false;
}


// reimplemented (ilog::CLoggerComponentBase)
void CDesignTokenFileParserComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}






} // namespace imtstyle
