#pragma once


// ImtCore includes
#include <imtstyle/IDesignSchemaInfoProvider.h>


class QPalette;


namespace imtstyle
{


class IColorPaletteProvider: virtual public IDesignSchemaInfoProvider
{
public:
	struct GradientColors
	{
		QColor startColor;
		QColor endColor;
	};

	struct ColorSchema
	{
		ColorSchema()
		{
		}

		QByteArray id;
		GradientColors toolButtonGradientColors;
		GradientColors pressedToolButtonGradientColors;
		QPalette palette;
		QString stylePath;
		QColor toolButtonBorderColor;
	};

	virtual bool GetColorPalette(const QByteArray& designSchemaId, ColorSchema& palette) const = 0;
};


} // namespace imtstyle


