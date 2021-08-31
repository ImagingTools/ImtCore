#pragma once


// Qt includes
#include <QtCore/QtCore>
#include <QtGui/QPalette>

// ACF includes

// ImtCore includes
#include <imtstyle/IColorPaletteProvider.h>


namespace imtstyle
{

class IDesignTokenFileParser: virtual public IColorPaletteProvider
{
public:
	virtual bool SetFile(const QByteArray& filePath) = 0;
	virtual bool ParseFile() = 0;
	virtual bool SplitFile(const QString& outputDirPath, const QString& projectName) = 0;

	virtual bool GetColorRoleGroup(const QString& name, QPalette::ColorGroup& group,  QPalette::ColorRole& role) const = 0;
	virtual QString GetColorName(QPalette::ColorGroup group, QPalette::ColorRole role) const = 0;


	virtual bool GetStyleSheetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const = 0;
	virtual QByteArray GetRawColor(const QByteArray& styleName, QPalette::ColorGroup group, QPalette::ColorRole role) const = 0;

	virtual QByteArray GetTemplateIconColor(const QByteArray& styleName = QByteArray()) const = 0;
	virtual QByteArray GetNormalColor(const QByteArray& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOffNormalColor(const QByteArray& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOffDisabledColor(const QByteArray& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOffActiveColor(const QByteArray& styleName = QByteArray()) const  = 0;
	virtual QByteArray GetOffSelectedColor(const QByteArray& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOnNormalColor(const QByteArray& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOnDisabledColor(const QByteArray& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOnActiveColor(const QByteArray& styleName = QByteArray()) const = 0;
	virtual QByteArray GetOnSelectedColor(const QByteArray& styleName = QByteArray()) const = 0;


};


} // namespace imtstyle
