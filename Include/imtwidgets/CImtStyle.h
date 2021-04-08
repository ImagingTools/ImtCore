#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtWidgets/QProxyStyle>


namespace imtwidgets
{


class CImtStyle: public QProxyStyle
{
public:
	typedef QProxyStyle BaseClass;

	/**
		Design shema for the application style.
	*/
	enum DesignSchema
	{
		DS_LIGHT,
		DS_DARK
	};

	CImtStyle();

	void SetDesignSchema(DesignSchema designSchema);

	// reimplemented (QStyle)
	virtual void drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget = nullptr) const override;
	virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
	virtual QRect subElementRect(SubElement subElement, const QStyleOption *option, const QWidget *widget = nullptr) const override;
	virtual QSize sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget = nullptr) const override;

private:
	struct GradientColors
	{
		QColor startColor;
		QColor endColor;
	};

	struct ColorSchema
	{
		GradientColors toolButtonGradientColors;
		GradientColors pressedToolButtonGradientColors;
	};

	typedef QMap<DesignSchema, ColorSchema> ColorSchemaMap;

	ColorSchemaMap m_colorSchemaMap;

	DesignSchema m_designSchema;
};


} // namespace imtwidgets


