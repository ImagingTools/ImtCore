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
{
}


void CEventGroup::setColor(const QColor& color)
{
	m_color = color;
}


void CEventGroup::setGeometry(const QRectF &geometry)
{
	m_geometryRect = geometry;
	m_boundingRect = QRectF(QPointF(), geometry.size());
	setPos(geometry.topLeft());
}


void CEventGroup::update()
{
}


// reimplemented (QGraphicsItem)

QRectF CEventGroup::boundingRect() const
{
	return m_boundingRect;
}


void CEventGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen pen;
	QPainterPath path;
	path.addRoundedRect(m_boundingRect, 10, 10);

	pen = QPen(Qt::black, 3);
	painter->setPen(pen);
	painter->fillPath(path, m_color);
	painter->drawPath(path);
}


} // namespace imtloggui


