#include <imtloggui/CProductionQualityItem.h>

// Qt includes
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsSceneHoverEvent>


namespace imtloggui
{


// public methods

CProductionQualityItem::CProductionQualityItem(QGraphicsItem* parent)
	:BaseClass(parent),
	m_positionProviderPtr(nullptr),
	m_style(S_Bar)
{
}


istd::TSmartPtr<istd::IChangeable> CProductionQualityItem::GetModel()
{
	return m_modelPtr;
}


void CProductionQualityItem::SetModel(istd::TSmartPtr<istd::IChangeable> modelPtr)
{
	m_modelPtr = modelPtr;
}


void CProductionQualityItem::SetScenePositionProvider(IEventScenePositionProvider* providerPtr)
{
	m_positionProviderPtr = providerPtr;
}


void CProductionQualityItem::SetStyle(Style style)
{
	m_style = style;
}


// reimplemented (QGraphicsItem)

QRectF CProductionQualityItem::boundingRect() const
{
	return rect(); 
}


void CProductionQualityItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	Q_ASSERT(m_positionProviderPtr != nullptr);

	CRepresentationProductionSpeedModel* modelPtr = dynamic_cast<CRepresentationProductionSpeedModel*>(m_modelPtr.GetPtr()); 
	if (modelPtr == nullptr || m_positionProviderPtr == nullptr){
		return;
	}

	const imtloggui::CRepresentationProductionSpeedModel::Timeline& timeline = modelPtr->GetTimeline();
	quint64 max = modelPtr->GetMaxCount();
	const CRepresentationProductionSpeedModel::MaxCounters& maxCounters = modelPtr->GetMaxCounters();

	QRectF visibleRect = mapFromScene(scene()->views()[0]->mapToScene(scene()->views()[0]->viewport()->rect())).boundingRect();
	double scaleX = scene()->views().first()->viewportTransform().m11();
	double scaleY = scene()->views().first()->viewportTransform().m22();

	double granularity = modelPtr->GetGranularity();
	double granularityS = modelPtr->GetGranularity() / 1000.;
	qint64 beginTime = modelPtr->CalculateIntervalBeginTime(m_positionProviderPtr->GetVisibleBeginTime().toMSecsSinceEpoch());
	qint64 endTime = modelPtr->CalculateIntervalBeginTime(m_positionProviderPtr->GetVisibleEndTime().toMSecsSinceEpoch());
	beginTime -= modelPtr->GetGranularity();
	beginTime += modelPtr->GetGranularity();

	QRectF r = rect();

	// Prepare point arrays

	m_okPoints.clear();
	m_warningPoints.clear();
	m_nokPoints.clear();
	m_errorsPoints.clear();

	qint64 curTime = beginTime;
	double curPos = mapFromScene(m_positionProviderPtr->GetScenePositionFromTime(QDateTime::fromMSecsSinceEpoch(curTime)), 0).x();

