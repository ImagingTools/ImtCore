#pragma once


// Qt includes
#include <QtCore/QtCore>
#include <QtGui/QPalette>

// ACF includes

// ImtCore includes
#include <imtstyle/IColorPaletteProvider.h>
#include <imtstyle/IFontProvider.h>


namespace imtdesign
{

class IDesignTokenFileParser: virtual public imtstyle::IColorPaletteProvider, virtual public imtstyle::IFontProvider
{
public:
	enum IconState
	{
		IS_NORMAL = 0,
		IS_OFF_NORMAL,
		IS_OFF_DISABLED,
		IS_OFF_ACTIVE,
		IS_OFF_SELECTED,
		IS_ON_NORMAL,
		IS_ON_DISABLED,
		IS_ON_ACTIVE,
		IS_ON_SELECTED
	};

	virtual bool SetFile(const QByteArray& filePath) = 0;
	virtual bool ParseFile() = 0;
	virtual bool SplitFile(const QString& outputDirPath, const QString& projectName) = 0;

	virtual bool GetBasePalette(const QByteArray& designSchemaId, QVariantMap& palette) const = 0;
	virtual bool GetStyleSheetColorPalette(const QByteArray& designSchemaId, QVariantMap& palette) const = 0;
	virtual QByteArray GetRawColor(const QByteArray& styleName, QPalette::ColorGroup group, QPalette::ColorRole role) const = 0;

	virtual QByteArray GetTemplateIconColor(const QByteArray& styleName) const = 0;
	virtual QByteArray GetIconColor(const QByteArray& styleName, IconState iconState) const = 0;
};


} // namespace imtdesign
