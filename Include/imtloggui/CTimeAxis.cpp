#include <imtloggui/CTimeAxis.h>

// Qt includes
#include <QDebug>
#include <QtCore/QRectF>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


CTimeAxis::CTimeAxis(QGraphicsItem *parent)
{
}


void CTimeAxis::setTimeSpan(const QDateTime & startDateTime, const QDateTime & endDateTime)
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


void CTimeAxis::setColor(const QColor& color)
{
	m_color = color;
}


void CTimeAxis::setGeometry(const QRectF &geometry)
{
	m_geometryRect = geometry;
	m_boundingRect = QRectF(QPointF(), geometry.size());
	m_boundingRect.translate(0, -geometry.height());
	setPos(geometry.bottomLeft());
}


void CTimeAxis::update()
{
	QGraphicsScene* scenePtr = scene();
	QGraphicsView* viewPtr = scene()->views()[0];

	for (QGraphicsItem* item : m_sceneItems){
		scenePtr->removeItem(item);
	}

	m_sceneItems.clear();

	QPen pen = QPen(Qt::black, 3);
	pen.setWidth(0);

	QRectF visibleSceneRect = viewPtr->mapToScene(viewPtr->viewport()->rect()).boundingRect();
	QRectF rect = m_geometryRect;

	rect.setLeft(rect.left() + 5);
	rect.setRight(rect.right() - 5);

	rect.setTop(visibleSceneRect.bottom() - m_geometryRect.height() / viewPtr->transform().m22());
	rect.setBottom(visibleSceneRect.bottom());
	m_sceneItems.append(scenePtr->addRect(rect, QPen(), QBrush(Qt::red)));

	QPointF pnt(visibleSceneRect.left() + 50, rect.top());
	while (pnt.x() < visibleSceneRect.right()){
		QGraphicsTextItem* item = scenePtr->addText(QString::number((int)pnt.x()/100));
		item->setPos(pnt);
		item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIgnoresTransformations, true);
		m_sceneItems.append(item);
		pnt.rx() += 100;

		//QGraphicsItem* item = scenePtr->addLine(pnt.x(), rect.top(), pnt.x(), rect.bottom(), pen);
		//m_sceneItems.append(item);
		//pnt.rx() += 50;
	}

	qDebug() << m_sceneItems.count();
}


// reimplemented (QGraphicsItem)

QRectF CTimeAxis::boundingRect() const
{
	return m_boundingRect;
}


void CTimeAxis::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	//QPen pen;
	//QPainterPath path;
	//path.addRoundedRect(m_boundingRect, 10, 10);

	//pen = QPen(Qt::black, 3);
	//painter->setPen(pen);
	//painter->fillPath(path, m_color);
	//painter->drawPath(path);

	//return;

	//QGraphicsScene* scenePtr = scene();
	//if (scenePtr->views().isEmpty()){
	//	return;
	//}

	//QRectF rect = boundingRect();

	//pen.setColor(Qt::red);
	//painter->setPen(pen);
	//painter->drawLine(QLineF(rect.topLeft(), rect.bottomLeft()));
	//painter->drawLine(QLineF(rect.topRight(), rect.bottomRight()));


	//QGraphicsView* viewPtr = scenePtr->views().first();
	//QRectF visibleRect = mapFromScene(viewPtr->mapToScene(viewPtr->rect()).boundingRect()).boundingRect();

	//if (visibleRect.left() < 0){
	//	visibleRect.setLeft(0);
	//}

	//if (visibleRect.right() > rect.right()){
	//	visibleRect.setRight(rect.right());
	//}

	////double k = (m_endDateTime.toMSecsSinceEpoch() - m_startDateTime.toMSecsSinceEpoch()) / rect.width();

	//QDateTime firstMajDT = m_startDateTime;
	//firstMajDT.setTime(QTime(firstMajDT.time().hour(), 0));

	//if (firstMajDT.toMSecsSinceEpoch() < m_startDateTime.toMSecsSinceEpoch()){
	//	firstMajDT = firstMajDT.addMSecs(MS_HOUR);
	//}

	//double firstMajTick = convertDateTimeToPosX(firstMajDT);
	//double nextMajTick = convertDateTimeToPosX(firstMajDT.addMSecs(MS_HOUR));
	//double deltaMajTick = nextMajTick - firstMajTick;
	//double deltaMinTick = deltaMajTick / m_minorTickCount;

	//pen.setColor(Qt::black);
	//painter->setPen(pen);

	//while (firstMajTick < rect.right()){
	//	painter->drawLine(firstMajTick, rect.bottom(), firstMajTick, rect.bottom() - 50);
	//	painter->drawText(QPointF(firstMajTick, rect.bottom() - 50), "TEST");
	//	
	//	firstMajTick += deltaMajTick;
	//}
}


// private methods

double CTimeAxis::convertDateTimeToPosX(const QDateTime& dateTime)
{
	qint64 msecStart = m_startDateTime.toMSecsSinceEpoch();
	qint64 msecEnd = m_endDateTime.toMSecsSinceEpoch();
	qint64 msecDateTime = dateTime.toMSecsSinceEpoch();

	if (msecDateTime < msecStart || msecDateTime > msecEnd){
		return -1;
	}

	return m_geometryRect.width() * (msecDateTime - msecStart) / (msecEnd - msecStart);
}


} // namespace imtloggui


