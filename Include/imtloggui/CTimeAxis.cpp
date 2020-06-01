#include <imtloggui/CTimeAxis.h>


// Qt includes
#include <QtCore/QRectF>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>

// ImtCore includes
#include <imtbase/DateTimeUtils.h>


namespace imtloggui
{


CTimeAxis::CTimeAxis(QGraphicsItem* parent)
	:BaseClass(parent),
	m_fontMetrics(QFont()),
	m_labelWidthFactor(1.5),
	m_minMinorTickStep(20)
{
	setZValue(100000);

	m_startTime = QDateTime();
	m_endTime = QDateTime();

	CreateTimeItemTable();
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

	setRect(0, 0, m_startTime.secsTo(m_endTime), 40);
	if (m_baseTime != m_startTime){
		m_baseTime = m_startTime;
		Q_EMIT EmitAxisChanged();
	}
}


int CTimeAxis::GetMargin()
{
	return m_fontMetrics.horizontalAdvance("##.##.#### ##:##:##.###") / 2;
}


void CTimeAxis::CreateTimeItemTable()
{
	m_intervals.clear();

	m_intervals.reserve(TI_COUNT);
	for (int i = 0; i < TI_COUNT; i++){
		m_intervals.append(MajorIntervalItem());
	}

	m_intervals[TI_1MS].timeFormat = "dd.MM.yyyy hh:mm:ss.zzz";
	m_intervals[TI_1MS].distance = 0.001;
	m_intervals[TI_10MS].timeFormat = "dd.MM.yyyy hh:mm:ss.zzz";
	m_intervals[TI_10MS].distance = 0.01;
	m_intervals[TI_100MS].timeFormat = "dd.MM.yyyy hh:mm:ss.zzz";
	m_intervals[TI_100MS].distance = 0.1;
	
	m_intervals[TI_1S].timeFormat = "dd.MM.yyyy hh:mm:ss";
	m_intervals[TI_1S].distance = 1;
	m_intervals[TI_5S].timeFormat = "dd.MM.yyyy hh:mm:ss";
	m_intervals[TI_5S].distance = 5;
	m_intervals[TI_15S].timeFormat = "dd.MM.yyyy hh:mm:ss";
	m_intervals[TI_15S].distance = 15;
	m_intervals[TI_30S].timeFormat = "dd.MM.yyyy hh:mm:ss";
	m_intervals[TI_30S].distance = 30;

	m_intervals[TI_1M].timeFormat = "dd.MM.yyyy hh:mm";
	m_intervals[TI_1M].distance = 60;
	m_intervals[TI_5M].timeFormat = "dd.MM.yyyy hh:mm";
	m_intervals[TI_5M].distance = 300;
	m_intervals[TI_15M].timeFormat = "dd.MM.yyyy hh:mm";
	m_intervals[TI_15M].distance = 900;
	m_intervals[TI_30M].timeFormat = "dd.MM.yyyy hh:mm";
	m_intervals[TI_30M].distance = 1800;

	m_intervals[TI_1H].timeFormat = "dd.MM.yyyy hh:mm";
	m_intervals[TI_1H].distance = 3600;
	m_intervals[TI_3H].timeFormat = "dd.MM.yyyy hh:mm";
	m_intervals[TI_3H].distance = 10800;
	m_intervals[TI_6H].timeFormat = "dd.MM.yyyy hh:mm";
	m_intervals[TI_6H].distance = 21600;
	m_intervals[TI_12H].timeFormat = "dd.MM.yyyy hh:mm";
	m_intervals[TI_12H].distance = 43200;

	m_intervals[TI_DAY].timeFormat = "dd.MM.yyyy";
	m_intervals[TI_DAY].distance = 86400;
	m_intervals[TI_WEEK].timeFormat = "dd.MM.yyyy";
	m_intervals[TI_WEEK].distance = 604800;
	m_intervals[TI_MONTH].timeFormat = "MM.yyyy";
	m_intervals[TI_MONTH].distance = 2635200; // approximately
	m_intervals[TI_QUARTER].timeFormat = "MM.yyyy";
	m_intervals[TI_QUARTER].distance = 7905600; // approximately
	m_intervals[TI_YEAR].timeFormat = "yyyy";
	m_intervals[TI_YEAR].distance = 31536000; // approximately

	double labelWidth;

	for (int i = 0; i < TI_COUNT; i++){
		labelWidth = m_fontMetrics.horizontalAdvance(m_intervals[i].timeFormat);
		m_intervals[i].scaleMin = labelWidth * m_labelWidthFactor / m_intervals[i].distance;
		m_intervals[i].interval = (TimeInterval)i;

		for (int j = 1; j <= 3; j++){
			if (i - j < 0){
				break;
			}

			MinorIntervalItem minorInterval;
			minorInterval.distance = m_intervals[i - j].distance;
			minorInterval.interval = m_intervals[i - j].interval;
			minorInterval.scaleMin = m_minMinorTickStep / minorInterval.distance;

			m_intervals[i].minorIntervals.append(minorInterval);
		}
	}

	m_intervals[TI_YEAR].scaleMin = DBL_MIN;

	for (int i = 1; i < TI_COUNT; i++){
		m_intervals[i].scaleMax = m_intervals[i-1].scaleMin;
	}

	m_intervals[TI_1MS].scaleMax = DBL_MAX;
}


// reimplemented (QGraphicsRectItem)

QRectF CTimeAxis::boundingRect() const
{
	QRectF visibleRect = GetSceneVisibleRect();
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
	axisRect.adjust(-100 / GetCurrentScaleX(), 0, 100 / GetCurrentScaleX(), 0);

	//double scale = GetCurrentScale();
	//
	//if (scale > 1){
	//	axisRect.setLeft(origin.x() + (axisRect.left() - origin.x()) * scale);
	//	axisRect.setRight(origin.x() + (axisRect.right() - origin.x()) * scale);
	//}

	return axisRect;
}


void CTimeAxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* /*widget*/)
{
	if (m_fontMetrics != option->fontMetrics){
		m_fontMetrics = option->fontMetrics;
	}

	// Fill the full axis rectangle with the background color:
	painter->setPen(Qt::transparent);
	painter->setBrush(QApplication::palette().color(QPalette::Background));
	painter->drawRect(boundingRect());

	if (!m_startTime.isValid() || !m_endTime.isValid()){
		return;
	}

	double scaleX = GetCurrentScaleX();
	double scaleY = GetCurrentScaleY();

	IntervalsInfo intervalsInfo = CalculateIntervals(scaleX);
	Ticks ticks = CalculateTicks(intervalsInfo);

	// Logical rectangle for the defined time range:
	QRectF itemRect = rect();

	int labelWidth = 0;
	for (TickInfo info : ticks){
		if (info.type == TT_MAJOR){
			labelWidth = option->fontMetrics.horizontalAdvance(info.timeFormat);
			break;
		}
	}

	QPen pen(QBrush(), 1, Qt::SolidLine, Qt::FlatCap);
	pen.setColor(QColor("#335777"));
	painter->setPen(pen);

	if (m_startTime == m_endTime){
		return;
	}

	// Draw ticks:
	for (QDateTime time : ticks.keys()){
		TickInfo info = ticks[time];
		if (info.type == TT_MINOR){
			double xPos = GetRectPositionFromTime(time);
			if (xPos < 0){
				continue;
			}

			pen.setWidthF(1 / scaleX);
			painter->setPen(pen);
			painter->drawLine(QLineF(xPos, (rect().top() + 1) / scaleY, xPos, (rect().top() + itemRect.height() * 0.2) / scaleY));
		}
		else if (info.type == TT_MAJOR){
			double xPos = GetRectPositionFromTime(time);
			if (xPos < 0){
				continue;
			}
			
			pen.setWidthF(3 / scaleX);
			painter->setPen(pen);
			painter->drawLine(QLineF(xPos, (rect().top() + 1) / scaleY, xPos, (itemRect.bottom() - itemRect.height() / 1.5) / scaleY));
			
			QString labelText = time.toString(info.timeFormat);
			
			QTransform savedTransform = painter->transform();
			QTransform newTransform = savedTransform;

			newTransform.translate(xPos, (itemRect.top() + itemRect.height() * 3 / 4) / scaleY);
			newTransform.scale(1 / scaleX, 1 / scaleY);

			painter->setTransform(newTransform);
			painter->drawText(-labelWidth / 2, 0, labelText);
			painter->setTransform(savedTransform);
		}
	}
}


