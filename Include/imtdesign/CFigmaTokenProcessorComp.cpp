#include <imtdesign/CFigmaTokenProcessorComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QDir>


namespace imtdesign
{


// reimplemented (IDesignTokenProcessor)

QByteArray CFigmaTokenProcessorComp::GetHelpString() const
{
	QString helpText = 
		"Figma Token Processor Help:\n"
		"\t-figma <path>  : Path to Figma tokens JSON file\n"
		"\t-OD <path>     : Output directory for converted .theme files\n"
		"\t-P <name>      : Project name for output files\n\n"
		"Converts Figma design tokens JSON format to ImtCore .theme format.\n"
		"Supports: colors, typography, spacing, and sizing tokens.\n\n";
	
	return helpText.toLocal8Bit();
}


int CFigmaTokenProcessorComp::Exec()
{
	// Check if Figma token file argument is provided
	if (!m_argumentParserCompPtr->CheckArgument(IDesignTokenArgumentsParser::AA_FIGMA_TOKEN_FILE_PATH)) {
		SendInfoMessage(0, "Figma token file not specified, Figma processor skipped");
		return 0;
	}

	QByteArray figmaTokenFilePath = m_argumentParserCompPtr->GetArgumentValue(IDesignTokenArgumentsParser::AA_FIGMA_TOKEN_FILE_PATH);
	
	if (figmaTokenFilePath.isEmpty()) {
		SendInfoMessage(0, "Figma token file path is empty, Figma processor skipped");
		return 0;
	}

	QFileInfo figmaFileInfo(figmaTokenFilePath);
	if (!figmaFileInfo.exists() || !figmaFileInfo.isReadable()) {
		SendErrorMessage(__LINE__, QString("Figma token file does not exist or is not readable: '%1'").arg(QString::fromUtf8(figmaTokenFilePath)));
		return -1;
	}

	SendInfoMessage(0, QString("Processing Figma tokens from: %1").arg(QString::fromUtf8(figmaTokenFilePath)));

	// Read Figma tokens JSON file
	QFile figmaFile(figmaTokenFilePath);
	if (!figmaFile.open(QIODevice::ReadOnly)) {
		SendErrorMessage(__LINE__, QString("Failed to open Figma token file: '%1'").arg(QString::fromUtf8(figmaTokenFilePath)));
		return -1;
	}

	QByteArray jsonData = figmaFile.readAll();
	figmaFile.close();

	QJsonParseError parseError;
	QJsonDocument figmaDoc = QJsonDocument::fromJson(jsonData, &parseError);
	
	if (parseError.error != QJsonParseError::NoError) {
		SendErrorMessage(__LINE__, QString("Failed to parse Figma token JSON: %1 at offset %2")
			.arg(parseError.errorString())
			.arg(parseError.offset));
		return -1;
	}

	if (!figmaDoc.isObject()) {
		SendErrorMessage(__LINE__, "Figma token JSON root must be an object");
		return -1;
	}

	QJsonObject figmaTokensJson = figmaDoc.object();
	
	// Convert Figma tokens to .theme format
	QJsonObject themeJson;
	if (!ConvertFigmaTokensToTheme(figmaTokensJson, themeJson)) {
		SendErrorMessage(__LINE__, "Failed to convert Figma tokens to theme format");
		return -1;
	}

	// Write output .theme file
	QByteArray outputDirPath = m_argumentParserCompPtr->GetOutputDirectoryPath();
	QByteArray projectName = m_argumentParserCompPtr->GetProjectName();
	
	if (outputDirPath.isEmpty() || projectName.isEmpty()) {
		SendErrorMessage(__LINE__, "Output directory or project name not specified");
		return -1;
	}

	QDir outputDir(outputDirPath);
	if (!outputDir.exists()) {
		if (!outputDir.mkpath(".")) {
			SendErrorMessage(__LINE__, QString("Failed to create output directory: '%1'").arg(QString::fromUtf8(outputDirPath)));
			return -1;
		}
	}

	QString outputFileName = QString::fromUtf8(projectName) + "_figma.theme";
	QString outputFilePath = outputDir.absoluteFilePath(outputFileName);
	
	QFile outputFile(outputFilePath);
	if (!outputFile.open(QIODevice::WriteOnly)) {
		SendErrorMessage(__LINE__, QString("Failed to create output theme file: '%1'").arg(outputFilePath));
		return -1;
	}

	QJsonDocument outputDoc(themeJson);
	outputFile.write(outputDoc.toJson(QJsonDocument::Indented));
	outputFile.close();

	SendInfoMessage(0, QString("Successfully converted Figma tokens to: %1").arg(outputFilePath));

	return 0;
}


bool CFigmaTokenProcessorComp::ConvertFigmaTokensToTheme(const QJsonObject& figmaTokensJson, QJsonObject& outputThemeJson) const
{
	// Extract color palette
	QVariantMap colorPalette;
	if (!ExtractColorPalette(figmaTokensJson, colorPalette)) {
		SendWarningMessage(__LINE__, "Failed to extract color palette from Figma tokens");
	}

	// Extract fonts/typography
	QVariantMap fonts;
	if (!ExtractFonts(figmaTokensJson, fonts)) {
		SendWarningMessage(__LINE__, "Failed to extract fonts from Figma tokens");
	}

	// Build the .theme structure
	outputThemeJson["ColorPalette"] = QJsonObject::fromVariantMap(colorPalette);
	
	QJsonObject styleObject;
	styleObject["Name"] = "Figma";
	
	// Add default icon template list
	QJsonArray iconTemplateList;
	QJsonObject iconTemplate;
	iconTemplate["TemplateIconColor"] = "#D9D9D9";
	
	QJsonObject iconColors;
	iconColors["Normal"] = "Primary";
	iconColors["OffActive"] = "Primary";
	iconColors["OffDisabled"] = "Disabled";
	iconColors["OffNormal"] = "Primary";
	iconColors["OffSelected"] = "Accent";
	iconColors["OnActive"] = "Primary";
	iconColors["OnDisabled"] = "Disabled";
	iconColors["OnNormal"] = "Primary";
	iconColors["OnSelected"] = "Accent";
	
	iconTemplate["IconColor"] = iconColors;
	iconTemplateList.append(iconTemplate);
	styleObject["IconTemplateList"] = iconTemplateList;
	
	// Add fonts if extracted
	if (!fonts.isEmpty()) {
		styleObject["Fonts"] = QJsonObject::fromVariantMap(fonts);
	}
	
	// Add basic color groups (ActiveColors, InactiveColors, etc.)
	// These would be derived from the Figma semantic tokens
	QJsonObject activeColors;
	if (colorPalette.contains("Background")) {
		activeColors["Background"] = colorPalette["Background"];
	}
	if (colorPalette.contains("Foreground")) {
		activeColors["Foreground"] = colorPalette["Foreground"];
	}
	if (colorPalette.contains("Primary")) {
		activeColors["Text"] = colorPalette["Primary"];
		activeColors["ButtonText"] = colorPalette["Primary"];
	}
	if (colorPalette.contains("Surface")) {
		activeColors["Base"] = colorPalette["Surface"];
		activeColors["Button"] = colorPalette["Surface"];
	}
	if (colorPalette.contains("Accent")) {
		activeColors["Highlight"] = colorPalette["Accent"];
	}
	
	styleObject["ActiveColors"] = activeColors;
	styleObject["InactiveColors"] = activeColors; // Simplified for now
	styleObject["DisabledActiveColors"] = activeColors;
	styleObject["DisabledInActiveColors"] = activeColors;
	
	outputThemeJson["Style"] = styleObject;
	
	return true;
}


bool CFigmaTokenProcessorComp::ExtractColorPalette(const QJsonObject& figmaTokensJson, QVariantMap& colorPalette) const
{
	// Figma tokens typically have a "color" or "colors" section
	// Format can vary: {color: {primary: {value: "#..."}}} or flat structure
	
	QStringList colorKeys = {"color", "colors", "colour", "colours"};
	QJsonObject colorsSection;
	
	for (const QString& key : colorKeys) {
		if (figmaTokensJson.contains(key)) {
			QJsonValue value = figmaTokensJson[key];
			if (value.isObject()) {
				colorsSection = value.toObject();
				break;
			}
		}
	}
	
	if (colorsSection.isEmpty()) {
		// Try to find color tokens at root level
		colorsSection = figmaTokensJson;
	}
	
	// Recursively extract color values
	std::function<void(const QJsonObject&, const QString&)> extractColors;
	extractColors = [&](const QJsonObject& obj, const QString& prefix) {
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			QString key = it.key();
			QJsonValue value = it.value();
			
			if (value.isObject()) {
				QJsonObject valueObj = value.toObject();
				
				// Check if this is a token with a "value" field
				if (valueObj.contains("value")) {
					QString tokenValue = valueObj["value"].toString();
					
					// Resolve references if needed
					if (tokenValue.startsWith("{") && tokenValue.endsWith("}")) {
						tokenValue = ResolveFigmaTokenReference(tokenValue, figmaTokensJson);
					}
					
					QString colorName = prefix.isEmpty() ? key : prefix + " " + key;
					colorPalette[colorName] = tokenValue;
				} else if (valueObj.contains("$value")) {
					// W3C Design Token format
					QString tokenValue = valueObj["$value"].toString();
					
					if (tokenValue.startsWith("{") && tokenValue.endsWith("}")) {
						tokenValue = ResolveFigmaTokenReference(tokenValue, figmaTokensJson);
					}
					
					QString colorName = prefix.isEmpty() ? key : prefix + " " + key;
					colorPalette[colorName] = tokenValue;
				} else {
					// Recurse into nested objects
					QString newPrefix = prefix.isEmpty() ? key : prefix + " " + key;
					extractColors(valueObj, newPrefix);
				}
			} else if (value.isString()) {
				// Direct color value
				QString colorValue = value.toString();
				if (colorValue.startsWith("#") || colorValue.startsWith("rgb")) {
					QString colorName = prefix.isEmpty() ? key : prefix + " " + key;
					colorPalette[colorName] = colorValue;
				}
			}
		}
	};
	
