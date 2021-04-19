#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtWidgets/QProxyStyle>
#include <QtWidgets/QStyleOption>


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
		DS_INVALID = -1,

		DS_LIGHT = 0,
		DS_DARK,

		DS_LAST
	};

	enum StyleType
	{
		ST_IMAGINGTOOLS,
		ST_FLAT
	};

	static CImtStyle& GetInstance();

	int GetDesignSchemaCount();
	DesignSchema GetDesignSchema() const;
	void SetDesignSchema(DesignSchema designSchema);

	StyleType GetStyleType() const;
	void SetStyleType(StyleType styleType);

	DesignSchema GetDesignSchemaFromIndex(int index);

	// reimplemented (QStyle)
	virtual void polish(QWidget* widgetPtr) override;
	virtual void drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget = nullptr) const override;
	virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
	virtual QRect subElementRect(SubElement subElement, const QStyleOption *option, const QWidget *widget = nullptr) const override;
	virtual QSize sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget = nullptr) const override;

protected:
	virtual void DrawImagingToolsToolButton(
				const QStyleOptionToolButton* optionPtr,
				QPainter* painter,
				const QWidget* widget = nullptr) const;

private:
	void EnsureStyleSheetApplied(bool force = false) const;

private:
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

		GradientColors toolButtonGradientColors;
		GradientColors pressedToolButtonGradientColors;
		QString styleSheetPath;
		QPalette palette;
	};

	typedef QMap<DesignSchema, ColorSchema> ColorSchemaMap;

	ColorSchemaMap m_colorSchemaMap;

	DesignSchema m_designSchema;
	StyleType m_styleType;

	mutable bool m_wasStyleSheetInitialized;

private:
	CImtStyle();
	Q_DISABLE_COPY(CImtStyle);
};


} // namespace imtwidgets


