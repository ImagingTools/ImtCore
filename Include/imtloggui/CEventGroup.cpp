#include <imtloggui/CEventGroup.h>


// Qt includes
#include <QDebug>
#include <QtCore/QRectF>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


CEventGroup::CEventGroup(QGraphicsItem *parent)
	: BaseClass(parent)
{
}


void CEventGroup::setColor(const QColor& color)
{
	m_color = color;
}


// reimplemented (QGraphicsItem)

QRectF CEventGroup::boundingRect() const
{
	QRectF bounding = rect();
	QPointF origin = bounding.bottomLeft();
	
	bounding.translate(-origin);

	return bounding;
}


void CEventGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen pen;
	pen = QPen(Qt::darkGray);
	painter->setPen(pen);
	painter->setBrush(m_color);

	QRectF bounding = boundingRect();
	painter->drawRect(bounding);
}


} // namespace imtloggui


