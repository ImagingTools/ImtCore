#include <imtloggui/CEventBasedRepresentationViewComp.h>


// ACF inlcudes
#include <ilog/IMessageContainer.h>


namespace imtloggui
{


// public methods

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

void CEventBasedRepresentationViewComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	GraphicsItemList itemList;

	const ilog::IMessageContainer* representationObjectPtr = dynamic_cast<const ilog::IMessageContainer*>(GetObservedObject());
	if (representationObjectPtr != nullptr){
		ilog::IMessageContainer::Messages messages = representationObjectPtr->GetMessages();

		if (m_eventItemFactoryCompPtr.IsValid() && m_positionProviderCompPtr.IsValid()){
			for (uint64_t id : m_items.keys()){
				uint64_t foundId = ULLONG_MAX;
				for (int i = 0; i < messages.count(); i++){
					if (messages.at(i)->GetInformationId() == id){
						foundId = id;
						break;
					}
				}

				if (foundId == ULLONG_MAX){
					m_items.remove(id);
				}
			}

			for (int i = 0; i < messages.count(); i++){
				IEventItem* itemPtr = nullptr;
				int messageId = messages[i]->GetInformationId();
				QDateTime messageTimeStamp = messages[i]->GetInformationTimeStamp();

				if (m_items.contains(messageId)){
					GraphicsItem item = m_items[messageId];
					itemList.append(item);

					item->setPos(m_positionProviderCompPtr->GetScenePositionFromTime(messageTimeStamp), item->pos().y());
				}
				else{
					itemPtr = m_eventItemFactoryCompPtr->CreateInstance(messages[i]);
					if (itemPtr != nullptr){
						QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(itemPtr);
						graphicsItemPtr->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIgnoresTransformations);
						graphicsItemPtr->setPos(m_positionProviderCompPtr->GetScenePositionFromTime(messageTimeStamp), 0);

						GraphicsItem item(graphicsItemPtr);
						itemList.append(item);

						m_items[messageId] = item;
					}
				}
			}
		}
	}
}


} // namespace imtloggui