	while (curTime < endTime + granularity){
		double relativeOks = 0;
		double relativeWarnings = 0;
		double relativeNoks = 0;
		double relativeErrors = 0;

		if (m_style == S_Bar){
			if (timeline.contains(curTime)){
				Q_ASSERT(!((timeline[curTime].oks > 0 ||
							timeline[curTime].warnings > 0 ||
							timeline[curTime].noks > 0 ||
							timeline[curTime].errors > 0) ^ (timeline[curTime].count > 0)));

				relativeOks = timeline[curTime].count > 0 ? timeline[curTime].oks * 1. / timeline[curTime].count : 0;
				relativeWarnings = timeline[curTime].count > 0 ? timeline[curTime].warnings * 1. / timeline[curTime].count : 0;
				relativeNoks = timeline[curTime].count > 0 ? timeline[curTime].noks * 1. / timeline[curTime].count : 0;
				relativeErrors = timeline[curTime].count > 0 ? timeline[curTime].errors * 1. / timeline[curTime].count : 0;
			}

			m_okPoints.append(QPointF(curPos + granularityS / 2, r.height() * relativeOks));
			m_warningPoints.append(QPointF(curPos + granularityS / 2, r.height() * relativeWarnings));
			m_nokPoints.append(QPointF(curPos + granularityS / 2, r.height() * relativeNoks));
			m_errorsPoints.append(QPointF(curPos + granularityS / 2, r.height() * relativeErrors));

			if (relativeOks != 0 || relativeWarnings != 0 || relativeNoks != 0 || relativeErrors != 0){
				Q_ASSERT(fabs(relativeOks + relativeWarnings + relativeNoks + relativeErrors - 1) < DBL_EPSILON);
			}
		}

		if (m_style == S_Polyline){
			if (timeline.contains(curTime)){
				relativeOks = maxCounters.oks > 0 ? timeline[curTime].oks * 1. / maxCounters.oks : 0;
				relativeWarnings = maxCounters.warnings > 0 ? timeline[curTime].warnings * 1. / maxCounters.warnings : 0;
				relativeNoks = maxCounters.noks > 0 ? timeline[curTime].noks * 1. / maxCounters.noks : 0;
				relativeErrors = maxCounters.errors > 0 ? timeline[curTime].errors * 1. / maxCounters.errors : 0;
			}

			QPointF pntOks = QPointF(curPos + granularityS / 2, r.bottom() - r.height() * relativeOks);
			QPointF pntWarnings = QPointF(curPos + granularityS / 2, r.bottom() - r.height() * relativeWarnings);
			QPointF pntNoks = QPointF(curPos + granularityS / 2, r.bottom() - r.height() * relativeNoks);
			QPointF pntErrors = QPointF(curPos + granularityS / 2, r.bottom() - r.height() * relativeErrors);

			m_okPoints.append(pntOks);
			m_warningPoints.append(pntWarnings);
			m_nokPoints.append(pntNoks);
			m_errorsPoints.append(pntErrors);
		}

		curTime += granularity;
		curPos += granularityS;
	}

	if (m_style == S_Polyline){
		m_okPoints.append(QPointF(m_okPoints.last().x(), r.bottom()));
		m_okPoints.append(QPointF(m_okPoints.first().x(), r.bottom()));
		m_okPoints.append(m_okPoints.first());
		m_warningPoints.append(QPointF(m_warningPoints.last().x(), r.bottom()));
		m_warningPoints.append(QPointF(m_warningPoints.first().x(), r.bottom()));
		m_warningPoints.append(m_warningPoints.first());
		m_nokPoints.append(QPointF(m_nokPoints.last().x(), r.bottom()));
		m_nokPoints.append(QPointF(m_nokPoints.first().x(), r.bottom()));
		m_nokPoints.append(m_nokPoints.first());
		m_errorsPoints.append(QPointF(m_errorsPoints.last().x(), r.bottom()));
		m_errorsPoints.append(QPointF(m_errorsPoints.first().x(), r.bottom()));
		m_errorsPoints.append(m_errorsPoints.first());
	}

	// Draw

	QTransform savedTransform = painter->transform();
	QTransform transform;

