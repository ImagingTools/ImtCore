#include <imtdesign/CDesignTokenStyleUtils.h>



#ifdef Q_OS_WIN
#include "windows.h"
#elif defined(Q_OS_LINUX)
#include <unistd.h>
#endif

// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtdesign/IDesignTokenFileParser.h>


namespace imtdesign
{


const QMap<QString, QPalette::ColorGroup> CDesignTokenStyleUtils::s_colorGroupNamesMap = {
	std::make_pair("Active", QPalette::ColorGroup::Active),
	std::make_pair("Disabled", QPalette::ColorGroup::Disabled),
	std::make_pair("Inactive", QPalette::ColorGroup::Inactive),
	std::make_pair("NColorGroups", QPalette::ColorGroup::NColorGroups),
	std::make_pair("Current", QPalette::ColorGroup::Current),
	std::make_pair("All", QPalette::ColorGroup::All),
	std::make_pair("Normal", QPalette::ColorGroup::Normal)
};


const QMap<QString, QPalette::ColorRole>CDesignTokenStyleUtils::s_colorRolesNamesMap = {
	std::make_pair("WindowText", QPalette::ColorRole::WindowText),
	std::make_pair("Button", QPalette::ColorRole::Button),
	std::make_pair("Light", QPalette::ColorRole::Light),
	std::make_pair("Midlight", QPalette::ColorRole::Midlight),
	std::make_pair("Dark", QPalette::ColorRole::Dark),
	std::make_pair("Mid", QPalette::ColorRole::Mid),
	std::make_pair("Text", QPalette::ColorRole::Text),
	std::make_pair("BrightText", QPalette::ColorRole::BrightText),
	std::make_pair("ButtonText", QPalette::ColorRole::ButtonText),
	std::make_pair("Base", QPalette::ColorRole::Base),
	std::make_pair("Window", QPalette::ColorRole::Window),
	std::make_pair("Shadow", QPalette::ColorRole::Shadow),
	std::make_pair("Highlight", QPalette::ColorRole::Highlight),
	std::make_pair("HighlightedText", QPalette::ColorRole::HighlightedText),
	std::make_pair("Link", QPalette::ColorRole::Link),
	std::make_pair("LinkVisited", QPalette::ColorRole::LinkVisited),
	std::make_pair("AlternateBase", QPalette::ColorRole::AlternateBase),
	std::make_pair("NoRole", QPalette::ColorRole::NoRole),
	std::make_pair("ToolTipBase", QPalette::ColorRole::ToolTipBase),
	std::make_pair("ToolTipText", QPalette::ColorRole::ToolTipText),
	std::make_pair("PlaceholderText", QPalette::ColorRole::PlaceholderText),
	std::make_pair("Foreground", QPalette::ColorRole::WindowText),
	std::make_pair("Background", QPalette::ColorRole::Window),
	std::make_pair("PlaceholderText", QPalette::ColorRole::PlaceholderText)
};


const QMap<QString, QFont::HintingPreference> CDesignTokenStyleUtils::s_fontHintingPreferenceNamesMap = {
	std::make_pair("PreferDefaultHinting", QFont::HintingPreference::PreferDefaultHinting),
	std::make_pair("PreferFullHinting", QFont::HintingPreference::PreferFullHinting),
	std::make_pair("PreferNoHinting", QFont::HintingPreference::PreferNoHinting),
	std::make_pair("PreferVerticalHinting", QFont::HintingPreference::PreferVerticalHinting)
};


const QMap<QString, QFont::Capitalization> CDesignTokenStyleUtils::s_fontCapitalizationNamesMap = {
	std::make_pair("AllLowercase", QFont::Capitalization::AllLowercase),
	std::make_pair("AllUppercase", QFont::Capitalization::AllUppercase),
	std::make_pair("Capitalize", QFont::Capitalization::Capitalize),
	std::make_pair("MixedCase", QFont::Capitalization::MixedCase),
	std::make_pair("SmallCaps", QFont::Capitalization::SmallCaps)
};


const QMap<QString, QFont::Style> CDesignTokenStyleUtils::s_fontStyleNamesMap = {
	std::make_pair("StyleItalic", QFont::Style::StyleItalic),
	std::make_pair("StyleNormal", QFont::Style::StyleNormal),
	std::make_pair("StyleOblique", QFont::Style::StyleOblique)
};


const QMap<QString, QFont::StyleStrategy> CDesignTokenStyleUtils::s_fontStyleStrategyNamesMap = {
	std::make_pair("ForceIntegerMetrics",QFont::StyleStrategy(0x0400)), //QFont::StyleStrategy::ForceIntegerMetrics
	std::make_pair("ForceOutline", QFont::StyleStrategy::ForceOutline),
	std::make_pair("NoAntialias", QFont::StyleStrategy::NoAntialias),
	std::make_pair("NoFontMerging", QFont::StyleStrategy::NoFontMerging),
	std::make_pair("NoSubpixelAntialias", QFont::StyleStrategy::NoSubpixelAntialias),
	std::make_pair("OpenGLCompatible", QFont::StyleStrategy(0x0200)), //QFont::StyleStrategy::OpenGLCompatible
	std::make_pair("PreferAntialias", QFont::StyleStrategy::PreferAntialias),
	std::make_pair("PreferBitmap", QFont::StyleStrategy::PreferBitmap),
	std::make_pair("PreferDefault", QFont::StyleStrategy::PreferDefault),
	std::make_pair("PreferDevice", QFont::StyleStrategy::PreferDevice),
	std::make_pair("PreferMatch", QFont::StyleStrategy::PreferMatch),
	std::make_pair("PreferNoShaping", QFont::StyleStrategy::PreferNoShaping),
	std::make_pair("PreferOutline", QFont::StyleStrategy::PreferOutline),
	std::make_pair("PreferQuality", QFont::StyleStrategy::PreferQuality)
};


const QMap<QString, QFont::StyleHint> CDesignTokenStyleUtils::s_fontStyleHintNamesMap = {
	std::make_pair("AnyStyle", QFont::StyleHint::AnyStyle),
	std::make_pair("Courier", QFont::StyleHint::Courier),
	std::make_pair("Cursive", QFont::StyleHint::Cursive),
	std::make_pair("Decorative", QFont::StyleHint::Decorative),
	std::make_pair("Fantasy", QFont::StyleHint::Fantasy),
	std::make_pair("Helvetica", QFont::StyleHint::Helvetica),
	std::make_pair("Monospace", QFont::StyleHint::Monospace),
	std::make_pair("OldEnglish", QFont::StyleHint::OldEnglish),
	std::make_pair("SansSerif", QFont::StyleHint::SansSerif),
	std::make_pair("Serif", QFont::StyleHint::Serif),
	std::make_pair("System", QFont::StyleHint::System),
	std::make_pair("Times", QFont::StyleHint::Times),
	std::make_pair("TypeWriter", QFont::StyleHint::TypeWriter)
};


const QMap<QString, QFont::SpacingType> CDesignTokenStyleUtils::s_fontSpacingTypeNamesMap = {
	std::make_pair("AbsoluteSpacing", QFont::SpacingType::AbsoluteSpacing),
	std::make_pair("PercentageSpacing", QFont::SpacingType::PercentageSpacing)
};


QString CDesignTokenStyleUtils::GetColorName(QPalette::ColorGroup group, QPalette::ColorRole role)
{
	return CDesignTokenStyleUtils::s_colorGroupNamesMap.key(group) + CDesignTokenStyleUtils::s_colorRolesNamesMap.key(role);
}


QPalette CDesignTokenStyleUtils::GetPaletteFromMultiEntry(const QJsonValue& styleEntry)
{
	QPalette palette;

	QJsonObject jsonObject = styleEntry.toObject();
	QJsonValue v;
	for(QJsonObject::const_iterator jsonValue = jsonObject.constBegin(); jsonValue != jsonObject.constEnd(); ++jsonValue){

		for (const QString& groupName: s_colorGroupNamesMap.keys()){
			if (jsonValue->isObject() && jsonValue.key().startsWith(groupName)){
				QJsonObject colorGroupObject = jsonValue->toObject();
				QPalette::ColorGroup colorRoleGroup = s_colorGroupNamesMap[groupName];
				for (QJsonObject::const_iterator jsonColorValue = colorGroupObject.constBegin(); jsonColorValue != colorGroupObject.constEnd(); ++jsonColorValue){
					if(s_colorRolesNamesMap.contains(jsonColorValue.key())){
						QColor color;
						if (!CDesignTokenStyleUtils::CreateColorFromGrb(jsonColorValue->toString(), color)){
							color.setNamedColor(jsonColorValue->toString());
						}
						if (color.isValid()){
							palette.setColor(colorRoleGroup, s_colorRolesNamesMap[jsonColorValue.key()], color);
						}
					}
				}
			}
		}
	}
	return palette;
}


bool SetVariableColor(QByteArray& data, const QPalette& palette)
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

