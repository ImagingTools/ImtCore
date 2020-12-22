#include <imtloggui/CProductionSpeedItem.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsSceneHoverEvent>


namespace imtloggui
{


// public methods

CProductionSpeedItem::CProductionSpeedItem(QGraphicsItem* parent)
	:BaseClass(parent),
	m_positionProviderPtr(nullptr)
{
}


istd::TSmartPtr<istd::IChangeable> CProductionSpeedItem::GetModel()
{
	return m_modelPtr;
}


void CProductionSpeedItem::SetModel(istd::TSmartPtr<istd::IChangeable> modelPtr)
{
	m_modelPtr = modelPtr;
}


void CProductionSpeedItem::SetScenePositionProvider(IEventScenePositionProvider* providerPtr)
{
	m_positionProviderPtr = providerPtr;
}


// reimplemented (QGraphicsItem)

QRectF CProductionSpeedItem::boundingRect() const
{
	return rect(); 
}


void CProductionSpeedItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	Q_ASSERT(m_positionProviderPtr != nullptr);

	CRepresentationProductionSpeedModel* modelPtr = dynamic_cast<CRepresentationProductionSpeedModel*>(m_modelPtr.GetPtr()); 
	if (modelPtr == nullptr || m_positionProviderPtr == nullptr){
		return;
	}

	const imtloggui::CRepresentationProductionSpeedModel::Timeline& timeline = modelPtr->GetTimeline();
	QList<qint64> timelineKeys = timeline.keys();

	QRectF visibleRect = mapFromScene(scene()->views()[0]->mapToScene(scene()->views()[0]->viewport()->rect())).boundingRect();
	double scaleX = scene()->views().first()->viewportTransform().m11();
	double scaleY = scene()->views().first()->viewportTransform().m22();

	double granularity = modelPtr->GetGranularity();
	double granularityS = modelPtr->GetGranularity() / 1000.;
	qint64 beginTime = m_positionProviderPtr->GetVisibleBeginTime().toMSecsSinceEpoch();
	beginTime -= beginTime % modelPtr->GetGranularity();
	qint64 endTime = m_positionProviderPtr->GetVisibleEndTime().toMSecsSinceEpoch();

	QRectF r = rect();

	QVector<QPointF> points;
	points.reserve((endTime - beginTime) / granularity  + 10);

	quint64 max = modelPtr->GetMaxCount();
	
	qint64 curTime = beginTime;
	double curPos = mapFromScene(m_positionProviderPtr->GetScenePositionFromTime(QDateTime::fromMSecsSinceEpoch(beginTime)), 0).x();

	while (curTime < endTime){
		double relativeY = 0;
		if (timeline.contains(curTime)){
			relativeY= timeline[curTime].count * 1.0 / max;
		}

		QPointF pnt1 = QPointF(curPos, (r.bottom() - (r.bottom() - r.top()) * relativeY));
		QPointF pnt2 = QPointF(curPos + granularityS, (r.bottom() - (r.bottom() - r.top()) * relativeY));
		if (curTime + granularity > endTime){
			pnt2.rx() -= ((curTime + granularity) - endTime) / 1000.;
		}

		points.append(pnt1);
		points.append(pnt2);

		curTime += granularity;
		curPos += granularityS;
	}

	points.append(QPointF(points.last().x(), r.bottom()));
	points.append(QPointF(points.first().x(), r.bottom()));
	points.append(points.first());

	QTransform savedTransform = painter->transform();
	QTransform transform;

	painter->setTransform(savedTransform);
	painter->setPen(Qt::transparent);
	QLinearGradient grad(0, r.bottom(), 0, r.top());
	grad.setColorAt(0, QColor("#1BFFFF00"));
	grad.setColorAt(0.5, QColor("#9BFFFF00"));
	grad.setColorAt(1, QColor("#FFFF8000"));
	painter->setBrush(grad);
	painter->drawPolygon(points.data(), points.count());

	QPen pen;
	pen.setColor(Qt::red);
	pen.setWidth(2);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	painter->setPen(pen);

	quint64 count = points.count() - 3;
	QPointF prevPnt;
	prevPnt.rx() = -1;
	for (int i = 0; i < count; i += 2){
		transform = savedTransform;
		transform.translate(points[i].x(), 0);
		transform.scale(1 / scaleX, 1 / scaleY);
		painter->setTransform(transform);

		if (prevPnt.x() == -1){
			prevPnt = QPointF(0, r.bottom());
		}

		painter->drawLine(
			0, points[i].y() * scaleY,
			prevPnt.x() * scaleX, prevPnt.y() * scaleY);
		painter->drawLine(
			0, points[i].y() * scaleY,
			(points[i + 1].x() - points[i].x()) * scaleX, points[i].y() * scaleY);
		prevPnt = QPointF(0, points[i + 1].y());
	}

	painter->setPen(QPen(Qt::black));
	painter->setBrush(Qt::transparent);

	QFont font = QFont("Helvetica");
	font.setPointSize(12);
	painter->setFont(font);

	QFontMetrics metricsB(font);
	QString textB = QObject::tr("0 pcs/h");
	QRectF rectB = metricsB.boundingRect(textB);

	QFontMetrics metricsT(font);
	QString textT = QObject::tr("%1 pcs/h").arg(max * 360000. / modelPtr->GetGranularity());
	QRectF rectT = metricsT.boundingRect(textT);

	transform = savedTransform;
	transform.translate(visibleRect.right() - (20 + rectB.width()) / scaleX, r.bottom() + rectB.height() * 2 / 7 / scaleY);
	transform.scale(1 / scaleX, 1 / scaleY);
	painter->setTransform(transform);
	painter->setPen(QPen(Qt::black));
	painter->setBrush(QColor("#A0A0FF"));
	painter->drawRoundedRect(rectB.adjusted(-10, -10, 10, 10), 3, 3);
	painter->setPen(QPen(Qt::black));
	painter->setBrush(Qt::transparent);
	painter->drawText(rectB, Qt::AlignCenter, textB);

	transform = savedTransform;
	transform.translate(visibleRect.right() - (20 + rectT.width()) / scaleX, r.top() + rectT.height() * 2 / 7 / scaleY);
	transform.scale(1 / scaleX, 1 / scaleY);
	painter->setTransform(transform);
	painter->setPen(QPen(Qt::black));
	painter->setBrush(QColor("#A0A0FF"));
	painter->drawRoundedRect(rectT.adjusted(-10, -10, 10, 10), 3, 3);
	painter->setPen(QPen(Qt::black));
	painter->setBrush(Qt::transparent);
	painter->drawText(rectT, Qt::AlignCenter, textT);
}


void CProductionSpeedItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
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
	qint64 timestamp = m_positionProviderPtr->GetTimeFromScenePosition(mapToScene(pos).x()).toMSecsSinceEpoch();
	timestamp -= timestamp % modelPtr->GetGranularity();

	if (timeline.contains(timestamp) ){
		tooltip = QObject::tr("%1 pcs/h").arg(timeline[timestamp].count * 360000. / modelPtr->GetGranularity());
	}
	else{
		tooltip = QObject::tr("0 pcs/h");
	}

	QGraphicsView *viewPtr = scene()->views()[0];
	viewPtr->setToolTipDuration(1000);
	viewPtr->setToolTip(tooltip);
}


} // namespace imtloggui