	if (m_style == S_Bar){
		QPen pen;
		pen.setColor(Qt::transparent);
		pen.setCapStyle(Qt::RoundCap);
		pen.setJoinStyle(Qt::RoundJoin);
		pen.setWidth(0);
		painter->setPen(pen);

		quint64 count = m_okPoints.count();
		for (int i = 0; i < count; i++){
			transform = savedTransform;
			transform.translate(m_okPoints[i].x(), 0);
			painter->setTransform(transform);

			pen.setColor(Qt::transparent);
			painter->setPen(pen);

			double minHeight = 3 / scaleY;
			double delta = 0;
			if (m_okPoints[i].y() > 0 && m_okPoints[i].y() < minHeight){
				delta += minHeight - m_okPoints[i].y();
				m_okPoints[i].ry() = minHeight;
			}
			if (m_warningPoints[i].y() > 0 && m_warningPoints[i].y() < minHeight){
				delta += minHeight - m_warningPoints[i].y();
				m_warningPoints[i].ry() = minHeight;
			}
			if (m_nokPoints[i].y() > 0 && m_nokPoints[i].y() < minHeight){
				delta += minHeight - m_nokPoints[i].y();
				m_nokPoints[i].ry() = minHeight;
			}
			if (m_errorsPoints[i].y() > 0 && m_errorsPoints[i].y() < minHeight){
				delta += minHeight - m_errorsPoints[i].y();
				m_errorsPoints[i].ry() = minHeight;
			}

			double maxValue = 0;
			int cat = 0;
			if (m_okPoints[i].y() > maxValue){
				maxValue = m_okPoints[i].y();
				cat = 0;
			}
			if (m_warningPoints[i].y() > maxValue){
				maxValue = m_warningPoints[i].y();
				cat = 1;
			}
			if (m_nokPoints[i].y() > maxValue){
				maxValue = m_nokPoints[i].y();
				cat = 2;
			}
			if (m_errorsPoints[i].y() > maxValue){
				maxValue = m_errorsPoints[i].y();
				cat = 1;
			}

			switch (cat){
			case 0:
				if (m_okPoints[i].ry() > 0){
					m_okPoints[i].ry() -= delta;
				}
				break;
			case 1:
				if (m_warningPoints[i].ry() > 0){
					m_warningPoints[i].ry() -= delta;
				}
				break;
			case 2:
				if (m_nokPoints[i].ry() > 0){
					m_nokPoints[i].ry() -= delta;
				}
				break;
			case 3:
				if (m_errorsPoints[i].ry() > 0){
					m_errorsPoints[i].ry() -= delta;
				}
				break;
			}

			double offset = r.bottom();

			//QLinearGradient gradient;

			double barLeftSide = -granularityS / 2  - 1 / scaleX;
			double barWitdh = granularityS + 1 / scaleX;

			if (m_okPoints[i].y() > 0){
				//gradient = QLinearGradient(0, offset, 0, (offset - m_okPoints[i].y()));
				//gradient.setColorAt(0, QColor("#1000FF00"));
				//gradient.setColorAt(1, QColor("#FF00FF00"));
				painter->setBrush(Qt::green);
				painter->setPen(pen);
				painter->drawRect(QRectF(
					barLeftSide, offset,
					barWitdh, -m_okPoints[i].y()));

				offset -= m_okPoints[i].y();
			}

			if (m_warningPoints[i].y() > 0){
				//gradient = QLinearGradient(0, offset, 0, (offset - m_warningPoints[i].y()));
				//gradient.setColorAt(0, QColor("#10FFFF00"));
				//gradient.setColorAt(1, QColor("#FFFFFF00"));
				painter->setBrush(Qt::yellow);
				painter->setPen(pen);
				painter->drawRect(QRectF(
					barLeftSide, offset,
					barWitdh, -m_warningPoints[i].y()));

				offset -= m_warningPoints[i].y();
			}

			if (m_nokPoints[i].y() > 0){
				//gradient = QLinearGradient(0, offset, 0, (offset - m_nokPoints[i].y()));
				//gradient.setColorAt(0, QColor("#10FF0000"));
				//gradient.setColorAt(1, QColor("#FFFF0000"));
				painter->setBrush(Qt::red);
				painter->setPen(pen);
				painter->drawRect(QRectF(
					barLeftSide, offset,
					barWitdh, -m_nokPoints[i].y()));

				offset -= m_nokPoints[i].y();
			}

			if (m_errorsPoints[i].y() > 0){
				//gradient = QLinearGradient(0, offset, 0, (offset - m_errorsPoints[i].y()));
				//gradient.setColorAt(0, QColor("#10A00000"));
				//gradient.setColorAt(1, QColor("#FFA00000"));
				painter->setBrush(Qt::darkRed);
				painter->setPen(pen);
				painter->drawRect(QRectF(
					barLeftSide, offset,
					barWitdh, -m_errorsPoints[i].y()));
			}

			//pen.setColor(Qt::black);
			//painter->setPen(pen);
			//painter->setBrush(Qt::transparent);
			//painter->drawRect(QRectF(
			//	barLeftSide, r.top(),
			//	barWitdh, r.height()));
		}
	}

