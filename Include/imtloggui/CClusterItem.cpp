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


// reimplemented (imtloggui::IEventItemCluster)

void CClusterItem::SetParams(const QSize& size)
{
	m_rect = QRect(-size.width() / 2., -size.height() / 2., size.width(), size.height());
}


void CClusterItem::Attach(IEventItem* itemPtr)
{
	QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(itemPtr);

	QPointF pos = graphicsItemPtr->parentItem()->mapToScene(graphicsItemPtr->pos());
	graphicsItemPtr->setParentItem(this);
	graphicsItemPtr->setPos(this->mapFromScene(pos));
	graphicsItemPtr->setVisible(false);

	m_counters[itemPtr->GetInformationProvider()->GetInformationCategory()]++;
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
	MetaInfoTypes types{
				idoc::IDocumentMetaInfo::MIT_USER + 0,
				idoc::IDocumentMetaInfo::MIT_USER + 1,
				idoc::IDocumentMetaInfo::MIT_USER + 2,
				idoc::IDocumentMetaInfo::MIT_USER + 3,
				idoc::IDocumentMetaInfo::MIT_USER + 4};

	return types;
}


QVariant CClusterItem::GetMetaInfo(int metaInfoType) const
{
	metaInfoType -= idoc::IDocumentMetaInfo::MIT_USER;
	if (metaInfoType >= 0 && metaInfoType < 5){
		QObject::tr("%1 Events").arg(m_counters[metaInfoType]);
	}

	return QVariant();
}


bool CClusterItem::SetMetaInfo(int metaInfoType, const QVariant& metaInfo)
{
	return false;
}


QString CClusterItem::GetMetaInfoName(int metaInfoType) const
{
	metaInfoType -= idoc::IDocumentMetaInfo::MIT_USER;
	switch (metaInfoType){
	case 0:
		return QObject::tr("NONE");
	case 1:
		return QObject::tr("INFO");
	case 2:
		return QObject::tr("WARNING");
	case 3:
		return QObject::tr("ERROR");
	case 4:
		return QObject::tr("CRITICAL");
	default:
		return QString();
	}
}


QString CClusterItem::GetMetaInfoDescription(int metaInfoType) const
{
	return QString();
}


bool CClusterItem::IsMetaInfoWritable(int metaInfoType) const
{
	return false;
}


// reimplemented (QGraphicsItem)

QRectF CClusterItem::boundingRect() const
{
	return m_rect;
}


void CClusterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setRenderHints(0);

	painter->fillRect(m_rect, Qt::white);

	painter->save();
	painter->setPen(QPen(Qt::black, 2));
	painter->drawRoundedRect(m_rect, 2, 2);
	painter->restore();

	QPen pen;
	QBrush brush;

	pen.setColor(Qt::black);
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
	path.clear();
	path.addEllipse(ellipse);
	painter->fillPath(path, Qt::green);
	painter->drawEllipse(ellipse);
	labelRect = ellipse.translated(height + space, 0);
	labelRect.setWidth(m_rect.width() - height - 2 * space);
	painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, QObject::tr("%1 Events").arg(m_counters[1]));

	ellipse.translate(0, height + space);
	path.clear();
	path.addEllipse(ellipse);
	painter->fillPath(path, Qt::yellow);
	painter->drawEllipse(ellipse);
	labelRect = ellipse.translated(height + space, 0);
	labelRect.setWidth(m_rect.width() - height - 2 * space);
	painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, QObject::tr("%1 Events").arg(m_counters[2]));

	ellipse.translate(0, height + space);
	path.clear();
	path.addEllipse(ellipse);
	painter->fillPath(path, Qt::red);
	painter->drawEllipse(ellipse);
	labelRect = ellipse.translated(height + space, 0);
	labelRect.setWidth(m_rect.width() - height - 2 * space);
	painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, QObject::tr("%1 Events").arg(m_counters[3]));

	ellipse.translate(0, height + space);
	path.clear();
	path.addEllipse(ellipse);
	painter->fillPath(path, Qt::darkRed);
	painter->drawEllipse(ellipse);
	labelRect = ellipse.translated(height + space, 0);
	labelRect.setWidth(m_rect.width() - height - 2 * space);
	painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, QObject::tr("%1 Events").arg(m_counters[4]));
}


} // namespace imtloggui


