#include <imtloggui/CEventGroupItem.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>

// ImtCore includes
//#include <imtlog/IMessageGroupInfoProvider.h>


namespace imtloggui
{


// public methods

CEventGroupItem::CEventGroupItem(QGraphicsItem* parent)
	: BaseClass(parent)
{
	m_bgColor = QColor("#00000000");
	m_rect = QRectF(0, 0, 0, 0);
	setGraphicsItem(this);
}


void CEventGroupItem::SetBackgroundColor(const QColor& color)
{
	m_bgColor = color;
}


void CEventGroupItem::SetRect(QRectF rect)
{
	prepareGeometryChange();
	m_rect = rect;
}


void CEventGroupItem::ViewPortChanged()
{
	//prepareGeometryChange();

	//QGraphicsView* viewPtr = scene()->views().first();
	//QRect viewportRect = viewPtr->viewport()->rect();
	//QRectF sceneVisibleRect = viewPtr->mapToScene(viewportRect).boundingRect();
	//
	//m_boundingRect = BaseClass::boundingRect();
}


// reimplemented (QGraphicsItem)

QRectF CEventGroupItem::boundingRect() const
{
	return m_rect;
}


void CEventGroupItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	QBrush brush(m_bgColor);

	painter->save();
	painter->setBrush(brush);
	painter->setPen(Qt::NoPen);
	QRectF rect = geometry();
	painter->drawRect(rect);
	painter->restore();

	BaseClass::paint(painter, option, widget);
}


// reimplemented (QGraphicsLayoutItem)

QSizeF CEventGroupItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
	switch (which){
	case Qt::SizeHint::MinimumSize:
	case Qt::SizeHint::MinimumDescent:
	case Qt::SizeHint::PreferredSize:
		return m_rect.size();
	default:
		return QSizeF(-1, -1);
	}
}


} // namespace imtloggui


