// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtGui/QFont>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtdesign/IDesignTokenFileParser.h>


namespace imtdesign
{


class CDesignTokenFileParserComp:
			public ilog::CLoggerComponentBase,
			public IDesignTokenFileParser
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenFileParserComp);
		I_REGISTER_INTERFACE(IDesignTokenFileParser);
	I_END_COMPONENT;

	// reimplemented (IDesignTokenFileParser)
	virtual bool ParseFile() override;
	virtual bool SplitFile(const QString& outputDirPath, const QString& projectName) override;
	virtual bool SetFile(const QByteArray& filePath) override;
	virtual QByteArray GetRawColor(const QByteArray& styleName, QPalette::ColorGroup group, QPalette::ColorRole role) const override;
	virtual bool GetStyleSheetColorPalette(const QByteArray& designSchemaId, QVariantMap& palette) const override;
	virtual bool GetBasePalette(const QByteArray& designSchemaId, QVariantMap& palette) const override;
	virtual QByteArrayList GetTemplateIconColorList(const QByteArray& styleName) const override;
	virtual QByteArray GetIconColor(const QByteArray& styleName, IconState iconState, const QByteArray& templateColor) const override;

	// reimplemented (IColorPaletteProvider)
	virtual const imtbase::ICollectionInfo& GetDesignSchemaList() const override;
	virtual bool GetColorPalette(const QByteArray& designSchemaId, ColorSchema& palette) const override;

	// reimplemented (IFontProvider)
	virtual const imtbase::ICollectionInfo& GetFontList(const QByteArray& designSchemaId) const override;
	virtual bool GetFont(const QByteArray& designSchemaId, const QByteArray& fontId, QFont& font) const override;

private:
	static void ReplaceColorNames(QJsonObject& json, const QVariantMap& variableMaps);
	static void ReplaceColorNamesRecursive(QJsonObject& json, const QVariantMap& variableMaps);

private:
	I_REF(ifile::IFileNameParam, m_designTokenFilePathAttrPtr);

	imtbase::CCollectionInfo m_designSchemaList;
	QByteArrayList m_templateIconColorList;
	QFileInfo m_designTokenFileInfo;

	imtbase::CCollectionInfo m_emptyCollectionInfo;

	QMap<QString, QVariantMap> m_stylesPalettes;
	QMap<QString, QVariantMap> m_stylesBasePalettes;
	QMap<QString, ColorSchema> m_colorPalettes;

	QMap<QString, istd::TSmartPtr<imtbase::ICollectionInfo>> m_fontsCollectionInfos;
	QMap<QString, QMap<QByteArray, QFont>> m_fonts;

	struct IconColor
	{
		IconColor(const QByteArray& aColor = QByteArray(),
				  const QByteArray& aStyle = QByteArray(),
				  const QVariantMap& aColorList = QVariantMap())
		{
			color = aColor;
			style = aStyle;
			colorList =  aColorList;
		}
		QByteArray color;
		QByteArray style;
		QVariantMap colorList;
		bool operator==(const IconColor& other) const
		{
			return color == other.color && style == other.style && colorList == other.colorList;
		}
	};
	QList<IconColor> m_iconColors;

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


} // namespace imtdesign


