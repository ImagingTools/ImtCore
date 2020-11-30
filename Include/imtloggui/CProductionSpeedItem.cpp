#include <imtloggui/CProductionSpeedItem.h>


// Qt includes
#include <QtCore/QDebug>
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
	CRepresentationProductionSpeedModel* modelPtr = dynamic_cast<CRepresentationProductionSpeedModel*>(m_modelPtr.GetPtr()); 
	if (modelPtr == nullptr || m_positionProviderPtr == nullptr){
		return;
	}

	int count = modelPtr->GetStatisticsItemCount();
	if (count == 0){
		return;
	}

	double scaleX = scene()->views().first()->viewportTransform().m11();
	double scaleY = scene()->views().first()->viewportTransform().m22();

	double granularity = modelPtr->GetGranularity() / 1000;
	uint64_t beginTime = modelPtr->GetStatisticsItem(0).time;
	double beginX = mapFromScene(m_positionProviderPtr->GetScenePositionFromTime(QDateTime::fromMSecsSinceEpoch(beginTime)), 0).x();

	uint64_t max = 0;
	for (int i = 0; i < count; i++){
		if (max < modelPtr->GetStatisticsItem(i).info.count){
			max = modelPtr->GetStatisticsItem(i).info.count;
		}
	}

	QRectF r = rect();

	QVector<QPointF> points;
	points.reserve(count + 5);

	m_nodes.clear();
	m_nodes.reserve(count);

	QRectF visibleRect = mapFromScene(scene()->views()[0]->mapToScene(scene()->views()[0]->viewport()->rect())).boundingRect();
	points.append(QPointF(visibleRect.x(), r.bottom()));
	points.append(QPointF(0, r.bottom()));

	for (int i = 0; i < count; i++){
		double relativeY = modelPtr->GetStatisticsItem(i).info.count * 1.0 / max;
		QPointF pnt = QPointF(i * granularity, (r.bottom() - (r.bottom() - r.top()) * relativeY));
		points.append(pnt);

		m_nodes.append(QRectF(pnt.x() - 5, pnt.y() - 5, 10, 10));
	}

	points.append(QPointF(points.last().x() + granularity, r.bottom()));
	points.append(QPointF(visibleRect.right(), r.bottom()));
	points.append(QPointF(visibleRect.left(), r.bottom()));

	QTransform savedTransform = painter->transform();
	QTransform transform;

	painter->setTransform(savedTransform);
	painter->setPen(Qt::transparent);
	QLinearGradient grad(0, r.bottom(), 0, r.top());
	grad.setColorAt(0, QColor("#1BFFFF00"));
	grad.setColorAt(1, QColor("#9BFFFF00"));
	painter->setBrush(grad);
	painter->drawPolygon(points.data(), points.count());

	QPen pen;
	pen.setColor(Qt::red);
	pen.setWidth(2);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	painter->setPen(pen);

	for (int i = 0; i < 3 + count; i++){
		transform = savedTransform;
		transform.translate(points[i].x(), 0);
		transform.scale(1 / scaleX, 1 / scaleY);
		painter->setTransform(transform);
		painter->drawLine(
					0, points[i].y() * scaleY,
					(points[i + 1].x() - points[i].x()) * scaleX, points[i + 1].y() * scaleY);
	}

	pen.setColor(Qt::transparent);
	pen.setWidth(2);
	painter->setPen(pen);
	painter->setBrush(QColor("#500000FF"));

	for (int i = 2; i < 2 + count; i++){
		transform = savedTransform;
		transform.translate(points[i].x(), points[i].y());
		transform.scale(1 / scaleX, 1 / scaleY);
		painter->setTransform(transform);
		painter->drawEllipse(QRectF(
			-5, -5,
			10, 10));
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
	CRepresentationProductionSpeedModel* modelPtr = dynamic_cast<CRepresentationProductionSpeedModel*>(m_modelPtr.GetPtr());
	if (modelPtr == nullptr || m_positionProviderPtr == nullptr){
		setToolTip("");
		return;
	}

	QPointF pos = event->pos();
	double scaleX = scene()->views().first()->viewportTransform().m11();
	double scaleY = scene()->views().first()->viewportTransform().m22();

	for (int i = 0; i < m_nodes.count(); i++){
		QRectF rect = m_nodes[i];
		QPointF pnt = rect.center();
		rect.setLeft(pnt.x() - 5 / scaleX);
		rect.setRight(pnt.x() + 5 / scaleX);
		rect.setTop(pnt.y() - 5 / scaleY);
		rect.setBottom(pnt.y() + 5 / scaleY);

		if (rect.contains(pos)){
			QString tooltip = QObject::tr("%1 pcs/h").arg(modelPtr->GetStatisticsItem(i).info.count * 360000. / modelPtr->GetGranularity());
			if (toolTip() != tooltip){
				setToolTip(tooltip);
			}

			return;
		}
	}

	setToolTip("");
}


} // namespace imtloggui


