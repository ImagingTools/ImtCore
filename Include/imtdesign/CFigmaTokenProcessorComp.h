#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtdesign/IDesignTokenProcessor.h>
#include <imtdesign/IDesignTokenArgumentsParser.h>


namespace imtdesign
{


class CFigmaTokenProcessorComp: public ilog::CLoggerComponentBase, public IDesignTokenProcessor
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFigmaTokenProcessorComp);
		I_REGISTER_INTERFACE(IDesignTokenProcessor);
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentsParser", "Arguments Parser", true, "IDesignTokenArgumentsParser")
	I_END_COMPONENT;


	// reimplemented (IDesignTokenProcessor)
	virtual int Exec() override;
	virtual QByteArray GetHelpString() const override;


private:
	/**
		\brief Convert Figma tokens JSON to .theme format
		\param figmaTokensJson The Figma tokens JSON object
		\param outputThemeJson The output .theme JSON object
		\return true on success, false on failure
	*/
	bool ConvertFigmaTokensToTheme(const QJsonObject& figmaTokensJson, QJsonObject& outputThemeJson) const;

	/**
		\brief Extract color palette from Figma tokens
		\param figmaTokensJson The Figma tokens JSON object
		\param colorPalette The output color palette map
		\return true on success, false on failure
	*/
	bool ExtractColorPalette(const QJsonObject& figmaTokensJson, QVariantMap& colorPalette) const;

	/**
		\brief Extract typography/fonts from Figma tokens
		\param figmaTokensJson The Figma tokens JSON object
		\param fonts The output fonts map
		\return true on success, false on failure
	*/
	bool ExtractFonts(const QJsonObject& figmaTokensJson, QVariantMap& fonts) const;

	/**
		\brief Resolve Figma token references (e.g., {color.primary})
		\param tokenValue The token value string
		\param figmaTokensJson The root Figma tokens JSON object
		\return The resolved value
	*/
	QString ResolveFigmaTokenReference(const QString& tokenValue, const QJsonObject& figmaTokensJson) const;

	/**
		\brief Get value from nested JSON path
		\param json The JSON object
		\param path The dot-separated path (e.g., "color.primary")
		\return The value at the path
	*/
	QJsonValue GetValueFromPath(const QJsonObject& json, const QString& path) const;


private:
	I_REF(IDesignTokenArgumentsParser, m_argumentParserCompPtr);
};


} // namespace imtdesign