	if (m_style == S_Polyline){
		painter->setPen(Qt::transparent);
		QLinearGradient gradient(0, r.bottom(), 0, r.top());
		gradient.setColorAt(0, QColor("#1000FF00"));
		gradient.setColorAt(1, QColor("#C000FF00"));
		painter->setBrush(gradient);
		painter->drawPolygon(m_okPoints.data(), m_okPoints.count());
		gradient.setColorAt(0, QColor("#10FFFF00"));
		gradient.setColorAt(1, QColor("#C0FFFF00"));
		painter->setBrush(gradient);
		painter->drawPolygon(m_warningPoints.data(), m_warningPoints.count());
		gradient.setColorAt(0, QColor("#10FF0000"));
		gradient.setColorAt(1, QColor("#C0FF0000"));
		painter->setBrush(gradient);
		painter->drawPolygon(m_nokPoints.data(), m_nokPoints.count());
		gradient.setColorAt(0, QColor("#10800000"));
		gradient.setColorAt(1, QColor("#C0800000"));
		painter->setBrush(gradient);
		painter->drawPolygon(m_errorsPoints.data(), m_errorsPoints.count());

		QPen pen;
		pen.setColor(Qt::red);
		pen.setWidth(2);
		pen.setCapStyle(Qt::RoundCap);
		pen.setJoinStyle(Qt::RoundJoin);

		quint64 count = m_okPoints.count() - 3;
		for (int i = 1; i < count; i++){
			transform = savedTransform;
			transform.translate(m_okPoints[i].x(), 0);
			transform.scale(1 / scaleX, 1 / scaleY);
			painter->setTransform(transform);

			pen.setColor(QColor("#FF00FF00"));
			painter->setPen(pen);
			if (m_okPoints[i - 1].y() != r.bottom() || m_okPoints[i].y() != r.bottom()){
				painter->setPen(pen);
				painter->drawLine(
					0, m_okPoints[i].y() * scaleY,
					(m_okPoints[i - 1].x() - m_okPoints[i].x()) * scaleX, m_okPoints[i - 1].y() * scaleY);
			}

			pen.setColor(QColor("#FFFFFF00"));
			painter->setPen(pen);
			if (m_warningPoints[i - 1].y() != r.bottom() || m_warningPoints[i].y() != r.bottom()){
				painter->setPen(pen);
				painter->drawLine(
					0, m_warningPoints[i].y() * scaleY,
					(m_warningPoints[i - 1].x() - m_warningPoints[i].x()) * scaleX, m_warningPoints[i - 1].y() * scaleY);
			}

			pen.setColor(QColor("#FFFF0000"));
			painter->setPen(pen);
			if (m_nokPoints[i - 1].y() != r.bottom() || m_nokPoints[i].y() != r.bottom()){
				painter->setPen(pen);
				painter->drawLine(
					0, m_nokPoints[i].y() * scaleY,
					(m_nokPoints[i - 1].x() - m_nokPoints[i].x()) * scaleX, m_nokPoints[i - 1].y() * scaleY);
			}

			pen.setColor(QColor("#FF800000"));
			painter->setPen(pen);
			if (m_errorsPoints[i - 1].y() != r.bottom() || m_errorsPoints[i].y() != r.bottom()){
				painter->setPen(pen);
				painter->drawLine(
					0, m_errorsPoints[i].y() * scaleY,
					(m_errorsPoints[i - 1].x() - m_errorsPoints[i].x()) * scaleX, m_errorsPoints[i - 1].y() * scaleY);
			}
		}
	}

	painter->setPen(QPen(Qt::black));

	QFont font = QFont("Helvetica");
	font.setPointSize(12);
	painter->setFont(font);

