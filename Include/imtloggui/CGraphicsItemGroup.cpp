#include <imtloggui/CGraphicsItemGroup.h>


namespace imtloggui
{


CGraphicsItemGroup::CGraphicsItemGroup(QGraphicsItem* parent)
	:BaseClass(parent)
{
}


// reimplemented (QGraphicsItem)
QRectF CGraphicsItemGroup::boundingRect() const
{
	return QRectF();
}


void CGraphicsItemGroup::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
}


} // namespace imtloggui


