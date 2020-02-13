#pragma once


// Qt includes
#include <QtWidgets/QProxyStyle>


namespace imtwidgets
{


class CImtStyle: public QProxyStyle
{
public:
	typedef QProxyStyle BaseClass;

	virtual void drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget = nullptr) const override;
	virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
	virtual QRect subElementRect(SubElement subElement, const QStyleOption *option, const QWidget *widget = nullptr) const override;
};


} // namespace imtwidgets


