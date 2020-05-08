#include <imtloggui/CTimeAxis.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QRectF>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


CTimeAxis::CTimeAxis(QGraphicsItem* parent)
	: BaseClass(parent),
	QObject()
{
	setZValue(100000);
	setFlags(ItemIgnoresTransformations);
	m_startTime = QDateTime();
	m_endTime = QDateTime();
}


void CTimeAxis::setPos(const QPointF &origin)
{
	if (origin != pos()){
		BaseClass::setPos(origin);
		Q_EMIT AxisChanged();
	}
}


void CTimeAxis::setPos(double x, double y)
{
	setPos(QPointF(x, y));
}


const QDateTime& CTimeAxis::GetStartOfRange() const
{
	return m_startTime;
}


const QDateTime& CTimeAxis::GetEndOfRange() const
{
	return m_endTime;
}


void CTimeAxis::SetTimeRange(const QDateTime& firstEventTime, const QDateTime& lastEventTime)
{
	bool isAxisChanged = false;

	if (m_startTime != firstEventTime || m_endTime != lastEventTime){

		if (m_startTime != firstEventTime){
			isAxisChanged = true;
		}

		m_startTime = firstEventTime;
		m_endTime = lastEventTime;

		int diff = firstEventTime.msecsTo(lastEventTime);

		setPos(0, 0);
		setRect(0, 0, diff / 1000., 40);
	}

	if (isAxisChanged){
		Q_EMIT AxisChanged();
	}
}


bool CTimeAxis::SetMinorTickCount(int count)
{
	if (count > 0){
		m_minorTickCount = count;
		return true;
	}

	return false;
}


void CTimeAxis::SetColor(const QColor& color)
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

	axisRect.setTop(0);
	axisRect.setBottom(rect().height());

	// Left and right marings for the drawing the first and last tick labels:
	axisRect.adjust(-100, 0, 100, 0);

	double scale = scene()->views().first()->viewportTransform().m11();
	
	if (scale > 1){
		axisRect.setLeft(origin.x() + (axisRect.left() - origin.x()) * scale);
		axisRect.setRight(origin.x() + (axisRect.right() - origin.x()) * scale);
	}

	return axisRect;
}


void CTimeAxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* /*widget*/)
{
	// Full rectangle of the axis:
	QRectF axisRect = boundingRect();

	// Logical rectangle for the defined time range:
	QRectF itemRect = rect();

	// Fill the full axis rectangle with the background color:
	painter->setPen(Qt::transparent);
	painter->setBrush(Qt::darkGray);
	painter->drawRect(axisRect);

	if (!m_startTime.isValid() || !m_endTime.isValid()){
		return;
	}

	// Current view transformation:
	QGraphicsView* viewPtr = scene()->views().first();
	QTransform viewTransform = viewPtr->viewportTransform();

	quint64 timeRange = m_startTime.secsTo(m_endTime);

	double viewWidth = itemRect.width() * viewTransform.m11();
	double secondsPerPixel = timeRange / viewWidth;

	QString beginTime = m_startTime.toString("dd.MM.yyyy hh:mm:ss.zzz");
	QString endTime = m_endTime.toString("dd.MM.yyyy hh:mm:ss.zzz");
	int labelWidth = option->fontMetrics.horizontalAdvance(beginTime);

	double majorStepSize = 1.5 * labelWidth;
	int majorTicksCount = 1 + int(0.5 + viewWidth / majorStepSize);

	painter->setPen(Qt::white);

	QRectF firstLabelRect = QRectF(-labelWidth / 2, axisRect.top() + axisRect.height() / 2, labelWidth, axisRect.height() / 2 - 2);
	painter->drawText(firstLabelRect, beginTime);
	painter->drawLine(QLineF(0, axisRect.top() + 1, 0, axisRect.bottom() - axisRect.height() / 1.5));

	if (m_startTime == m_endTime){
		return;
	}

	QRectF lastLabelRect = QRectF(itemRect.right() * viewTransform.m11() - labelWidth / 2, axisRect.top() + axisRect.height() / 2, labelWidth, axisRect.height() / 2 - 2);
	painter->drawText(lastLabelRect, endTime);
	painter->drawLine(QLineF(itemRect.right() * viewTransform.m11(), axisRect.top() + 1, itemRect.right() * viewTransform.m11(), axisRect.bottom() - axisRect.height() / 1.5));

	// Draw major ticks:
	majorStepSize = viewWidth / double(majorTicksCount - 2);
	if (majorTicksCount > 0){
		for (int x = 1; x < majorTicksCount - 2; ++x){
			double xPos = x * majorStepSize;
			
			int msOffset = secondsPerPixel * xPos * 1000;
			QDateTime time = m_startTime.addMSecs(msOffset);
			QString timeString = time.toString("dd.MM.yyyy hh:mm:ss.zzz");

			painter->drawLine(QLineF(xPos, axisRect.top() + 1, xPos, axisRect.bottom() - axisRect.height() / 1.5));
			QRectF labelRect = QRectF(xPos - labelWidth / 2, axisRect.top() + axisRect.height() / 2, labelWidth, axisRect.height() / 2 - 2);
			painter->drawText(labelRect, timeString);
		}
	}
}


// reimplemented (IEventScenePositionProvider)
double CTimeAxis::GetScenePosition(const QDateTime& time) const
{
	if (m_startTime == m_endTime){
		return pos().x();
	}

	double delta = time.toMSecsSinceEpoch() - m_startTime.toMSecsSinceEpoch();

	return pos().x() + delta / 1000;
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


