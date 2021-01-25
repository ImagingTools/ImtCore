#include <imtloggui/CEventBasedRepresentationViewComp.h>


namespace imtloggui
{


// public methods

//imod::IObserver* CEventBasedRepresentationViewComp::GetRepresentationObserver()
//{
	//GraphicsItemList itemList;

	//const CRepresentationEventsObject* representationObjectPtr = dynamic_cast<const CRepresentationEventsObject*>(objectPtr.GetPtr());
	//if (representationObjectPtr != nullptr){
	//	if (m_eventItemFactoryCompPtr.IsValid() && m_positionProviderCompPtr.IsValid()){
	//		for (uint64_t id : m_items[groupId].keys()){
	//			uint64_t foundId = ULLONG_MAX;
	//			for (int i = 0; i < representationObjectPtr->count(); i++){
	//				if (representationObjectPtr->at(i)->id == id){
	//					foundId = id;
	//					break;
	//				}
	//			}

	//			if (foundId == ULLONG_MAX){
	//				m_items[groupId].remove(id);
	//			}
	//		}

	//		for (int i = 0; i < representationObjectPtr->count(); i++){
	//			IEventItem* itemPtr;

	//			if (m_items[groupId].contains(representationObjectPtr->at(i)->id)){
	//				GraphicsItem item = m_items[groupId][representationObjectPtr->at(i)->id];
	//				itemList.append(item);
	//				item->setPos(m_positionProviderCompPtr->GetScenePositionFromTime(representationObjectPtr->at(i)->messagePtr->GetInformationTimeStamp()), item->pos().y());
	//			}
	//			else{
	//				itemPtr = m_eventItemFactoryCompPtr->CreateInstance(representationObjectPtr->at(i)->messagePtr);
	//				if (itemPtr != nullptr){
	//					QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(itemPtr);
	//					graphicsItemPtr->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIgnoresTransformations);
	//					graphicsItemPtr->setPos(m_positionProviderCompPtr->GetScenePositionFromTime(representationObjectPtr->at(i)->messagePtr->GetInformationTimeStamp()), 0);
	//					GraphicsItem item(graphicsItemPtr);
	//					itemList.append(item);
	//					m_items[groupId][representationObjectPtr->at(i)->id] = item;
	//				}
	//			}
	//		}
	//	}
	//}
	//else{
	//	if (m_slaveFactoryCompPtr.IsValid()){
	//		return m_slaveFactoryCompPtr->CreateGraphicItems(objectPtr, groupId);
	//	}
	//}

//	return nullptr;
//}


// reimplemented (imtloggui::IGraphicsItemProvider)

IGraphicsItemProvider::GraphicsItemList CEventBasedRepresentationViewComp::GetItems() const
{
	return GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CEventBasedRepresentationViewComp::GetAddedItems() const
{
	return GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CEventBasedRepresentationViewComp::GetRemovedItems() const
{
	return GraphicsItemList();
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CEventBasedRepresentationViewComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{

}


} // namespace imtloggui


