#pragma once


// Qt includes
#include <QtGui/QColor>
#include <QtGui/QPalette>

// ImtCore includes
#include <imtstyle/IDesignSchemaInfoProvider.h>


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


