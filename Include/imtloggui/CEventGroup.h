#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


class CEventGroup: public QGraphicsItem
{
public:
	typedef QGraphicsItem BaseClass;

	CEventGroup(QGraphicsItem *parent = nullptr);

	void setColor(const QColor& color);
	void setGeometry(const QRectF &geometry);
	void update();

	// reimplemented (QGraphicsItem)
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
	QRectF m_geometryRect;
	QRectF m_boundingRect;

	QColor m_color;
	QList<QGraphicsItem*> m_sceneItems;
};


} // namespace imtloggui


