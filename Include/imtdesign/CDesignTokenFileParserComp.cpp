// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesign/CDesignTokenFileParserComp.h>


// ACF includes
#include <istd/CSystem.h>

// ImtCore includes
#include <imtdesign/CDesignTokenStyleUtils.h>


namespace imtdesign
{


QByteArray CDesignTokenFileParserComp::GetRawColor(
			const QByteArray& /*styleName*/,
			QPalette::ColorGroup group,
			QPalette::ColorRole role) const
{
	for(const RawColor& color: ::std::as_const(m_styleSheetColors)){
		if (color.group == group && color.role == role){
			return color.value;
		}
	}

	return QByteArray();
}


bool CDesignTokenFileParserComp::GetStyleSheetColorPalette(const QByteArray& designSchemaId, QVariantMap& palette) const
{
	if(designSchemaId.isEmpty() &&! m_styleSheetColors.isEmpty()){
		palette = m_stylesPalettes.first();
	}
	else{
		palette = m_stylesPalettes[designSchemaId];
	}

	return true;
}


bool CDesignTokenFileParserComp::GetBasePalette(const QByteArray& designSchemaId, QVariantMap& palette) const
{
	if(!m_stylesBasePalettes.isEmpty()){
		if(designSchemaId.isEmpty()){
			palette = m_stylesBasePalettes.first();

			return true;
		}
		if (m_stylesBasePalettes.contains(designSchemaId)){
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
		qDebug() << (qPrintable(QString("Theme file doesn't exist: '%1'").arg(qPrintable(filePath))));

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
			if (!m_designTokenFileInfo.absoluteFilePath().isEmpty()){
				SendErrorMessage(__LINE__, QString("Unable to open theme file : '%1'").arg(m_designTokenFileInfo.absoluteFilePath()));
			}

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

	QJsonObject singleStyle = designTokenObject["Style"].toObject();
	QJsonArray designTokenStylesArray = designTokenObject["Styles"].toArray();
	designTokenStylesArray << singleStyle;

	if (designTokenStylesArray.isEmpty()){
		SendErrorMessage(__LINE__, QString("Error occured '%1' during the styles processing in the file :'%2'").arg(jsonParseError.errorString(), designTokenFile.fileName()));

		return false;
	}

	QVariantMap colorPaletteVariables = designTokenObject["ColorPalette"].toObject().toVariantMap();

	const QString designTokenFileFileBaseName(QFileInfo(designTokenFile).baseName());

	for (const auto& style : ::std::as_const(designTokenStylesArray)){
		QJsonObject styleEntry = style.toObject();
		QString styleName = styleEntry["Name"].toString();

		if (styleName.compare(designTokenFileFileBaseName, Qt::CaseInsensitive) != 0){
			SendErrorMessage(0, QString("The file name does not match the theme name '%1' VS '%2'").arg(designTokenFileFileBaseName).arg(styleName));

			return false;
		}

		if (styleName != designTokenFileFileBaseName){
			SendVerboseMessage(QString("The file name and the theme name have different case '%1' VS '%2'").arg(designTokenFileFileBaseName).arg(styleName));
		}

		ReplaceColorNamesRecursive(styleEntry, colorPaletteVariables);
		m_stylesBasePalettes.insert(styleName.toUtf8(), colorPaletteVariables);

		if (styleName.isEmpty()){
			SendInfoMessage (0,"Skipping invalid style object");

			continue;
		}


		QJsonArray iconTemplateList = styleEntry["IconTemplateList"].toArray();
		if (iconTemplateList.isEmpty()){
			QJsonObject object;
			object.insert(QStringLiteral("IconColor"), styleEntry["IconColor"]);
			object.insert(QStringLiteral("TemplateIconColor"), styleEntry["TemplateIconColor"]);
			iconTemplateList << object;
		}

		for (const QJsonValue& iconColorsEntry: std::as_const(iconTemplateList)){
			if (iconColorsEntry["TemplateIconColor"].toString().isEmpty()){
				Q_ASSERT_X(false, "Parse color entry", "Template color is not set");

				continue;
			}

			const QByteArray templateIconColor = iconColorsEntry["TemplateIconColor"].toString().toUtf8();
			m_templateIconColorList << templateIconColor;

			QJsonObject colorsObject = iconColorsEntry["IconColor"].toObject();
			if (colorsObject.isEmpty()){
				SendWarningMessage (0,"Skipping empty IconColor object");
				Q_ASSERT_X(false, "Parse color entry", "Icon color is not set");

				continue;
			}

			QVariantMap colorsMap = colorsObject.toVariantMap();
			m_iconColors << IconColor(templateIconColor, styleName.toUtf8(), colorsMap);
		}

		m_designSchemaList.InsertItem(styleName.toUtf8(), styleName,"");
		m_stylesPalettes.insert(styleName, styleEntry.toVariantMap());

		ColorSchema schema;
		schema.id = styleName.toLocal8Bit();
		schema.stylePath = ":/" + styleName +  "/Styles/ImtColorStyle";
		schema.palette = CDesignTokenStyleUtils::GetPaletteFromMultiEntry(styleEntry);

		QJsonValue toolButtonGradientColorsObject = styleEntry["ToolButtonGradient"];
		if (toolButtonGradientColorsObject.isObject()){
			schema.toolButtonGradientColors = CDesignTokenStyleUtils::GetGradientColorsFromEntry(toolButtonGradientColorsObject);
		}

		QJsonValue pressedToolButtonGradientColorsObject = styleEntry["PressedToolButtonGradient"];
		if (pressedToolButtonGradientColorsObject.isObject()){
			schema.pressedToolButtonGradientColors = CDesignTokenStyleUtils::GetGradientColorsFromEntry(pressedToolButtonGradientColorsObject);
		}

		QJsonValue toolButtonBorderColorObject = styleEntry["ToolButtonBorderColor"];
		if (toolButtonBorderColorObject.isString()){
			QString value = toolButtonBorderColorObject.toString();

			schema.toolButtonBorderColor = QColor(value);
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
		SendErrorMessage(__LINE__, "Cannot parse <Styles> variable in the theme file");

		return false;
	}

	QJsonObject designTokenObjectSplitted = designTokenObject;

	for (const auto& style: ::std::as_const(designTokenStylesArray)){
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
	if(designSchemaId.isEmpty() && !m_colorPalettes.isEmpty()){
		palette = m_colorPalettes.first();
	}

	palette = m_colorPalettes[designSchemaId];

	return true;
}


QByteArrayList CDesignTokenFileParserComp::GetTemplateIconColorList(const QByteArray& /* styleName */) const
{
	return m_templateIconColorList;
}


QByteArray CDesignTokenFileParserComp::GetIconColor(const QByteArray& styleName, IconState iconState, const QByteArray& templateColor) const
{
	QList<IconColor>::const_iterator coloramtsMap = m_iconColors.cend();
	for (QList<IconColor>::const_iterator colorantListIterator = m_iconColors.cbegin(); colorantListIterator != m_iconColors.cend(); ++colorantListIterator){
		if (colorantListIterator->style == styleName && colorantListIterator->color == templateColor){
			coloramtsMap = colorantListIterator;

			break;
		}
	}

	if (coloramtsMap == m_iconColors.cend()){
		Q_ASSERT_X(false, "GetIconColor", "Unexpected request");

		return QByteArray();
	}

	switch (iconState){
	case IS_NORMAL:
		return coloramtsMap->colorList["Normal"].toByteArray();
	case IS_OFF_NORMAL:
		return coloramtsMap->colorList[CDesignTokenStyleUtils::s_offNormalColorParamName].toByteArray();
	case IS_OFF_DISABLED:
		return coloramtsMap->colorList[CDesignTokenStyleUtils::s_offDisabledColorParamName].toByteArray();
	case IS_OFF_ACTIVE:
		return coloramtsMap->colorList[CDesignTokenStyleUtils::s_offActiveColorParamName].toByteArray();
	case IS_OFF_SELECTED:
		return coloramtsMap->colorList[CDesignTokenStyleUtils::s_offSelectedColorParamName].toByteArray();
	case IS_ON_NORMAL:
		return coloramtsMap->colorList[CDesignTokenStyleUtils::s_onNormalColorParamName].toByteArray();
	case IS_ON_DISABLED:
		return coloramtsMap->colorList[CDesignTokenStyleUtils::s_onDisabledColorParamName].toByteArray();
	case IS_ON_ACTIVE:
		return coloramtsMap->colorList[CDesignTokenStyleUtils::s_onActiveColorParamName].toByteArray();
	case IS_ON_SELECTED:
		return coloramtsMap->colorList[CDesignTokenStyleUtils::s_onSelectedColorParamName].toByteArray();
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


void CDesignTokenFileParserComp::ReplaceColorNamesRecursive(QJsonObject& json, const QVariantMap& variableMaps)
{
	QStringList keys = json.keys();
	for (const QString& key : ::std::as_const(keys)){
		QJsonValue jsonValue = json[key];
		if(jsonValue.isObject()){
			QJsonObject jsonObject = jsonValue.toObject();
			ReplaceColorNames(jsonObject, variableMaps);
			json[key] = jsonObject;
		}
		else if (jsonValue.isArray()){
			QJsonArray array = jsonValue.toArray();
			for (int i = 0; i < array.count(); ++i){
				QJsonValue entryValue = array[i];
				if (entryValue.isObject()){
					QJsonObject entryObject = entryValue.toObject();
					ReplaceColorNamesRecursive(entryObject, variableMaps);
					array[i] = entryObject;
				}
			}
			json[key] = array;
		}
	}
}


} // namespace imtdesign


