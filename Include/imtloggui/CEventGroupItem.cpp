#include <imtloggui/CEventGroupItem.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>

// ImtCore includes
//#include <imtlog/IMessageGroupInfoProvider.h>


namespace imtloggui
{


// public methods

CEventGroupItem::CEventGroupItem(QGraphicsItem* parent)
	: BaseClass(parent)
{
	m_bgColor = QColor("#00000000");
	m_name = "Noname";
	//m_labelPtr = new (CGroupLabelItem);
	//m_labelPtr->setFlag(ItemIgnoresTransformations);
	//m_labelPtr->setZValue(1000);
}


CEventGroupItem::~CEventGroupItem()
{
	//scene()->removeItem(m_labelPtr);
	//delete m_labelPtr;
}


void CEventGroupItem::SetBackgroundColor(const QColor& color)
{
	m_bgColor = color;
}


void CEventGroupItem::SetGroupName(QString name)
{
	m_name = name;
	//m_labelPtr->SetText(name);
}


void CEventGroupItem::ViewPortChanged()
{
	update(GetItemVisibleRect());

	//QRectF sceneVisibleRect = GetSceneVisibleRect();
	
	//QPointF origin;
	//origin.setX(sceneVisibleRect.left());
	//origin.setY(mapToScene(pos()).y() + rect().height() / 2);

	//m_labelPtr->setPos(origin);
}


// reimplemented (QGraphicsItem)

QRectF CEventGroupItem::boundingRect() const
{
	//if (m_labelPtr->scene() == nullptr && scene() != nullptr){
	//	scene()->addItem(m_labelPtr);
	//}

	QRectF visibleRect = GetSceneVisibleRect();
	QRectF itemRect = rect();
	QPointF origin = itemRect.bottomLeft();

	if (visibleRect.left() < itemRect.left()){
		itemRect.setLeft(visibleRect.left());
	}

	if (visibleRect.right() > itemRect.right()){
		itemRect.setRight(visibleRect.right());
	}

	double scale = GetCurrentScaleX();
	
	if (scale > 1){
		itemRect.setLeft(origin.x() + (itemRect.left() - origin.x()) * scale);
		itemRect.setRight(origin.x() + (itemRect.right() - origin.x()) * scale);
	}

	return itemRect;
}


void CEventGroupItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	QBrush brush(m_bgColor);

	double scaleX = GetCurrentScaleX();
	double scaleY = GetCurrentScaleY();

	painter->save();
	painter->setBrush(brush);
	painter->setPen(Qt::NoPen);
	painter->drawRect(boundingRect());
	painter->restore();

	QRectF sceneVisibleRect = GetSceneVisibleRect();
	
	QRectF labelRect = option->fontMetrics.boundingRect(m_name);
	labelRect.setWidth(labelRect.width() + 1);

	painter->setRenderHint(QPainter::SmoothPixmapTransform);

	painter->translate(sceneVisibleRect.left(), rect().height() / 2);
	painter->scale(1 / scaleX, 1 / scaleY);
	painter->rotate(-90);
	painter->drawText(QPointF(-labelRect.width() / 2, labelRect.height()), m_name);
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


QRectF CEventGroupItem::GetItemVisibleRect() const
{
	QRectF sceneVisibleRect = GetSceneVisibleRect();
	QRectF itemVisibleRect = boundingRect().intersected(mapFromScene(sceneVisibleRect).boundingRect());
	
	return itemVisibleRect;
}


} // namespace imtloggui


