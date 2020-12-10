#include <imtloggui/CItemBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtloggui
{


// public methods

void CItemBase::SetParams(QGraphicsItem *parent)
{
	setParentItem(parent);
}


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


bool CItemBase::SetMetaInfo(int /*metaInfoType*/, const QVariant& /*metaInfo*/)
{
	return false;
}


QString CItemBase::GetMetaInfoDescription(int /*metaInfoType*/) const
{
	return QString();
}


bool CItemBase::IsMetaInfoWritable(int /*metaInfoType*/) const
{
	return false;
}


} // namespace imtloggui


