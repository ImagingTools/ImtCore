#pragma once


// Qt includes
#include <QtCore/QtCore>
#include <QtGui/QPalette>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtstyle/IDesignTokenFileParser.h>


namespace imtstyle
{


class CDesignTokenFileParserComp: public ilog::CLoggerComponentBase, public IDesignTokenFileParser
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenFileParserComp);
		I_REGISTER_INTERFACE(IDesignTokenFileParser);
	I_END_COMPONENT;

	// reimplemented (IDesignTokenFileParser)
	bool ParseFile() override;
	bool SplitFile(const QString& outputDirPath, const QString& projectName) override;
	bool SetFile(const QByteArray& filePath) override;

    QByteArray GetRawColor(const QByteArray& styleName, QPalette::ColorGroup group, QPalette::ColorRole role) const override;	
	bool GetStyleSheetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const override;

	QByteArray GetTemplateIconColor(const QByteArray& styleName) const override;
    QByteArray GetNormalColor(const QByteArray& styleName) const override;
	QByteArray GetOffNormalColor(const QByteArray& styleName) const override;
	QByteArray GetOffDisabledColor(const QByteArray& styleName) const override;
	QByteArray GetOffActiveColor(const QByteArray& styleName) const override;
	QByteArray GetOffSelectedColor(const QByteArray& styleName) const override;
	QByteArray GetOnNormalColor(const QByteArray& styleName) const override;
	QByteArray GetOnDisabledColor(const QByteArray& styleName) const override;
	QByteArray GetOnActiveColor(const QByteArray& styleName) const override;
	QByteArray GetOnSelectedColor(const QByteArray& styleName) const override;

	// reimplemented (IColorPaletteProvider)
	const imtbase::ICollectionInfo& GetDesignSchemaList() const override;
	bool GetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const override;

	// reimplemented (IFontProvider)
	virtual const imtbase::ICollectionInfo& GetFontList(const QByteArray& designSchemaId) const override;
	virtual bool GetFont(const QByteArray& designSchemaId, const QByteArray& fontId, QFont& font) const override;

private:
	static void ReplaceColorNames(QJsonObject& json, const QVariantMap& variableMaps);
	static void ReplaceColorNamesRecursivle(QJsonObject& json, const QVariantMap& variableMaps);

private:
	I_REF(ifile::IFileNameParam, m_designTokenFilePathAttrPtr);

	imtbase::CCollectionInfo m_designSchemaList;
	QByteArray m_templateIconColor;
	QFileInfo m_designTokenFileInfo;

	imtbase::CCollectionInfo m_emptyCollectionInfo;

	QVariantMap m_iconColors;
	QMap<QString, QPalette> m_stylesPalettes;
	QMap<QString, QPalette> m_colorPalettes;

	QMap<QString, istd::TSmartPtr<imtbase::ICollectionInfo>> m_fontsCollectionInfos;
	QMap<QString, QMap<QByteArray, QFont>> m_fonts;

    struct RawColor
    {
       RawColor(QPalette::ColorGroup group, QPalette::ColorRole role, const QByteArray& value)
       {
           this->group = group;
           this->role = role;
           this->value = value;
       }
       QPalette::ColorGroup group;
       QPalette::ColorRole role;
       QByteArray value;
    };
    QMultiMap<QByteArray, RawColor> m_styleSheetColors;

};


} // namespace imtstyle


