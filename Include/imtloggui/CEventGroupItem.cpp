#include <imtloggui/CEventGroupItem.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


// public methods

CEventGroupItem::CEventGroupItem(QGraphicsItem* parent)
	:BaseClass(parent)
{
	m_backgroundColor = QColor("#00000000");
	m_name = QObject::tr("Noname");
	m_labelPtr = new CEventGroupLabelItem();
	m_labelPtr->setZValue(1);
}


CEventGroupItem::~CEventGroupItem()
{
	if (scene() != nullptr){
		scene()->removeItem(m_labelPtr);
	}

	delete m_labelPtr;
}


void CEventGroupItem::SetBackgroundColor(const QColor& color)
{
	m_backgroundColor = color;
	m_labelPtr->SetBackgroundColor(color);
}


void CEventGroupItem::SetGroupName(const QString& name)
{
	m_name = name;
	m_labelPtr->SetGroupName(name);
}


void CEventGroupItem::OnViewPortChanged()
{
	update(GetItemVisibleRect());

	QRectF visibleRect = GetSceneVisibleRect();
	
	QPointF labelOrigin(visibleRect.left(), pos().y() + rect().center().y());
	//if (m_labelPtr->pos() != labelOrigin){
	m_labelPtr->setPos(-1000000000, 0);
	m_labelPtr->setPos(labelOrigin);
//}

	prepareGeometryChange();
}


// reimplemented (QGraphicsItem)

QRectF CEventGroupItem::boundingRect() const
{
	if (m_labelPtr->scene() == nullptr){
		scene()->addItem(m_labelPtr);
		m_labelPtr->SetHeight(rect().height());
	}

	QRectF visibleRect = mapRectFromScene(GetSceneVisibleRect());
	QRectF itemRect = rect().intersected(visibleRect);

	if (visibleRect.left() < itemRect.left()){
		itemRect.setLeft(visibleRect.left());
	}

	if (visibleRect.right() > itemRect.right()){
		itemRect.setRight(visibleRect.right());
	}

	return itemRect;
}


void CEventGroupItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	QBrush brush(m_backgroundColor);

	painter->save();
	painter->setBrush(brush);
	painter->setPen(Qt::NoPen);
	painter->drawRect(boundingRect());
	painter->restore();
}


// private methods

double CEventGroupItem::GetCurrentScaleX() const
{
	return scene()->views().first()->viewportTransform().m11();
}


double CEventGroupItem::GetCurrentScaleY() const
{
	return scene()->views().first()->viewportTransform().m22();
}


QRectF CEventGroupItem::GetSceneVisibleRect() const
{
	QGraphicsView* viewPtr = scene()->views().first();

	QRect viewportRect = viewPtr->viewport()->rect();

	QRectF visibleSceneRect = viewPtr->mapToScene(viewportRect).boundingRect();

	return visibleSceneRect;
}


QRectF CEventGroupItem::GetItemVisibleRect() const
{
	QRectF sceneVisibleRect = mapRectFromScene(GetSceneVisibleRect());
	//QRectF itemVisibleRect = boundingRect().intersected(mapFromScene(sceneVisibleRect).boundingRect());
	QRectF itemVisibleRect = rect().intersected(sceneVisibleRect);

	return itemVisibleRect;
}


} // namespace imtloggui


