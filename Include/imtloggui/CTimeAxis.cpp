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


void CTimeAxis::EnsureTimeRange(const QDateTime& time)
{
	if (!m_firstEvent.isValid()){
		m_firstEvent = time;
		m_lastEvent = time;
		m_startTime = time;
		m_endTime = time;
	}

	if (time < m_firstEvent){
		m_firstEvent = time;
	}

	if (time > m_lastEvent){
		m_lastEvent = time;
	}

	qint64 range = m_firstEvent.secsTo(m_lastEvent);

	if (range > QUA_MONTH){
		m_startTime.setDate(QDate(m_firstEvent.date().year(), 1, 1));
		m_startTime.setTime(QTime(0, 0));

		m_endTime.setDate(QDate(m_lastEvent.date().year(), 1, 1));
		m_endTime.setTime(QTime(0, 0));
		m_endTime = m_endTime.addMonths(1);
	}
	else if (range > QUA_DAY){
		m_startTime.setDate(m_firstEvent.date());
		m_startTime.setTime(QTime(0, 0));

		m_endTime.setDate(m_lastEvent.date());
		m_endTime.setTime(QTime(0, 0));
		m_endTime = m_endTime.addDays(1);
	}
	else{
		m_startTime.setDate(m_firstEvent.date());
		m_startTime.setTime(QTime(m_firstEvent.time().hour(), 0));

		m_endTime.setDate(m_lastEvent.date());
		m_endTime.setTime(QTime(m_lastEvent.time().hour(), 0));
		m_endTime = m_endTime.addSecs(3600);

		if (!m_baseTime.isValid()){
			m_baseTime = m_startTime;
		}
	}

	qDebug() << "---------------------------------------------------------------------------------------";
	qDebug() << "Inserted: " << time;
	qDebug() << m_startTime;
	qDebug() << m_endTime;

	setRect(0, 0, m_startTime.secsTo(m_endTime), 40);
	if (m_baseTime != m_startTime){
		m_baseTime = m_startTime;
		Q_EMIT AxisChanged();
	}
}


void CTimeAxis::AdaptTickPitch()
{
	QRectF sceneVisibleRect = SceneVisibleRect();
	QRectF visibleRect = rect().intersected(mapFromScene(sceneVisibleRect).boundingRect());
	double viewPortWidth = scene()->views().first()->viewport()->width();

	qDebug() << visibleRect;
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
	// Fill the full axis rectangle with the background color:
	painter->setPen(Qt::transparent);
	painter->setBrush(Qt::darkGray);
	painter->drawRect(boundingRect());

	if (!m_startTime.isValid() || !m_endTime.isValid()){
		return;
	}

	// Full rectangle of the axis:
	QRectF visibleRect = rect().intersected(SceneVisibleRect());

	// Logical rectangle for the defined time range:
	QRectF itemRect = rect();

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

	QRectF firstLabelRect = QRectF(-labelWidth / 2, itemRect.top() + itemRect.height() / 2, labelWidth, itemRect.height() / 2 - 2);
	painter->drawText(firstLabelRect, beginTime);
	painter->drawLine(QLineF(0, itemRect.top() + 1, 0, itemRect.bottom() - itemRect.height() / 1.5));

	if (m_startTime == m_endTime){
		return;
	}

	QRectF lastLabelRect = QRectF(itemRect.right() * viewTransform.m11() - labelWidth / 2, itemRect.top() + itemRect.height() / 2, labelWidth, itemRect.height() / 2 - 2);
	painter->drawText(lastLabelRect, endTime);
	painter->drawLine(QLineF(itemRect.right() * viewTransform.m11(), itemRect.top() + 1, itemRect.right() * viewTransform.m11(), itemRect.bottom() - itemRect.height() / 1.5));

	// Draw major ticks:
	majorStepSize = viewWidth / double(majorTicksCount - 2);
	if (majorTicksCount > 0){
		for (int x = 1; x < majorTicksCount - 2; ++x){
			double xPos = x * majorStepSize;
			
			int msOffset = secondsPerPixel * xPos * 1000;
			QDateTime time = m_startTime.addMSecs(msOffset);
			QString timeString = time.toString("dd.MM.yyyy hh:mm:ss.zzz");

			painter->drawLine(QLineF(xPos, itemRect.top() + 1, xPos, itemRect.bottom() - itemRect.height() / 1.5));
			QRectF labelRect = QRectF(xPos - labelWidth / 2, itemRect.top() + itemRect.height() / 2, labelWidth, itemRect.height() / 2 - 2);
			painter->drawText(labelRect, timeString);
		}
	}
}


// reimplemented (IEventScenePositionProvider)

double CTimeAxis::GetScenePositionFromTime(const QDateTime& time) const
{
	if (!m_startTime.isValid() || !m_endTime.isValid()){
		return DBL_MIN;
	}

	if (time < m_startTime || time > m_endTime){
		return DBL_MIN;
	}

	if (m_startTime == m_endTime){
		return pos().x();
	}

	double delta = time.toMSecsSinceEpoch() - m_startTime.toMSecsSinceEpoch();

	return pos().x() + delta / 1000;
}

QDateTime CTimeAxis::GetTimeFromScenePosition(double position) const
{
	if (!m_startTime.isValid() || !m_endTime.isValid()){
		return QDateTime();
	}

	if (position < rect().left() || position > rect().right()){
		return QDateTime();
	}

	if (m_startTime == m_endTime){
		if (qFuzzyCompare(position, pos().x())){
			return m_startTime;
		}

		return QDateTime();
	}

	return QDateTime::fromMSecsSinceEpoch(m_startTime.toMSecsSinceEpoch() + m_startTime.msecsTo(m_endTime) * (position - pos().x()) / rect().width());
}


// private methods

QRectF CTimeAxis::SceneVisibleRect() const
{
	QGraphicsScene* scenePtr = scene();
	if (scenePtr == nullptr){
		return QRectF();
	}

	QGraphicsView* viewPtr = scenePtr->views().first();
	if (viewPtr == nullptr){
		return QRectF();
	}

	QRect viewportRect = viewPtr->viewport()->rect();

	QRectF visibleSceneRect = viewPtr->mapToScene(viewportRect).boundingRect();

	return visibleSceneRect;
}


} // namespace imtloggui