	QFontMetrics metricsB(font);

	QString textOks = QObject::tr("Oks");
	QString textWarnings = QObject::tr("Warnings");
	QString textNoks = QObject::tr("Noks");
	QString textErrors = QObject::tr("Errors");

	QRectF rectOks = metricsB.boundingRect(textOks);
	QRectF rectWarnings = metricsB.boundingRect(textWarnings);
	QRectF rectNoks = metricsB.boundingRect(textNoks);
	QRectF rectErrors = metricsB.boundingRect(textErrors);

	transform = savedTransform;
	transform.translate(visibleRect.right() - (60 + rectWarnings.width()) / scaleX, 0);
	transform.scale(1 / scaleX, 1 / scaleY);
	painter->setTransform(transform);

	double offset;
	QRectF rect;

	offset = -rectWarnings.height() * 2 - 39;
	rect = QRectF(0, offset, 20 + rectWarnings.width(), rectWarnings.height() + 15);
	painter->setBrush(Qt::darkRed);
	painter->drawRoundedRect(rect, 3, 3);
	painter->drawText(rect, Qt::AlignVCenter | Qt::AlignHCenter, "Errors");

	offset = -rectWarnings.height() - 18;
	rect = QRectF(0, offset, 20 + rectWarnings.width(), rectWarnings.height() + 15);
	painter->setBrush(Qt::red);
	painter->drawRoundedRect(rect, 3, 3);
	painter->drawText(rect, Qt::AlignVCenter | Qt::AlignHCenter, "Noks");

	offset = 3;
	rect = QRectF(0, offset, 20 + rectWarnings.width(), rectWarnings.height() + 15);
	painter->setBrush(Qt::yellow);
	painter->drawRoundedRect(rect, 3, 3);
	painter->drawText(rect, Qt::AlignVCenter | Qt::AlignHCenter, "Warnings");

	offset = rectWarnings.height() + 24;
	rect = QRectF(0, offset, 20 + rectWarnings.width(), rectWarnings.height() + 15);
	painter->setBrush(Qt::green);
	painter->drawRoundedRect(rect, 3, 3);
	painter->drawText(rect, Qt::AlignVCenter | Qt::AlignHCenter, "Oks");
}


void CProductionQualityItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_ASSERT(m_positionProviderPtr != nullptr);

	CRepresentationProductionSpeedModel* modelPtr = dynamic_cast<CRepresentationProductionSpeedModel*>(m_modelPtr.GetPtr());
	if (modelPtr == nullptr || m_positionProviderPtr == nullptr){
		setToolTip("");
		return;
	}

	QPointF pos = event->pos();

	const imtloggui::CRepresentationProductionSpeedModel::Timeline& timeline = modelPtr->GetTimeline();

	QString tooltip;
	qint64 timeStamp = m_positionProviderPtr->GetTimeFromScenePosition(mapToScene(pos).x()).toMSecsSinceEpoch();
	qint64 beginTime = modelPtr->CalculateIntervalBeginTime(timeStamp);

	if (timeline.contains(beginTime) ){
		tooltip = QObject::tr("Oks: %1\nWarnings: %2\nNoks: %3\nErrors: %4")
					.arg(timeline[beginTime].oks)
					.arg(timeline[beginTime].warnings)
					.arg(timeline[beginTime].noks)
					.arg(timeline[beginTime].errors);
	}
	else{
		tooltip = QObject::tr("Oks: 0\nWarnings: 0\nNoks: 0\nErrors: 0");
	}

	QGraphicsView *viewPtr = scene()->views()[0];
	viewPtr->setToolTipDuration(1000);
	viewPtr->setToolTip(tooltip);
}


bool CProductionQualityItem::IsEmptyInterval(int i)
{
	return !(m_okPoints[i].y() != 0 || m_warningPoints[i].y() != 0 || m_nokPoints[i].y() != 0 || m_errorsPoints[i].y() != 0);
}


} // namespace imtloggui

