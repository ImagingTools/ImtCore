#pragma once


// Qt includes
#include <QtCore/QtCore>
#include <QtGui/QPalette>
#include <QtGui/QFont>


// ImtCore includes
#include <imtbase/CJsonHelper.h>
#include <imtstyle/IColorPaletteProvider.h>


namespace imtbase
{
	class ICollectionInfo;
	class CCollectionInfo;
}


namespace imtdesign
{


class CDesignTokenStyleUtils: imtbase::CJsonHelper
{
public:
	static QByteArrayList GetVariables(
				const QByteArray& data, 
				const QChar& variableBeginSymbol,
				const QChar& variableBeginSymbol2,
				const QChar& variableEndSymbol);
	static bool SetVariables(QByteArray& data,
				const QChar& variableBeginSymbol,
				const QChar& variableBeginSymbol2,
				const QChar& variableEndSymbol,
				const QMap<QByteArray, QByteArray>& variables);
	static bool SetVariables(QByteArray& data,
				const QChar& variableBeginSymbol,
				const QChar& variableBeginSymbol2,
				const QChar& variableEndSymbol,
				const QVariantMap& variables);
	/**
		sets variable that declared as dual i.e. Active.Background
	*/
	static bool SetVariablesFromDualVariable(QByteArray& data,
				const QChar& variableBeginSymbol,
				const QChar& variableBeginSymbol2,
				const QChar& variableEndSymbol,
				const QVariantMap& variables);

	static bool GetColorRoleGroup(const QString& name, QPalette::ColorGroup& group, QPalette::ColorRole& role);
	static QString GetColorName(QPalette::ColorGroup group, QPalette::ColorRole role);
	static QPalette GetPaletteFromMultiEntry(const QJsonValue& styleEntry);
	static QPalette GetPaletteFromEntry(const QJsonValue& paletteEntry);
	static imtstyle::IColorPaletteProvider::GradientColors GetGradientColorsFromEntry(const QJsonValue& gradientColorsEntry);
	static bool CreateColorFromGrb(const QString& rgbString, QColor& color);
	static bool GetFontHintingPreference(const QString& name, QFont::HintingPreference& hintingPreference);
	static bool GetFontCapitalization(const QString& name, QFont::Capitalization& capitalization);
	static bool GetFontStyle(const QString& name, QFont::Style& style);
	static bool GetFontStyleStrategy(const QString& name, QFont::StyleStrategy& styleStrategy);
	static bool GetFontStyleStrategy(const QStringList& name, QFont::StyleStrategy& styleStrategy);
	static bool GetFontStyleHint(const QString& name, QFont::StyleHint& styleHint);
	static bool GetFontSpacingType(const QString& name, QFont::SpacingType& spacingType);
	static bool GetFontsFromEntry(
				const QJsonValue& paletteEntry,
				QMap<QByteArray, QFont>& fonts,
				imtbase::CCollectionInfo* fontsCollectionInfos = nullptr);
	static bool CreateCssFont(QByteArray&output, const QFont& font);

public:
	constexpr static const char* s_offNormalColorParamName = "OffNormal";
	constexpr static const char* s_offDisabledColorParamName = "OffDisabled";
	constexpr static const char* s_offActiveColorParamName = "OffActive";
	constexpr static const char* s_offSelectedColorParamName = "OffSelected";
	constexpr static const char* s_onNormalColorParamName = "OnNormal";
	constexpr static const char* s_onDisabledColorParamName = "OnDisabled";
	constexpr static const char* s_onActiveColorParamName = "OnActive";
	constexpr static const char* s_onSelectedColorParamName = "OnSelected";

	static const QMap<QString, QPalette::ColorGroup> s_colorGroupNamesMap;
	static const QMap<QString, QPalette::ColorRole> s_colorRolesNamesMap;
	static const QMap<QString, QFont::HintingPreference> s_fontHintingPreferenceNamesMap;
	static const QMap<QString, QFont::Capitalization> s_fontCapitalizationNamesMap;
	static const QMap<QString, QFont::Style> s_fontStyleNamesMap;
	static const QMap<QString, QFont::StyleStrategy> s_fontStyleStrategyNamesMap;
	static const QMap<QString, QFont::StyleHint> s_fontStyleHintNamesMap;
	static const QMap<QString, QFont::SpacingType> s_fontSpacingTypeNamesMap;

private:
	static bool SetVariable_(
				QByteArray& data,
				const QChar& variableBeginSymbol,
				const QChar& variableBeginSymbol2,
				const QChar& variableEndSymbol,
				const QMap<QByteArray, QByteArray> variables);
	static bool SetVariablesFromDualVariable_(
				QByteArray& data,
				const QChar& variableBeginSymbol,
				const QChar& variableBeginSymbol2,
				const QChar& variableEndSymbol,
				const QVariantMap& variables);
	static bool FindColorEnrty(const QByteArray& name, const QVariantMap& variantMap, QVariant& output, const QString& appendix = QString());
};


} // namespace imtdesign

