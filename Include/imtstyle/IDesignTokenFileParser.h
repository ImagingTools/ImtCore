#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes

// ImtCore includes
#include <imtstyle/IColorPaletteProvider.h>

namespace imtstyle
{

class IDesignTokenFileParser: public IColorPaletteProvider
{
public:
	virtual bool SetFile(const QByteArray& filePath) = 0;
	virtual bool ParseFile() = 0;

	virtual QByteArray GetTemplateIconColor(const QByteArray& styleName = QByteArray()) const = 0;
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
