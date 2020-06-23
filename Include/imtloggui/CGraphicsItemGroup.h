#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>


namespace imtloggui
{


class CGraphicsItemGroup: public QGraphicsItem
{
public:
	typedef QGraphicsItem BaseClass;

	CGraphicsItemGroup(QGraphicsItem* parent = nullptr);

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};


} // namespace imtloggui


