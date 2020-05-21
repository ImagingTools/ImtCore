#include <imtloggui/CEventGroupItem.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


// public methods

CEventGroupItem::CEventGroupItem(QGraphicsItem* parent)
	: BaseClass(parent)
{
	m_bgColor = QColor("#00000000");
	m_name = "Noname";
}


void CEventGroupItem::SetBackgroundColor(const QColor& color)
{
	m_bgColor = color;
}


void CEventGroupItem::SetGroupName(QString name)
{
	m_name = name;
}


void CEventGroupItem::ViewPortChanged()
{
	update(GetItemVisibleRect());
}


// reimplemented (QGraphicsItem)

QRectF CEventGroupItem::boundingRect() const
{
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
	QGraphicsView* viewPtr = scene()->views().first();

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


