#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItemGroup>
#include <QtWidgets/QGraphicsLayoutItem>

// ImtCore includes
//#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


class CEventGroupItem: public QGraphicsLayoutItem, public QGraphicsItemGroup
{
public:
	typedef QGraphicsItemGroup BaseClass;

	CEventGroupItem(QGraphicsItem* parent = nullptr);

	void SetBackgroundColor(const QColor& color);
	void SetRect(QRectF rect);
	void ViewPortChanged();

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	// reimplemented (QGraphicsLayoutItem)
	virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

private:
	QColor m_bgColor;
	QRectF m_rect;
};


} // namespace imtloggui


