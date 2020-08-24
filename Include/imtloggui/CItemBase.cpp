#include <imtloggui/CItemBase.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IItemBase)

bool CItemBase::CollidesWithItem(const IItemBase* otherPtr, double scale) const
{
	const QGraphicsItem* graphicsItemPtr = dynamic_cast<const QGraphicsItem*>(otherPtr);

	if (graphicsItemPtr == nullptr){
		return false;
	}

	int w1 = this->boundingRect().width();
	int w2 = graphicsItemPtr->boundingRect().width();

	int dist = abs(pos().x() - graphicsItemPtr->pos().x()) * scale;

	return dist < (w1 + w2)*0.5;
}


} // namespace imtloggui


