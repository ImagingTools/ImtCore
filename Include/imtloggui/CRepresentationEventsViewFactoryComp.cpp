#include <imtloggui/CRepresentationEventsViewFactoryComp.h>

#include <qdebug>

// ACF includes
#include <imtloggui/CRepresentationEventsObject.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationViewFactory)

IRepresentationViewFactory::GraphicsItemList CRepresentationEventsViewFactoryComp::CreateGraphicItems(const istd::TSmartPtr<istd::IChangeable> objectPtr, const QByteArray& groupId) const
{
	GraphicsItemList itemList;

	const CRepresentationEventsObject* representationObjectPtr = dynamic_cast<const CRepresentationEventsObject*>(objectPtr.GetPtr());
	if (representationObjectPtr != nullptr){
		if (m_eventItemFactoryCompPtr.IsValid() && m_positionProviderCompPtr.IsValid()){
			for (uint64_t id : m_items[groupId].keys()){
				uint64_t foundId = ULLONG_MAX;
				for (int i = 0; i < representationObjectPtr->count(); i++){
					if (representationObjectPtr->at(i).id == id){
						foundId = id;
						break;
					}
				}

				if (foundId == ULLONG_MAX){
					m_items[groupId].remove(id);
				}
			}


			for (int i = 0; i < representationObjectPtr->count(); i++){
				IEventItem* itemPtr;

				if (m_items[groupId].contains(representationObjectPtr->at(i).id)){
					itemPtr = m_items[groupId][representationObjectPtr->at(i).id];
					QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(itemPtr);
					if (graphicsItemPtr != nullptr){
						itemList.append(graphicsItemPtr);
					}
				}
				else{
					itemPtr = m_eventItemFactoryCompPtr->CreateInstance(representationObjectPtr->at(i).messagePtr);
					QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(itemPtr);
					if (graphicsItemPtr != nullptr){
						graphicsItemPtr->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIgnoresTransformations);
						graphicsItemPtr->setPos(m_positionProviderCompPtr->GetScenePositionFromTime(representationObjectPtr->at(i).messagePtr->GetInformationTimeStamp()), 0);
						itemList.append(graphicsItemPtr);
					}
					m_items[groupId][representationObjectPtr->at(i).id] = itemPtr;
				}
			}
		}
	}
	else{
		if (m_slaveFactoryCompPtr.IsValid()){
			return m_slaveFactoryCompPtr->CreateGraphicItems(objectPtr, groupId);
		}
	}

	return itemList;
}


} // namespace imtloggui