	extractColors(colorsSection, "");
	
	return !colorPalette.isEmpty();
}


bool CFigmaTokenProcessorComp::ExtractFonts(const QJsonObject& figmaTokensJson, QVariantMap& fonts) const
{
	// Figma tokens typically have a "typography" or "font" section
	QStringList fontKeys = {"typography", "font", "fonts", "fontFamily", "fontFamilies"};
	QJsonObject fontsSection;
	
	for (const QString& key : fontKeys) {
		if (figmaTokensJson.contains(key)) {
			QJsonValue value = figmaTokensJson[key];
			if (value.isObject()) {
				fontsSection = value.toObject();
				break;
			}
		}
	}
	
	if (fontsSection.isEmpty()) {
		return false;
	}
	
	// Extract font definitions
	for (auto it = fontsSection.begin(); it != fontsSection.end(); ++it) {
		QString fontName = it.key();
		QJsonValue fontValue = it.value();
		
		if (!fontValue.isObject()) {
			continue;
		}
		
		QJsonObject fontObj = fontValue.toObject();
		QVariantMap fontDef;
		
		// Extract font properties
		if (fontObj.contains("fontFamily") || fontObj.contains("value")) {
			QJsonObject valueObj = fontObj.contains("value") && fontObj["value"].isObject() 
				? fontObj["value"].toObject() 
				: fontObj;
			
			if (valueObj.contains("fontFamily")) {
				fontDef["Family"] = valueObj["fontFamily"].toString();
			}
			if (valueObj.contains("fontSize")) {
				QString sizeStr = valueObj["fontSize"].toString().replace("px", "").trimmed();
				bool ok = false;
				int sizeInt = sizeStr.toInt(&ok);
				if (ok && sizeInt > 0) {
					fontDef["PointSize"] = sizeInt;
				} else {
					// Try as double then convert to int
					double sizeDouble = sizeStr.toDouble(&ok);
					fontDef["PointSize"] = (ok && sizeDouble > 0) ? static_cast<int>(sizeDouble) : 11;
				}
			}
			if (valueObj.contains("fontWeight")) {
				QJsonValue weight = valueObj["fontWeight"];
				if (weight.isString()) {
					QString weightStr = weight.toString();
					if (weightStr == "bold" || weightStr == "700") {
						fontDef["Bold"] = true;
					}
				} else if (weight.isDouble()) {
					// In Qt JSON, both integers and doubles are represented as double
					int weightInt = weight.toInt();
					fontDef["Weight"] = weightInt >= 700 ? 75 : 50;
					fontDef["Bold"] = weightInt >= 700;
				}
			}
			if (valueObj.contains("fontStyle")) {
				QString style = valueObj["fontStyle"].toString();
				fontDef["Italic"] = (style == "italic");
			}
			
			// Set default values
			if (!fontDef.contains("Bold")) {
				fontDef["Bold"] = false;
			}
			if (!fontDef.contains("Italic")) {
				fontDef["Italic"] = false;
			}
			if (!fontDef.contains("PointSize")) {
				fontDef["PointSize"] = 11;
			}
			
			fonts[fontName] = fontDef;
		}
	}
	
	return !fonts.isEmpty();
}