			if(CDesignTokenStyleUtils::GetColorRoleGroup(colorName, colorGroup, colorRole)){
				QColor color = palette.color(colorGroup, colorRole);
				QByteArray colorHex;
				if(color.name() == "#000000"){
					//						colorHex = m_designTokenFileParserAttrPtr->GetRawColor(m_currentTheme, colorGroup, colorRole);
					if(!colorHex.length()){
						colorHex = "#000000";
					}
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

QByteArrayList CDesignTokenStyleUtils::GetVariables(const QByteArray& data, const QChar& variableBeginSymbol, const QChar& variableBeginSymbol2, const QChar& variableEndSymbol)
{
	QByteArrayList retval;

	/// points to 'beginSymbol' symbol
	int indexOfBeginVariable = -1;
	/// points to 'endSymbol' symbol
	int indexOfEndVariable = -1;

	for(int i = 0; i < data.length(); ++i){

		QChar symbol(data.at(i));
		if(symbol == variableBeginSymbol && i+1 < data.length()){

			QChar nextSymbol = data.at(i+1);
			if(nextSymbol == variableBeginSymbol2){
				indexOfBeginVariable = i;

				for(int j = i+1; j < data.length(); ++j){
					QChar endSymbol = data.at(j);

					if(endSymbol == variableEndSymbol){
						indexOfEndVariable = j;
						break;
					}
				}
			}
		}
		if(indexOfBeginVariable > -1 && indexOfEndVariable > indexOfBeginVariable){

			int lengthOfVariable = indexOfEndVariable - indexOfBeginVariable - 2;
			QByteArray variableName = data.mid(indexOfBeginVariable+2, lengthOfVariable);
			retval << variableName;
		}
	}
	return retval;
}

bool CDesignTokenStyleUtils::SetVariables(QByteArray& data, const QChar& variableBeginSymbol, const QChar& variableBeginSymbol2, const QChar& variableEndSymbol, const QMap<QByteArray, QByteArray>& variables)
{
	bool retval = SetVariable_(data, variableBeginSymbol, variableBeginSymbol2, variableEndSymbol, variables);
	while(SetVariable_(data, variableBeginSymbol, variableBeginSymbol2, variableEndSymbol, variables));
	return retval;
}

bool CDesignTokenStyleUtils::SetVariables(QByteArray& data, const QChar& variableBeginSymbol, const QChar& variableBeginSymbol2, const QChar& variableEndSymbol, const QVariantMap& variables)
{
	QMap<QByteArray, QByteArray> _variables;
	for(auto variable = variables.cbegin(); variable != variables.cend(); ++variable){
		auto k = variable.key();
		auto ks = k.toUtf8();
		_variables.insert(ks, variable->toByteArray());
	}
	bool retval = SetVariable_(data, variableBeginSymbol, variableBeginSymbol2, variableEndSymbol, _variables);
	while(SetVariable_(data, variableBeginSymbol, variableBeginSymbol2, variableEndSymbol, _variables));
	return retval;
}

bool CDesignTokenStyleUtils::SetVariablesFromDualVariable(QByteArray& data, const QChar& variableBeginSymbol, const QChar& variableBeginSymbol2, const QChar& variableEndSymbol, const QVariantMap& variables)
{
	bool retval = SetVariablesFromDualVariable_(data, variableBeginSymbol, variableBeginSymbol2, variableEndSymbol, variables);
	while(SetVariablesFromDualVariable_(data, variableBeginSymbol, variableBeginSymbol2, variableEndSymbol, variables));
	return retval;
}


bool CDesignTokenStyleUtils::GetColorRoleGroup(const QString& name, QPalette::ColorGroup& group, QPalette::ColorRole& role)
{
	const QStringList& groupNames = CDesignTokenStyleUtils::s_colorGroupNamesMap.keys();

	for(const QString& groupName: groupNames){

		QRegularExpression groupRegEx(QString("^" + groupName), QRegularExpression::PatternOption::CaseInsensitiveOption);
		QRegularExpressionMatchIterator globalMatch = groupRegEx.globalMatch(name);

		if(globalMatch.hasNext()){

			QRegularExpressionMatch groupRegExMatch;
			groupRegExMatch = globalMatch.next();

			if(!s_colorGroupNamesMap.contains(groupName)) {
				return false;
			}

			group = s_colorGroupNamesMap[groupName];

			QString roleName = name;
			roleName.remove(groupRegExMatch.capturedStart(), groupRegExMatch.capturedLength());

			while(roleName.startsWith('.')){
				roleName.remove(0,1);
			}

			if(!s_colorRolesNamesMap.contains(roleName)) {
				return false;
			}

			role = s_colorRolesNamesMap[roleName];
			return true;
		}
	}

	if(s_colorRolesNamesMap.contains(name)){
		group = QPalette::All;
		role = s_colorRolesNamesMap[name];
		return true;
	}

	return false;
}


QPalette CDesignTokenStyleUtils::GetPaletteFromEntry(const QJsonValue& paletteEntry)
{
	QPalette palette;

	const QJsonObject& paletteEntryObject = paletteEntry.toObject();

	for(QJsonObject::const_iterator value = paletteEntryObject.constBegin(); value != paletteEntryObject.constEnd(); ++value){
		QPalette::ColorGroup colorGroup;
		QPalette::ColorRole colorRole;
		CDesignTokenStyleUtils::GetColorRoleGroup(value.key(), colorGroup, colorRole);

		QColor color;
		if (!CDesignTokenStyleUtils::CreateColorFromGrb(value->toString(), color)){
			color.setNamedColor(value->toString());
		}
		if(color.isValid()){
			palette.setColor(colorGroup, colorRole, color);
		}
	}
	return palette;
}


bool CDesignTokenStyleUtils::CreateColorFromGrb(const QString& rgbString, QColor& color)
{
	int indexOfBegin = rgbString.indexOf("rgb(");
	if(indexOfBegin >= 0){
		bool rFill = false, gFill = false;
		QString r,g,b;

		for(int i = indexOfBegin + 4; i < rgbString.length(); ++i){
			QChar symbol = rgbString[i];
			if(symbol.isDigit()){
				if(!rFill){
					r += symbol;
				}
				else if(!gFill){
					g += symbol;
				}
				else {
					b += symbol;
				}
			}
			else{
				if(!rFill){
					rFill = true;
				}
				else if(!gFill){
					gFill = true;
				}
				else{
					int iR = r.toInt(), iG = g.toInt(), iB = b.toInt();
					color = QColor::fromRgb(iR, iG, iB);
					return color.isValid();
				}
			}
		}
	}
	return false;
}


bool CDesignTokenStyleUtils::GetFontHintingPreference(const QString& name, QFont::HintingPreference& hintingPreference)
{
	if(s_fontHintingPreferenceNamesMap.contains(name)){
		hintingPreference = s_fontHintingPreferenceNamesMap[name];
		return true;
	}
	return false;
}


bool CDesignTokenStyleUtils::GetFontCapitalization(const QString& name, QFont::Capitalization& capitalization)
{
	if(s_fontCapitalizationNamesMap.contains(name)){
		capitalization = s_fontCapitalizationNamesMap[name];
		return true;
	}
	return false;
}


bool CDesignTokenStyleUtils::GetFontStyle(const QString& name, QFont::Style& style)
{
	if(s_fontStyleNamesMap.contains(name)){
		style = s_fontStyleNamesMap[name];
		return true;
	}
	return false;
}


bool CDesignTokenStyleUtils::GetFontStyleStrategy(const QString& name, QFont::StyleStrategy& styleStrategy)
{
	if(s_fontStyleStrategyNamesMap.contains(name)){
		styleStrategy = s_fontStyleStrategyNamesMap[name];
		return true;
	}
	return false;
}


bool CDesignTokenStyleUtils::GetFontStyleStrategy(const QStringList& names, QFont::StyleStrategy& styleStrategy)
{
	int output = 0x0000;
	for(const QString& name: names){
		if(!s_fontStyleStrategyNamesMap.contains(name)){
			return false;
		}
		output |= s_fontStyleStrategyNamesMap[name];
	}
	styleStrategy =  QFont::StyleStrategy(output);
	return true;
}


bool CDesignTokenStyleUtils::GetFontStyleHint(const QString& name, QFont::StyleHint& styleHint)
{
	if(s_fontStyleHintNamesMap.contains(name)){
		styleHint = s_fontStyleHintNamesMap[name];
		return true;
	}
	return false;
}


bool CDesignTokenStyleUtils::GetFontSpacingType(const QString& name, QFont::SpacingType& spacingType)
{
	if(s_fontSpacingTypeNamesMap.contains(name)){
		spacingType = s_fontSpacingTypeNamesMap[name];
		return true;
	}
	return false;
}


bool CDesignTokenStyleUtils::GetFontsFromEntry(const QJsonValue& fontsEntry, QMap<QByteArray, QFont>& fonts, imtbase::CCollectionInfo* fontsCollectionInfo)
{
	QJsonObject jsonObject = fontsEntry.toObject();

	for(QJsonObject::const_iterator fontEntry = jsonObject.constBegin(); fontEntry != jsonObject.constEnd(); ++ fontEntry){

		QJsonObject fontObject = fontEntry->toObject();

		QFont font;

		bool Bold = false;
		bool FixedPitch = false;
		bool Italic = false;
		bool Kerning = false;
		bool Overline = false;
		bool StrikeOut = false;
		bool Underline = false;

		int pixelSize = 32;
		int pointSize = 16;
		int Stretch = 100;
		double pointSizeF = 8.5;
		int weight = 0;
		double wordSpacing = 1;

		QFont::HintingPreference hintingPreference = QFont::PreferDefaultHinting;
		QString s_hintingPreference;
		QFont::Capitalization caps = QFont::MixedCase;
		QString s_caps;
		QFont::Style style = QFont::StyleNormal;
		QString s_style;

		QString family;
		QString styleName;
		QStringList families;

		QFont::StyleStrategy styleStrategyS = QFont::PreferDefault;
		QStringList sl_styleStrategyS;

		QFont::StyleHint styleHint = QFont::AnyStyle;
		QString s_styleHint;
		QFont::StyleStrategy styleStrategy = QFont::PreferDefault;
		QStringList sl_styleStrategy;

		QFont::SpacingType spacingType = QFont::PercentageSpacing;
		QString s_spacingType;
		double letterSpacing = 100.;

		//------------------------------------------bool
		if(GetBoolValue(fontObject, "Bold", Bold)){
			font.setBold(Bold);
		}

		if(GetBoolValue(fontObject, "FixedPitch", FixedPitch)){
			font.setFixedPitch(FixedPitch);
		}

		if(GetBoolValue(fontObject, "Italic", Italic)){
			font.setItalic(Italic);
		}

		if(GetBoolValue(fontObject, "Kerning", Kerning)){
			font.setKerning(Kerning);
		}

		if(GetBoolValue(fontObject, "Overline", Overline)){
			font.setOverline(Overline);
		}

		if(GetBoolValue(fontObject, "StrikeOut", StrikeOut)){
			font.setStrikeOut(StrikeOut);
		}

		if(GetBoolValue(fontObject, "Underline", Underline)){
			font.setUnderline(Underline);
		}

		//------------------------------------------number
		if(GetIntValue(fontObject, "Stretch", Stretch)){
			font.setStretch(Stretch);
		}

		if(GetIntValue(fontObject, "PixelSize", pixelSize)){
			font.setPixelSize(pixelSize);
		}

		if(GetDoubleValue(fontObject, "PointSizeF", pointSizeF)){
			font.setPointSizeF(pointSizeF);
		}

		if(GetIntValue(fontObject, "PointSize", pointSize)){
			font.setPointSize(pointSize);
		}

		if(GetIntValue(fontObject, "Weight", weight)){
			if(Bold){
				font.setWeight(QFont::Weight::Bold);
			}
			else{
				font.setWeight(QFont::Weight(weight));
			}
		}

		if(GetDoubleValue(fontObject, "WordSpacing", wordSpacing)){
			font.setWordSpacing(wordSpacing);
		}

		//------------------------------------------oneOf
		if(GetStringValue(fontObject, "HintingPreference", s_hintingPreference) &&
				GetFontHintingPreference(s_hintingPreference, hintingPreference)){
			font.setHintingPreference(hintingPreference);
		}

		if(GetStringValue(fontObject, "Capitalization", s_caps) &&
				GetFontCapitalization(s_hintingPreference, caps)){
			font.setCapitalization(caps);
		}

		if(GetStringValue(fontObject, "Style", s_style) &&
				GetFontStyle(s_hintingPreference, style)){
			font.setStyle(style);
		}

		//------------------------------------------String
		if((GetStringValue(fontObject, "Family", family) || GetStringValue(fontObject, "Families", family)) && family.length()){
			font.setFamily(family);
		}

		if(GetStringValue(fontObject, "StyleName", styleName) && styleName.length()){
			font.setStyleName(styleName);
		}

		//------------------------------------------StringList
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
		if((GetStringListValue(fontObject, "Family", families) || GetStringListValue(fontObject, "Families", families)) && families.size()){
			font.setFamilies(families);
		}
#endif

		//------------------------------------------ManyOf
		if(GetStringListValue(fontObject, "StyleStrategy", sl_styleStrategyS) &&
				GetFontStyleStrategy(sl_styleStrategyS, styleStrategyS)){
			font.setStyleStrategy(styleStrategyS);
		}

		//------------------------------------------Combo
		QJsonObject comboObject;
		if(GetObjectValue(fontObject, "StyleHint", comboObject) &&
				GetStringValue(comboObject, "StyleHint", s_styleHint) &&
				GetFontStyleHint(s_styleHint, styleHint) &&
				GetStringListValue(comboObject, "StyleStrategy", sl_styleStrategy) &&
				GetFontStyleStrategy(sl_styleStrategy, styleStrategy)){
			font.setStyleHint(styleHint, styleStrategy);
		}

		if(GetObjectValue(fontObject, "LetterSpacing", comboObject) &&
				GetStringValue(comboObject, "SpacingType", s_spacingType) &&
				GetFontSpacingType(s_spacingType, spacingType) &&
				GetDoubleValue(comboObject, "Spacing", letterSpacing)
				){
			font.setLetterSpacing(spacingType, letterSpacing);
		}

		if(fontsCollectionInfo != nullptr){
			fontsCollectionInfo->InsertItem(fontEntry.key().toUtf8(), fontEntry.key(), "");
		}
		fonts.insert(fontEntry.key().toUtf8(), font);
	}

	return true;
}


bool CDesignTokenStyleUtils::CreateCssFont(QByteArray& output, const QFont& font)
{
	if(font.pointSize() > 0){
		output.append("font-size: ").append(QByteArray::number(font.pointSize())).append("pt;\n");
	}
	else if(font.pointSizeF() > 0){
		output.append("font-size: ").append(QByteArray::number(font.pointSizeF())).append("pt;\n");
	}
	else if(font.pixelSize() > 0){
		output.append("font-size: ").append(QByteArray::number(font.pixelSize())).append("px;\n");
	}

	QStringList fontFamilies;

	#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
		fontFamilies = font.families();
	#endif

	QByteArray fontFamilyString = "font-family: ";
	if(font.family().length()){
		fontFamilyString.append('"').append(font.family().toUtf8()).append('"');
	}
	else if(fontFamilies.count()){
		QStringList tempFonts = fontFamilies;
		for(QStringList::iterator fontIter = tempFonts.begin(); fontIter != tempFonts.end(); ++fontIter){
			fontIter->append('"');
			fontIter->prepend('"');
		}
	
		fontFamilyString.append(tempFonts.join(", ").toUtf8());
	}

	if(font.styleHint() != QFont::AnyStyle){
		if(!fontFamilyString.trimmed().endsWith(':')){
			fontFamilyString.append(", ");
			fontFamilyString.append(s_fontStyleHintNamesMap.key(font.styleHint()).toUtf8());
		}
	}

	if(!fontFamilyString.trimmed().endsWith(':')){
		fontFamilyString.append(";\n");
		output += fontFamilyString;
	}

	QByteArray fontStyleString = s_fontStyleNamesMap.key(font.style()).toUtf8();
	fontStyleString = fontStyleString.toLower();
	fontStyleString.replace("style", "");
	fontStyleString.prepend("font-style: ").append(";\n");
	output += fontStyleString;

	if(font.weight() > 0) {
		output += "font-weight: ";
		output += QByteArray::number(font.weight()).append(";\n");
	}

	if(font.stretch() > 0){
		output += "font-stretch: ";
		output += QByteArray::number(font.stretch()).append(";\n");

	}

	QStringList fontDecorations;
	if(font.underline()){
		fontDecorations << "underline";

	}
	if(font.overline()){
		fontDecorations << "overline";

	}
	if(font.strikeOut()){
		fontDecorations << "line-through";

	}
	if(fontDecorations.size()){
		output += "text-decoration: ";
		output += fontDecorations.join(", ").toUtf8();
		output += ";\n";
	}

	if(font.capitalization() != QFont::MixedCase && font.capitalization() != QFont::SmallCaps){
		QString transformString = s_fontCapitalizationNamesMap.key(font.capitalization());
		transformString = transformString.toLower();
		transformString.replace("all", "");
		output += "text-transform: ";
		output += transformString.toUtf8();
		output += ";\n";

	}
	else if (font.capitalization() == QFont::SmallCaps){
		output += "font-variant: small-caps;\n";
	}

	return true;
}


bool CDesignTokenStyleUtils::SetVariable_(QByteArray& data, const QChar& variableBeginSymbol, const QChar& variableBeginSymbol2, const QChar& variableEndSymbol, const QMap<QByteArray, QByteArray> variables)
{
	QMap<QString, QVariant> _variables;
	for (const QByteArray& variablesKey : variables.keys()){
		_variables[variablesKey] = variables[variablesKey];
	}

	/// points to 'beginSymbol' symbol
	int indexOfBeginVariable = -1;

	/// points to 'endSymbol' symbol
	int indexOfEndVariable = -1;

	for(int i = 0; i < data.length(); ++i){
		QChar symbol(data.at(i));
		if(symbol == variableBeginSymbol && i+1 < data.length()){

			QChar nextSymbol = data.at(i+1);
			if(nextSymbol == variableBeginSymbol2){
				indexOfBeginVariable = i;

				for(int j = i+1; j < data.length(); ++j){
					QChar endSymbol = data.at(j);

					if(endSymbol == variableEndSymbol){
						indexOfEndVariable = j;
						break;
					}
				}
			}
		}

		if(indexOfBeginVariable > -1 && indexOfEndVariable > indexOfBeginVariable){
			int lengthOfVariable = indexOfEndVariable - indexOfBeginVariable - 2;

			QByteArray variableName = data.mid(indexOfBeginVariable + 2, lengthOfVariable).toUpper();
			QVariant newColor;
			bool hasNewColor = CDesignTokenStyleUtils::FindColorEnrty(variableName, _variables, newColor);
			if (hasNewColor && newColor.isValid()){
				data.replace(indexOfBeginVariable, lengthOfVariable + 3, newColor.toByteArray());
				return true;
			}
		}
	}
	return false;
}

bool CDesignTokenStyleUtils::SetVariablesFromDualVariable_(QByteArray& data, const QChar& variableBeginSymbol, const QChar& variableBeginSymbol2, const QChar& variableEndSymbol, const QVariantMap& variables)
{
	/// points to 'beginSymbol' symbol
	int indexOfBeginVariable = -1;

	/// points to 'endSymbol' symbol
	int indexOfEndVariable = -1;

	for(int i = 0; i < data.length(); ++i){
		QChar symbol(data.at(i));
		if(symbol == variableBeginSymbol && i+1 < data.length()){

			QChar nextSymbol = data.at(i+1);
			if(nextSymbol == variableBeginSymbol2){
				indexOfBeginVariable = i;

				for(int j = i+1; j < data.length(); ++j){
					QChar endSymbol = data.at(j);

					if(endSymbol == variableEndSymbol){
						indexOfEndVariable = j;
						break;
					}
				}
			}
		}

		if(indexOfBeginVariable > -1 && indexOfEndVariable > indexOfBeginVariable){
			int lengthOfVariable = indexOfEndVariable - indexOfBeginVariable - 2;

			QByteArray variableName = data.mid(indexOfBeginVariable + 2, lengthOfVariable);
			QByteArrayList variablesParts = variableName.split('.');
			if(variablesParts.size() > 1){
				QByteArray varGroup = variablesParts[0];
				QByteArray varRole = variablesParts[1];


				QVariant currentVariableColorroles;
				QVariant currentVariableColor;
				bool hasColorGroup = CDesignTokenStyleUtils::FindColorEnrty(varGroup, variables, currentVariableColorroles, QString::fromUtf8("Colors"));
				bool hasColorRole = CDesignTokenStyleUtils::FindColorEnrty(varRole, currentVariableColorroles.toMap(),currentVariableColor);
				if (hasColorGroup && hasColorRole && currentVariableColor.isValid()){
					data.replace(indexOfBeginVariable, lengthOfVariable + 3, currentVariableColor.toString().toUtf8());
					return true;
				}
			}
		}
	}
	return false;
}

bool CDesignTokenStyleUtils::FindColorEnrty(const QByteArray& name, const QVariantMap& variantMap, QVariant& output, const QString& appendix)
{
	bool retval = false;

	QStringList keys = variantMap.keys();

	for (const QString& key: ::qAsConst(keys)){
		QString clearKey = key;
		if(appendix.length() && key.endsWith(appendix, Qt::CaseInsensitive)){
			clearKey = clearKey.chopped(appendix.length());
		}
		clearKey = clearKey.toUpper();
		if(clearKey == name.toUpper()){
			output = variantMap[key];
			return true;
		}
	}
	return retval;
}


} // namespace imtdesign


