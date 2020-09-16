#include <imtloggui/CClusterItem.h>


// Qt includes
#include <QtCore/QRectF>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


// public methods

CClusterItem::CClusterItem()
	:m_rect(QRect())
{
	for (int i = 0; i < 5; i++){
		m_counters[i] = 0;
	}
}


void CClusterItem::SetParams(const QSize& size, QGraphicsItem *parent)
{
	BaseClass::SetParams(parent);

	m_rect = QRect(-size.width() / 2., -size.height() / 2., size.width(), size.height());
}


// reimplemented (imtloggui::IEventItemCluster)

void CClusterItem::Attach(IEventItem* itemPtr)
{
	QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(itemPtr);

	QPointF pos = graphicsItemPtr->parentItem()->mapToScene(graphicsItemPtr->pos());
	graphicsItemPtr->setParentItem(this);
	graphicsItemPtr->setPos(this->mapFromScene(pos));
	graphicsItemPtr->setVisible(false);

	int category = itemPtr->GetInformationProvider()->GetInformationCategory();
	m_counters[category]++;

	switch (category){
	case istd::IInformationProvider::IC_NONE:
		SetMetaInfo(MIT_NONE_COUNTER, QString("%1 Events").arg(m_counters[category]));
		break;
	case istd::IInformationProvider::IC_INFO:
		SetMetaInfo(MIT_INFO_COUNTER, QString("%1 Events").arg(m_counters[category]));
		break;
	case istd::IInformationProvider::IC_WARNING:
		SetMetaInfo(MIT_WARNING_COUNTER, QString("%1 Events").arg(m_counters[category]));
		break;
	case istd::IInformationProvider::IC_ERROR:
		SetMetaInfo(MIT_ERROR_COUNTER, QString("%1 Events").arg(m_counters[category]));
		break;
	case istd::IInformationProvider::IC_CRITICAL:
		SetMetaInfo(MIT_CRITICAL_COUNTER, QString("%1 Events").arg(m_counters[category]));
		break;
	}
}


void CClusterItem::DetachAll()
{
	for (QGraphicsItem* itemPtr : childItems()){
		QPointF pos = mapToScene(itemPtr->pos());
		itemPtr->setParentItem(parentItem());
		itemPtr->setPos(parentItem()->mapFromScene(pos));
		itemPtr->setVisible(false);
	}
}


// reimplemented (idoc::IDocumentMetaInfo)

idoc::IDocumentMetaInfo::MetaInfoTypes CClusterItem::GetMetaInfoTypes(bool allowReadOnly) const
{
	if (allowReadOnly){
		return {MIT_NONE_COUNTER, MIT_INFO_COUNTER, MIT_WARNING_COUNTER, MIT_ERROR_COUNTER, MIT_CRITICAL_COUNTER};
	}

	return {};
}


QVariant CClusterItem::GetMetaInfo(int metaInfoType) const
{
	switch (metaInfoType){
	case MIT_NONE_COUNTER:
		return QString("%1 Events").arg(m_counters[0]);
	case MIT_INFO_COUNTER:
		return QString("%1 Events").arg(m_counters[1]);
	case MIT_WARNING_COUNTER:
		return QString("%1 Events").arg(m_counters[2]);
	case MIT_ERROR_COUNTER:
		return QString("%1 Events").arg(m_counters[3]);
	case MIT_CRITICAL_COUNTER:
		return QString("%1 Events").arg(m_counters[4]);
	default:
		return QVariant();
	}
}


QString CClusterItem::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case MIT_NONE_COUNTER:
		return QObject::tr("NONE");
	case MIT_INFO_COUNTER:
		return QObject::tr("INFO");
	case MIT_WARNING_COUNTER:
		return QObject::tr("WARNING");
	case MIT_ERROR_COUNTER:
		return QObject::tr("ERROR");
	case MIT_CRITICAL_COUNTER:
		return QObject::tr("CRITICAL");
	default:
		return QString();
	}
}


// reimplemented (QGraphicsItem)

QRectF CClusterItem::boundingRect() const
{
	return m_rect;
}


void CClusterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	painter->setRenderHints(0);

	painter->fillRect(m_rect, Qt::white);

	painter->save();
	if (isSelected()){
		painter->setPen(QPen(Qt::red, 2));
	}
	else{
		painter->setPen(QPen(Qt::black, 2));
	}
	painter->drawRoundedRect(m_rect, 2, 2);
	painter->restore();

	QPen pen;
	QBrush brush;

	if (isSelected()){
		pen.setColor(Qt::red);
	}
	else{
		pen.setColor(Qt::black);
	}
	brush.setColor(Qt::gray);

	painter->setPen(pen);

	int height = m_rect.height() / (5 + 6 * 0.2);
	int space = height * 0.2;

	QRect ellipse(m_rect.topLeft() + QPoint(space, space), QSize(height, height));
	QPainterPath path;

	QRect labelRect;
	QFont font = painter->font();
	font.setPointSize(height * 0.7);

	path.addEllipse(ellipse);
	painter->fillPath(path, Qt::gray);
	painter->drawEllipse(ellipse);
	labelRect = ellipse.translated(height + space, 0);
	labelRect.setWidth(m_rect.width() - height - 2 * space);
	painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, QObject::tr("%1 Events").arg(m_counters[0]));

	ellipse.translate(0, height + space);
	path = QPainterPath();
	path.addEllipse(ellipse);
	painter->fillPath(path, Qt::green);
	painter->drawEllipse(ellipse);
	labelRect = ellipse.translated(height + space, 0);
	labelRect.setWidth(m_rect.width() - height - 2 * space);
	painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, QObject::tr("%1 Events").arg(m_counters[1]));

	ellipse.translate(0, height + space);
	path = QPainterPath();
	path.addEllipse(ellipse);
	painter->fillPath(path, Qt::yellow);
	painter->drawEllipse(ellipse);
	labelRect = ellipse.translated(height + space, 0);
	labelRect.setWidth(m_rect.width() - height - 2 * space);
	painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, QObject::tr("%1 Events").arg(m_counters[2]));

	ellipse.translate(0, height + space);
	path = QPainterPath();
	path.addEllipse(ellipse);
	painter->fillPath(path, Qt::red);
	painter->drawEllipse(ellipse);
	labelRect = ellipse.translated(height + space, 0);
	labelRect.setWidth(m_rect.width() - height - 2 * space);
	painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, QObject::tr("%1 Events").arg(m_counters[3]));

	ellipse.translate(0, height + space);
	path = QPainterPath();
	path.addEllipse(ellipse);
	painter->fillPath(path, Qt::darkRed);
	painter->drawEllipse(ellipse);
	labelRect = ellipse.translated(height + space, 0);
	labelRect.setWidth(m_rect.width() - height - 2 * space);
	painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, QObject::tr("%1 Events").arg(m_counters[4]));
}


} // namespace imtloggui