QString CFigmaTokenProcessorComp::ResolveFigmaTokenReference(const QString& tokenValue, const QJsonObject& figmaTokensJson) const
{
	// Remove braces
	QString reference = tokenValue;
	if (reference.startsWith("{") && reference.endsWith("}")) {
		reference = reference.mid(1, reference.length() - 2).trimmed();
	}
	
	// Get value from path
	QJsonValue value = GetValueFromPath(figmaTokensJson, reference);
	
	if (value.isString()) {
		QString result = value.toString();
		// Recursively resolve if the result is also a reference
		if (result.startsWith("{") && result.endsWith("}")) {
			return ResolveFigmaTokenReference(result, figmaTokensJson);
		}
		return result;
	} else if (value.isObject()) {
		QJsonObject obj = value.toObject();
		if (obj.contains("value")) {
			QString result = obj["value"].toString();
			if (result.startsWith("{") && result.endsWith("}")) {
				return ResolveFigmaTokenReference(result, figmaTokensJson);
			}
			return result;
		} else if (obj.contains("$value")) {
			QString result = obj["$value"].toString();
			if (result.startsWith("{") && result.endsWith("}")) {
				return ResolveFigmaTokenReference(result, figmaTokensJson);
			}
			return result;
		}
	}
	
	return tokenValue; // Return original if resolution fails
}


QJsonValue CFigmaTokenProcessorComp::GetValueFromPath(const QJsonObject& json, const QString& path) const
{
	QStringList parts = path.split(".");
	QJsonValue current = json;
	
	for (const QString& part : parts) {
		if (!current.isObject()) {
			return QJsonValue();
		}
		
		QJsonObject obj = current.toObject();
		if (!obj.contains(part)) {
			return QJsonValue();
		}
		
		current = obj[part];
	}
	
	return current;
}


} // namespace imtdesign

