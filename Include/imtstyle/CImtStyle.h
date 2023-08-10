#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtWidgets/QProxyStyle>
#include <QtWidgets/QStyleOption>

// ImtCore includes
#include <imtstyle/IColorPaletteProvider.h>


namespace imtstyle
{


class CImtStyle: public QProxyStyle
{
	Q_OBJECT
public:
	typedef QProxyStyle BaseClass;

	enum StyleType
	{
		ST_IMAGINGTOOLS,
		ST_FLAT
	};

	static CImtStyle* GetInstance();

	StyleType GetStyleType() const;
	void SetStyleType(StyleType styleType);
	QByteArray GetActiveDesignSchemaId() const;
	void SetActiveDesignSchema(const IColorPaletteProvider::ColorSchema& schema);

	// reimplemented (QStyle)
	virtual void polish(QWidget* widgetPtr) override;
	virtual void drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget = nullptr) const override;
	virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
	virtual QRect subElementRect(SubElement subElement, const QStyleOption *option, const QWidget *widget = nullptr) const override;
	virtual QSize sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget = nullptr) const override;

public:
	class DesignSchemaSetterBlocker
	{
	public:
		explicit DesignSchemaSetterBlocker(CImtStyle& parent);
		~DesignSchemaSetterBlocker();

	private:
		CImtStyle& m_parent;
	};

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

	IColorPaletteProvider::ColorSchema m_activeColorSchema;

	StyleType m_styleType;

	mutable bool m_wasStyleSheetInitialized;

private:
	CImtStyle();
	Q_DISABLE_COPY(CImtStyle);

	static CImtStyle* m_instancePtr;

	int m_blockerCount;
};


} // namespace imtstyle


