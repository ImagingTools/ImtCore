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


// reimplemented (QGraphicsRectItem)

QRectF CTimeAxis::boundingRect() const
{
	QRectF visibleRect = SceneVisibleRect();
	QRectF axisRect = rect();
	QPointF origin = axisRect.bottomLeft();

	if (visibleRect.left() < axisRect.left()){
		axisRect.setLeft(visibleRect.left());
	}

	if (visibleRect.right() > axisRect.right()){
		axisRect.setRight(visibleRect.right());
	}

	axisRect.setTop(rect().height() / scene()->views().first()->transform().m22());
	axisRect.setBottom(0);
	

	return axisRect;
}


void CTimeAxis::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPainterPath path;
	path.addRect(boundingRect());

	QPen pen;
	pen.setWidth(2);

	painter->setPen(pen);
	painter->fillPath(path, m_color);
	painter->drawPath(path);

	return;

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

QRectF CTimeAxis::SceneVisibleRect() const
{
	QGraphicsView* viewPtr = scene()->views().first();
	QRectF visibleSceneRect = viewPtr->mapToScene(viewPtr->viewport()->rect()).boundingRect();
	return visibleSceneRect;
}


double CTimeAxis::convertDateTimeToPosX(const QDateTime& dateTime)
{
	qint64 msecStart = m_startDateTime.toMSecsSinceEpoch();
	qint64 msecEnd = m_endDateTime.toMSecsSinceEpoch();
	qint64 msecDateTime = dateTime.toMSecsSinceEpoch();

	if (msecDateTime < msecStart || msecDateTime > msecEnd){
		return -1;
	}

	return rect().width() * (msecDateTime - msecStart) / (msecEnd - msecStart);
}


} // namespace imtloggui


