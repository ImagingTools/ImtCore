#include <imtloggui/CProductionSpeedItem.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>


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

	uint64_t granularity = modelPtr->GetGranularity() / 1000;
	uint64_t beginTime = modelPtr->GetStatisticsItem(0).time;
	double beginX = mapFromScene(m_positionProviderPtr->GetScenePositionFromTime(QDateTime::fromMSecsSinceEpoch(beginTime)), 0).x();

	QTransform savedTransform = painter->transform();
	QTransform transform;

	uint64_t max = 0;
	for (int i = 0; i < count; i++){
		if (max < modelPtr->GetStatisticsItem(i).info.count){
			max = modelPtr->GetStatisticsItem(i).info.count;
		}
	}

	QVector<double> values;
	values.reserve(count);
	for (int i = 0; i < count; i++){
		values.append(modelPtr->GetStatisticsItem(i).info.count * 1.0 / max);
	}

	QPen pen;
	pen.setColor(Qt::red);
	pen.setWidth(0);
	painter->setPen(pen);
	QRectF r = rect();

	for (int i = 0; i < count - 1; i++){
		transform = savedTransform;
		transform.translate(i * granularity, 0);
		transform.scale(1 / scaleX, 1 / scaleY);
		painter->setTransform(transform);
		painter->drawLine(
					0, (r.bottom() - (r.bottom() - r.top()) * values[i]) * scaleY,
					granularity * scaleX, (r.bottom() - (r.bottom() - r.top()) * values[i + 1]) * scaleY);

	}

	transform = savedTransform;
	transform.translate((values.count() - 1) * granularity, 0);
	transform.scale(1 / scaleX, 1 / scaleY);
	painter->setTransform(transform);
	painter->drawLine(
				0, (r.bottom() - (r.bottom() - r.top()) * (values.back())) * scaleY,
				granularity * scaleX, r.bottom() * scaleY);


	//QVector<QPointF> points;
	//points.resize(m_values.count() + 1);
	//
}


} // namespace imtloggui