// reimplemented (QGraphicsItem)

void CTimeAxis::setPos(const QPointF &origin)
{
	if (origin != pos()){
		BaseClass::setPos(origin);
		Q_EMIT EmitAxisChanged();
	}
}


void CTimeAxis::setPos(double x, double y)
{
	setPos(QPointF(x, y));
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


QDateTime CTimeAxis::GetBeginTime() const
{
	return m_startTime;
}


QDateTime CTimeAxis::GetEndTime() const
{
	return m_endTime;
}


QDateTime CTimeAxis::GetVisibleBeginTime() const
{
	QRectF axisVisibleRect = GetAxisVisibleRect();

	return GetTimeFromRectPosition(axisVisibleRect.left());
}


QDateTime CTimeAxis::GetVisibleEndTime() const
{
	QRectF axisVisibleRect = GetAxisVisibleRect();

	return GetTimeFromRectPosition(axisVisibleRect.right());
}


// protected methods

CTimeAxis::IntervalsInfo CTimeAxis::CalculateIntervals(double scale) const
{
	IntervalsInfo timeItemInfo;
	timeItemInfo.minorInterval = TI_NONE;

	for (int i = 0; i < m_intervals.count(); i++){
		if (scale >= m_intervals[i].scaleMin && scale < m_intervals[i].scaleMax){
			timeItemInfo.majorInterval = m_intervals[i].interval;
			timeItemInfo.timeFormat = m_intervals[i].timeFormat;
			
			int minorTableSize = m_intervals[i].minorIntervals.count();
			for (int j = 0; j < minorTableSize; j++){
				if (scale > m_intervals[i].minorIntervals[minorTableSize - j - 1].scaleMin){
					timeItemInfo.minorInterval = m_intervals[i].minorIntervals[minorTableSize - j - 1].interval;
					break;
				}
			}

			break;
		}
	}

	return timeItemInfo;
}


CTimeAxis::Ticks CTimeAxis::CalculateTicks(const IntervalsInfo& intervalsInfo) const
{
	QRectF axisVisibleRect = GetAxisVisibleRect();

	QDateTime startTime = GetTimeFromRectPosition(axisVisibleRect.left());
	QDateTime endTime = GetTimeFromRectPosition(axisVisibleRect.right());
	startTime = startTime.addMSecs((startTime.toMSecsSinceEpoch() - endTime.toMSecsSinceEpoch()) / 5);
	endTime = endTime.addMSecs((endTime.toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch()) / 5);

	QDateTime startMajorTime;
	QDateTime startMinorTime;

	switch (intervalsInfo.majorInterval){
		case TI_1MS:
			startMajorTime = startTime;
			break;
		case TI_10MS:
			startMajorTime = imtbase::Get10MSecondBegin(startTime);
			break;
		case TI_100MS:
			startMajorTime = imtbase::Get100MSecondBegin(startTime);
			break;
		case TI_1S:
			startMajorTime = imtbase::GetSecondBegin(startTime);
			break;
		case TI_5S:
			startMajorTime = imtbase::Get5SecondBegin(startTime);
			break;
		case TI_15S:
			startMajorTime = imtbase::Get15SecondBegin(startTime);
			break;
		case TI_30S:
			startMajorTime = imtbase::Get30SecondBegin(startTime);
			break;
		case TI_1M:
			startMajorTime = imtbase::GetMinutesBegin(startTime);
			break;
		case TI_5M:
			startMajorTime = imtbase::Get5MinutesBegin(startTime);
			break;
		case TI_15M:
			startMajorTime = imtbase::Get15MinutesBegin(startTime);
			break;
		case TI_30M:
			startMajorTime = imtbase::Get30MinutesBegin(startTime);
			break;
		case TI_1H:
			startMajorTime = imtbase::GetHourBegin(startTime);
			break;
		case TI_3H:
			startMajorTime = imtbase::Get3HourBegin(startTime);
			break;
		case TI_6H:
			startMajorTime = imtbase::Get6HourBegin(startTime);
			break;
		case TI_12H:
			startMajorTime = imtbase::Get12HourBegin(startTime);
			break;
		case TI_DAY:
			startMajorTime = imtbase::GetDayBegin(startTime);
			break;
		case TI_WEEK:
			startMajorTime = imtbase::GetWeekBegin(startTime);
			break;
		case TI_MONTH:
			startMajorTime = imtbase::GetMonthBegin(startTime);
			break;
		case TI_QUARTER:
			startMajorTime = imtbase::GetQuorterBegin(startTime);
			break;
		case TI_YEAR:
			startMajorTime = imtbase::GetYearBegin(startTime);
			break;
	}

	switch (intervalsInfo.minorInterval){
		case TI_1MS:
			startMinorTime = startTime;
			break;
		case TI_10MS:
			startMinorTime = imtbase::Get10MSecondBegin(startTime);
			break;
		case TI_100MS:
			startMinorTime = imtbase::Get100MSecondBegin(startTime);
			break;
		case TI_1S:
			startMinorTime = imtbase::GetSecondBegin(startTime);
			break;
		case TI_5S:
			startMinorTime = imtbase::Get5SecondBegin(startTime);
			break;
		case TI_15S:
			startMinorTime = imtbase::Get15SecondBegin(startTime);
			break;
		case TI_30S:
			startMinorTime = imtbase::Get30SecondBegin(startTime);
			break;
		case TI_1M:
			startMinorTime = imtbase::GetMinutesBegin(startTime);
			break;
		case TI_5M:
			startMinorTime = imtbase::Get5MinutesBegin(startTime);
			break;
		case TI_15M:
			startMinorTime = imtbase::Get15MinutesBegin(startTime);
			break;
		case TI_30M:
			startMinorTime = imtbase::Get30MinutesBegin(startTime);
			break;
		case TI_1H:
			startMinorTime = imtbase::GetHourBegin(startTime);
			break;
		case TI_3H:
			startMinorTime = imtbase::Get3HourBegin(startTime);
			break;
		case TI_6H:
			startMinorTime = imtbase::Get6HourBegin(startTime);
			break;
		case TI_12H:
			startMinorTime = imtbase::Get12HourBegin(startTime);
			break;
		case TI_DAY:
			startMinorTime = imtbase::GetDayBegin(startTime);
			break;
		case TI_WEEK:
			startMinorTime = imtbase::GetWeekBegin(startTime);
			break;
		case TI_MONTH:
			startMinorTime = imtbase::GetMonthBegin(startTime);
			break;
		case TI_QUARTER:
			startMinorTime = imtbase::GetQuorterBegin(startTime);
			break;
		case TI_YEAR:
			startMinorTime = imtbase::GetYearBegin(startTime);
			break;
	}

	QDateTime currentTime;

	Ticks ticks;

	currentTime = startMajorTime;

	while (currentTime <= endTime){
		if (currentTime >= startTime){
			TickInfo info;
			info.type = TT_MAJOR;
			info.timeFormat = m_intervals[intervalsInfo.majorInterval].timeFormat;
			ticks[currentTime] = info;
		}

		if (intervalsInfo.majorInterval < TI_MONTH){
			if (m_intervals[intervalsInfo.majorInterval].distance < 1){
				currentTime = currentTime.addMSecs(m_intervals[intervalsInfo.majorInterval].distance * 1000);
			}
			else {
				currentTime = currentTime.addSecs(m_intervals[intervalsInfo.majorInterval].distance);
			}
		}
		else if (intervalsInfo.majorInterval == TI_MONTH){
			currentTime = currentTime.addMonths(1);
		}
		else if (intervalsInfo.majorInterval == TI_QUARTER){
			currentTime = currentTime.addMonths(3);
		}
		else if (intervalsInfo.majorInterval == TI_YEAR){
			currentTime = currentTime.addYears(1);
		}
	}

	currentTime = startMinorTime;

	if (intervalsInfo.minorInterval != TI_NONE){
		while (currentTime <= endTime){
			if (currentTime >= startTime){
				if (!ticks.contains(currentTime)){
					TickInfo info;
					info.type = TT_MINOR;
					ticks[currentTime] = info;
				}
			}

			if (intervalsInfo.minorInterval < TI_MONTH){
				if (m_intervals[intervalsInfo.minorInterval].distance < 1){
					currentTime = currentTime.addMSecs(m_intervals[intervalsInfo.minorInterval].distance * 1000);
				}
				else {
					currentTime = currentTime.addSecs(m_intervals[intervalsInfo.minorInterval].distance);
				}
			}
			else if (intervalsInfo.minorInterval == TI_MONTH){
				currentTime = currentTime.addMonths(1);
			}
			else if (intervalsInfo.minorInterval == TI_QUARTER){
				currentTime = currentTime.addMonths(3);
			}
			else if (intervalsInfo.minorInterval == TI_YEAR){
				currentTime = currentTime.addYears(1);
			}
		}
	}

	return ticks;
}


double CTimeAxis::GetCurrentScaleX() const
{
	return scene()->views().first()->viewportTransform().m11();
}


double CTimeAxis::GetCurrentScaleY() const
{
	return scene()->views().first()->viewportTransform().m22();
}


QRectF CTimeAxis::GetSceneVisibleRect() const
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


QRectF CTimeAxis::GetAxisVisibleRect() const
{
	QRectF sceneVisibleRect = GetSceneVisibleRect();
	QRectF axisRect = rect();
	
	axisRect.setTop(sceneVisibleRect.top());
	axisRect.setBottom(sceneVisibleRect.bottom());

	return axisRect.intersected(sceneVisibleRect);
}


double CTimeAxis::GetRectPositionFromTime(const QDateTime& time) const
{
	if (!m_startTime.isValid() || !m_endTime.isValid()){
		return -1;
	}

	if (time < m_startTime || time > m_endTime){
		return -1;
	}

	if (m_startTime == m_endTime){
		if (time == m_startTime){
			return rect().left();
		}

		return -1;
	}

	double delta = time.toMSecsSinceEpoch() - m_startTime.toMSecsSinceEpoch();

	return rect().left() + delta / 1000;
}


QDateTime CTimeAxis::GetTimeFromRectPosition(double position) const
{
	if (!m_startTime.isValid() || !m_endTime.isValid()){
		return QDateTime();
	}

	if (position < rect().left() || position > rect().right()){
		return QDateTime();
	}

	if (m_startTime == m_endTime){
		if (qFuzzyCompare(position, rect().left())){
			return m_startTime;
		}

		return QDateTime();
	}

	return QDateTime::fromMSecsSinceEpoch(m_startTime.toMSecsSinceEpoch() + m_startTime.msecsTo(m_endTime) * (position - rect().left()) / rect().width());
}


} // namespace imtloggui


