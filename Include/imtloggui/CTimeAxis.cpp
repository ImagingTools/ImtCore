#include <imtloggui/CTimeAxis.h>

// Qt includes
#include <QDebug>
#include <QtCore/QRectF>
#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
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
	return axisRect.adjusted(-100, 0, 100, 0);
}


void CTimeAxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	// Full rectangle of the axis:
	QRectF axisRect = boundingRect();

	// Logical rectangle for the defined time range:
	QRectF itemRect = rect();

	// Fill the full axis rectangle with the background color:
	painter->setPen(Qt::transparent);
	painter->setBrush(Qt::darkGray);
	painter->drawRect(axisRect);

	// Current view transformation:
	QGraphicsView* viewPtr = scene()->views().first();
	QTransform viewTransform = viewPtr->transform();

	quint64 timeRange = m_startDateTime.secsTo(m_endDateTime);

	double viewWidth = itemRect.width() * viewTransform.m11();
	double secondsPerPixel = viewWidth / timeRange;

	QString beginTime = m_startDateTime.toString("dd.MM.yyyy hh:mm:ss.zzz");
	QString endTime = m_endDateTime.toString("dd.MM.yyyy hh:mm:ss.zzz");
	int labelWidth = option->fontMetrics.width(beginTime);

	double majorStepSize = 1.5 * labelWidth;
	int majorTicksCount = 1 + int(0.5 + viewWidth / majorStepSize);

	painter->setPen(Qt::white);

	QRectF firstLabelRect = QRectF(-labelWidth / 2, axisRect.top() + axisRect.height() / 2, labelWidth, axisRect.height() / 2 - 2);
	painter->drawText(firstLabelRect, beginTime);
	painter->drawLine(QLineF(0, axisRect.top() + 1, 0, axisRect.bottom() - axisRect.height() / 1.5));

	QRectF lastLabelRect = QRectF(itemRect.right() * viewTransform.m11() - labelWidth / 2, axisRect.top() + axisRect.height() / 2, labelWidth, axisRect.height() / 2 - 2);
	painter->drawText(lastLabelRect, endTime);
	painter->drawLine(QLineF(itemRect.right() * viewTransform.m11(), axisRect.top() + 1, itemRect.right() * viewTransform.m11(), axisRect.bottom() - axisRect.height() / 1.5));

	// Draw major ticks:
	majorStepSize = viewWidth / double(majorTicksCount - 2);
	if (majorTicksCount > 0){
		for (int x = 1; x < majorTicksCount - 2; ++x){
			double xPos = x * majorStepSize;
			
			int secondsOffset = secondsPerPixel * xPos;
			QDateTime time = m_startDateTime.addSecs(secondsOffset);
			QString timeString = time.toString("dd.MM.yyyy hh:mm:ss.zzz");

			painter->drawLine(QLineF(xPos, axisRect.top() + 1, xPos, axisRect.bottom() - axisRect.height() / 1.5));
			QRectF labelRect = QRectF(xPos - labelWidth / 2, axisRect.top() + axisRect.height() / 2, labelWidth, axisRect.height() / 2 - 2);
			painter->drawText(labelRect, timeString);
		}
	}
}


// private methods

QRectF CTimeAxis::SceneVisibleRect() const
{
	QGraphicsView* viewPtr = scene()->views().first();

	QRect viewportRect = viewPtr->viewport()->rect();

	QRectF visibleSceneRect = viewPtr->mapToScene(viewportRect).boundingRect();

	return visibleSceneRect;
}


} // namespace imtloggui


