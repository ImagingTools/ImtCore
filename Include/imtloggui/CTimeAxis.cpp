#include <imtloggui/CTimeAxis.h>

// Qt includes
#include <QDebug>
#include <QtCore/QRectF>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


CTimeAxis::CTimeAxis(QGraphicsItem *parent)
{
	setGraphicsItem(this);
}


void CTimeAxis::setTimeInterval(const QDateTime & startDateTime, const QDateTime & endDateTime)
{
	m_startDateTime = startDateTime;
	m_endDateTime = endDateTime;
}


bool CTimeAxis::setMinorTickCount(int count)
{
	if (count > 0){
		m_minorTickCount = count;
		return true;
	}

	return false;
}


// reimplemented (QGraphicsLayoutItem)

void CTimeAxis::setGeometry(const QRectF &geometry)
{
	prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(geometry);
	setPos(geometry.topLeft());
}


// reimplemented (QGraphicsItem)

QRectF CTimeAxis::boundingRect() const
{
	return QRectF(QPointF(0, 0), geometry().size());
}


void CTimeAxis::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	qDebug() << option->exposedRect;

	QPen pen;
	pen.setWidth(0);
	painter->setPen(pen);

	QRectF geom = geometry();
	QPointF p1, p2;
	p1 = QPointF(0,0);
	p2 = QPointF(geom.width(), 0);
	while (p1.y() <= geom.height()){
		painter->drawLine(p1, p2);
		p1.ry() += 5;
		p2.ry() += 5;
	}

	p1 = QPointF(0, 0);
	p2 = QPointF(0, geom.height());
	while (p1.x() <= geom.width()){
		painter->drawLine(p1, p2);
		p1.rx() += 5;
		p2.rx() += 5;
	}

	//return;

	//for (QGraphicsItem* item : m_sceneItems){
	//	scene()->removeItem(item);
	//}

	//m_sceneItems.clear();

	//m_sceneItems.append(scene()->addLine(20, -25, 20, 25));

	//int x = (((int)m_viewRect.left()) / 40) * 40;
	//while (x < m_viewRect.right()){
	//	QGraphicsLineItem *item = nullptr;

	//	if (x % 400 == 0){
	//		if (transform().m11() > 0.1){
	//			item = scene()->addLine(x, 0, x, 25 - 1.5 / transform().m11() / 2.5);
	//		}
	//	}
	//	else if (x % 200 == 0){
	//		if (transform().m11() > 0.2){
	//			item = scene()->addLine(x, 0, x, 15);
	//		}
	//	}
	//	else{
	//		if (transform().m11() > 1){
	//			item = scene()->addLine(x, 0, x, 5);
	//		}
	//	}

	//	if (item != nullptr){
	//		QPen pen = item->pen();
	//		pen.setWidthF(0/*1.5/transform().m11()*/);
	//		pen.setColor(Qt::red);
	//		item->setPen(pen);

	//		//item->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
	//		m_sceneItems.append(item);
	//	}

	//	x += 40;
	//}

	//qDebug() << rect() << transform().m11() << transform().m22();
}


// protected methods

QSizeF CTimeAxis::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
	//switch (which) {
	//case Qt::MinimumSize:
	//case Qt::PreferredSize:
	//	// Do not allow a size smaller than the pixmap with two frames around it.
	//	//return m_pix.size() + QSize(12, 12);
	//case Qt::MaximumSize:
	//	return QSizeF(INT32_MAX, INT32_MAX);
	//default:
	//	break;
	//}

	return constraint;
}


} // namespace imtloggui


