#include <imtdesign/CDesignTokenFileParserComp.h>


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QtCore/QRegularExpressionMatchIterator>

// Imtcore includes
#include <imtdesign/CDesignTokenStyleUtils.h>


namespace imtdesign
{


QByteArray CDesignTokenFileParserComp::GetRawColor(
			const QByteArray& /*styleName*/,
			QPalette::ColorGroup group,
			QPalette::ColorRole role) const
{
	for(const RawColor& color: ::qAsConst(m_styleSheetColors)){
		if (color.group == group && color.role == role){
			return color.value;
		}
	}

	return QByteArray();
}


bool CDesignTokenFileParserComp::GetStyleSheetColorPalette(const QByteArray& designSchemaId, QVariantMap& palette) const
{
	if(!designSchemaId.length() && m_styleSheetColors.size()){
		palette = m_stylesPalettes.first();
	}
	else{
		palette = m_stylesPalettes[designSchemaId];
	}

	return true;
}


bool CDesignTokenFileParserComp::GetBasePalette(const QByteArray& designSchemaId, QVariantMap& palette) const
{
	if(m_stylesBasePalettes.size()){
		if(!designSchemaId.length()){
			palette = m_stylesBasePalettes.first();

			return true;
		}
		else if (m_stylesBasePalettes.contains(designSchemaId)){
			palette = m_stylesBasePalettes[designSchemaId];

			return true;
		}
	}

	return false;
}


// reimplemented (IDesignTokenFileParser)

bool CDesignTokenFileParserComp::SetFile(const QByteArray& filePath)
{
	bool retVal = false;

	QFileInfo designTokenFileInfo(filePath);
	if (designTokenFileInfo.exists()){
		retVal = designTokenFileInfo.isReadable();
		if(retVal){
			m_designTokenFileInfo = designTokenFileInfo;
		}
	}
	else{
		qDebug(qPrintable(QString("Theme file doesn't exist: '%1'").arg(qPrintable(filePath))));

		return false;
	}

	return retVal;
}


bool CDesignTokenFileParserComp::ParseFile()
{
	QFile designTokenFile;

	if (m_designTokenFileInfo.isReadable()){
		designTokenFile.setFileName(m_designTokenFileInfo.absoluteFilePath());
	}
	else {
		if (m_designTokenFilePathAttrPtr.IsValid()){
			designTokenFile.setFileName(m_designTokenFilePathAttrPtr->GetPath());
		}
		else{
			SendErrorMessage(__LINE__, QString("Unable to open theme file : '%1'").arg(m_designTokenFileInfo.absoluteFilePath()));

			return false;
		}
	}

	if (!designTokenFile.open(QFile::ReadOnly)){
		SendErrorMessage(__LINE__, QString("Cannot read file '%1'; Error: '%2'").arg(designTokenFile.fileName(), designTokenFile.errorString()));

		return false;
	}

	QByteArray fileData = designTokenFile.readAll();
	QJsonParseError jsonParseError;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(fileData, &jsonParseError);

	QJsonObject designTokenObject = jsonDocument.object();

	if (designTokenObject.isEmpty()){
		SendErrorMessage(__LINE__, QString("Error occured '%1' at symbol: %2  during the processing file : '%3'")
					.arg(jsonParseError.errorString(), QString::number(jsonParseError.offset), designTokenFile.fileName()));

		return false;
	}

	m_templateIconColor = designTokenObject["TemplateIconColor"].toString().toUtf8();

	QJsonObject singleStyle = designTokenObject["Style"].toObject();
	QJsonArray designTokenStylesArray = designTokenObject["Styles"].toArray();
	designTokenStylesArray << singleStyle;

	if (designTokenStylesArray.isEmpty()){
		SendErrorMessage(__LINE__, QString("Error occured '%1' during the styles processing in the file :'%2'").arg(jsonParseError.errorString(), designTokenFile.fileName()));

		return false;
	}

	QVariantMap colorPaletteVariables = designTokenObject["ColorPalette"].toObject().toVariantMap();

	const QString designTokenFileFileBaseName(QFileInfo(designTokenFile).baseName());

	for (const QJsonValue& style : ::qAsConst(designTokenStylesArray)){
		QJsonObject styleEntry = style.toObject();
		QString styleName = styleEntry["Name"].toString();

		if (styleName.compare(designTokenFileFileBaseName, Qt::CaseInsensitive) != 0){
			SendErrorMessage(0, QString("The file name does not match the theme name '%1' VS '%2'").arg(designTokenFileFileBaseName).arg(styleName));

			return false;
		}

		if (styleName != designTokenFileFileBaseName){
			SendInfoMessage(0, QString("The file name and the theme name have different case '%1' VS '%2'").arg(designTokenFileFileBaseName).arg(styleName));
		}

		ReplaceColorNamesRecursivle(styleEntry, colorPaletteVariables);
		m_stylesBasePalettes.insert(styleName.toUtf8(), colorPaletteVariables);

		if (!styleName.length()){
			SendInfoMessage (0,"Skipping invalid style object");

			continue;
		}

		QJsonObject colorsObject = styleEntry["IconColor"].toObject();
		if (colorsObject.isEmpty()){
			SendInfoMessage (0,"Skipping empty IconColor object");
		}

		if (!m_templateIconColor.length()){
			m_templateIconColor = style["TemplateIconColor"].toString().toUtf8();
		}
		QVariantMap colorsMap = colorsObject.toVariantMap();
		m_iconColors.insert(styleName, colorsMap);
		m_designSchemaList.InsertItem(styleName.toUtf8(), styleName,"");

		m_stylesPalettes.insert(styleName, styleEntry.toVariantMap());

		ColorSchema schema;
		schema.id = styleName.toLocal8Bit();
		schema.stylePath = ":/Styles/" + styleName +  "/ImtColorStyle";
		schema.palette = CDesignTokenStyleUtils::GetPaletteFromMultiEntry(styleEntry);

		QJsonValue toolButtonGradientColorsObject = styleEntry["ToolButtonGradient"];
		if (toolButtonGradientColorsObject.isObject()){
			schema.toolButtonGradientColors = CDesignTokenStyleUtils::GetGradientColorsFromEntry(toolButtonGradientColorsObject);
		}

		QJsonValue pressedToolButtonGradientColorsObject = styleEntry["PressedToolButtonGradient"];
		if (pressedToolButtonGradientColorsObject.isObject()){
			schema.pressedToolButtonGradientColors = CDesignTokenStyleUtils::GetGradientColorsFromEntry(pressedToolButtonGradientColorsObject);
		}


		m_colorPalettes.insert(styleName, schema);

		imtbase::CCollectionInfo* themeFontsCollection = new imtbase::CCollectionInfo;
		QMap<QByteArray, QFont> fonts;

		CDesignTokenStyleUtils::GetFontsFromEntry(styleEntry["Fonts"], fonts, themeFontsCollection);
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
	if (!outputDir.exists()){
		bool createOutputDir = istd::CSystem::EnsurePathExists(outputDirPath.toUtf8());
		if (!createOutputDir){
			SendErrorMessage(__LINE__, QString("Cannot create output dir '%1'").arg(outputDirPath));

			return false;
		}
	}

	if (m_designTokenFileInfo.isReadable()){
		designTokenFile.setFileName(m_designTokenFileInfo.absoluteFilePath());
	}
	else{
		designTokenFile.setFileName(m_designTokenFilePathAttrPtr->GetPath());
	}

	if (!designTokenFile.open(QFile::ReadOnly)){
		SendErrorMessage(__LINE__, QString("Cannot read file '%1'; Error: '%2'").arg(designTokenFile.fileName(), designTokenFile.errorString()));

		return false;
	}

	QFileInfo designTokenFileInfo(designTokenFile.fileName());

	QJsonParseError jsonParseError;
	QJsonObject designTokenObject = QJsonDocument::fromJson(designTokenFile.readAll(), &jsonParseError).object();
	if(designTokenObject.isEmpty()){
		SendErrorMessage(__LINE__, QString("Error occured '%1' during the styles processing in the file :'%2'").arg(jsonParseError.errorString(), designTokenFile.fileName()));

		return false;
	}

	QJsonArray designTokenStylesArray = designTokenObject["Styles"].toArray();
	if(designTokenStylesArray.isEmpty()){
		SendErrorMessage(__LINE__, "Cannot parse Styles");

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
		bool openOutputFile = outputSingleThemeFile.open(QFile::WriteOnly);
		if(!openOutputFile){
			SendErrorMessage(__LINE__, QString("Cannot read file '%1'; Error: '%2'").arg(outputSingleThemeFile.fileName(), outputSingleThemeFile.errorString()));

			return false;
		}
		outputSingleThemeFile.write(QJsonDocument(designTokenObjectSplitted).toJson());
		outputSingleThemeFile.close();
	}

	return true;
}


const imtbase::ICollectionInfo& CDesignTokenFileParserComp::GetDesignSchemaList() const
{
	return m_designSchemaList;
}


bool CDesignTokenFileParserComp::GetColorPalette(const QByteArray& designSchemaId, ColorSchema& palette) const
{
	if(!designSchemaId.length() && m_colorPalettes.size()){
		palette = m_colorPalettes.first();
	}

	palette = m_colorPalettes[designSchemaId];

	return true;
}


QByteArray CDesignTokenFileParserComp::GetTemplateIconColor(const QByteArray&) const
{
	return m_templateIconColor;
}


QByteArray CDesignTokenFileParserComp::GetIconColor(const QByteArray& styleName, IconState iconState) const
{
	switch (iconState){
	case IS_NORMAL:
		return m_iconColors[styleName].toMap()["Normal"].toByteArray();
	case IS_OFF_NORMAL:
		return m_iconColors[styleName].toMap()[CDesignTokenStyleUtils::s_offNormalColorParamName].toByteArray();
	case IS_OFF_DISABLED:
		return m_iconColors[styleName].toMap()[CDesignTokenStyleUtils::s_offDisabledColorParamName].toByteArray();
	case IS_OFF_ACTIVE:
		return m_iconColors[styleName].toMap()[CDesignTokenStyleUtils::s_offActiveColorParamName].toByteArray();
	case IS_OFF_SELECTED:
		return m_iconColors[styleName].toMap()[CDesignTokenStyleUtils::s_offSelectedColorParamName].toByteArray();
	case IS_ON_NORMAL:
		return m_iconColors[styleName].toMap()[CDesignTokenStyleUtils::s_onNormalColorParamName].toByteArray();
	case IS_ON_DISABLED:
		return m_iconColors[styleName].toMap()[CDesignTokenStyleUtils::s_onDisabledColorParamName].toByteArray();
	case IS_ON_ACTIVE:
		return m_iconColors[styleName].toMap()[CDesignTokenStyleUtils::s_onActiveColorParamName].toByteArray();
	case IS_ON_SELECTED:
		return m_iconColors[styleName].toMap()[CDesignTokenStyleUtils::s_onSelectedColorParamName].toByteArray();
	}

	return QByteArray();
}


const imtbase::ICollectionInfo& CDesignTokenFileParserComp::GetFontList(const QByteArray& designSchemaId) const
{
	if (m_fonts.contains(designSchemaId) && m_fontsCollectionInfos[designSchemaId].GetPtr() != nullptr){
		const imtbase::ICollectionInfo& retVal = *m_fontsCollectionInfos[designSchemaId];

		return retVal;
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


// private static methods

void CDesignTokenFileParserComp::ReplaceColorNames(QJsonObject& json, const QVariantMap& variableMaps)
{
	for(QJsonObject::iterator jsonValue = json.begin(); jsonValue != json.end(); ++jsonValue){
		if(variableMaps.contains(jsonValue->toString())){
			*jsonValue = variableMaps[jsonValue->toString()].toString();
		}
	}
}


void CDesignTokenFileParserComp::ReplaceColorNamesRecursivle(QJsonObject& json, const QVariantMap& variableMaps)
{
	QStringList keys = json.keys();
	for (const QString& key : ::qAsConst(keys)){
		QJsonValue jsonValue = json[key];
		if(jsonValue.isObject()){
			QJsonObject jsonObject = jsonValue.toObject();
			ReplaceColorNames(jsonObject, variableMaps);
			json[key] = jsonObject;
		}
	}
}


} // namespace imtdesign


