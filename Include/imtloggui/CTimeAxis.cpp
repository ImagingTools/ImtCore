#include <imtloggui/CTimeAxis.h>

// Qt includes
#include <QDebug>
#include <QtCore/QRectF>
#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


CTimeAxis::CTimeAxis(QGraphicsItem *parent)
{
	setZValue(100000);
	setFlags(ItemIgnoresTransformations);
}


void CTimeAxis::setTimeSpan(const QDateTime & startDateTime, const QDateTime & endDateTime)
{
	m_startDateTime = startDateTime;
	m_endDateTime = endDateTime;

	int secDiff = int(startDateTime.secsTo(endDateTime));

	int width = qMax(1000, secDiff);

	setRect(0, 40, width, -40);
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

	axisRect.setTop(rect().height());
	axisRect.setBottom(0);

	// Left and right marings for the drawing the first and last tick labels:
	return axisRect.adjusted(-200, 0, 200, 0);
}


void CTimeAxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	// Full rectangle of the axis:
	QRectF axisRect = boundingRect();

	// Logical rectangle for the defined time range:
	QRectF itemRect = rect();

	// Fill the full axis rectangle with the background color:
	painter->setPen(Qt::transparent);
	painter->setBrush(Qt::black);
	painter->drawRect(axisRect);

	// Current view transformation:
	QGraphicsView* viewPtr = scene()->views().first();
	QTransform viewTransform = viewPtr->transform();

	quint64 secDiff = m_startDateTime.secsTo(m_endDateTime);

	double viewWidth = itemRect.width() * viewTransform.m11();

	QString beginTime = m_startDateTime.toString(Qt::ISODate);
	QString endTime = m_endDateTime.toString(Qt::ISODate);
	int labelWidth = option->fontMetrics.width(beginTime);

	int majorTicksCount = viewWidth / double((2 * labelWidth));

	double pixelsPerTick = viewWidth / double(secDiff);
	if (pixelsPerTick < 5){
		// Switch to minutes:
		pixelsPerTick *= 60;
	}

	if (pixelsPerTick < 5){
		// Switch to hours:
		pixelsPerTick *= 60;
	}

	double step = pixelsPerTick;
	if (step < 1){
		step = 1;
	}

	painter->setPen(Qt::yellow);

	QRectF firstLabelRect = QRectF(-labelWidth / 2, axisRect.top() + axisRect.height() / 2, labelWidth, axisRect.height() / 2 - 2);
//	painter->drawRect(labelRect);
	painter->drawText(firstLabelRect, beginTime);

	QRectF lastLabelRect = QRectF(itemRect.right() * viewTransform.m11() - labelWidth / 2, axisRect.top() + axisRect.height() / 2, labelWidth, axisRect.height() / 2 - 2);
	//	painter->drawRect(labelRect);
	painter->drawText(lastLabelRect, endTime);

	painter->setPen(Qt::red);

	for (double x = itemRect.left(); x <= itemRect.right() * viewTransform.m11(); x += step){
		painter->drawLine(QLineF(x, axisRect.top() + 1, x, axisRect.bottom() - axisRect.height() / 1.5));
	}
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


