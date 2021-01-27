#include <imtloggui/CEventBasedRepresentationViewComp.h>


// ACF inlcudes
#include <ilog/IMessageContainer.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IGraphicsItemProvider)

IGraphicsItemProvider::GraphicsItemList CEventBasedRepresentationViewComp::GetItems() const
{
	return m_itemList;
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
	m_itemList.clear();

	const ilog::IMessageContainer* representationObjectPtr = dynamic_cast<const ilog::IMessageContainer*>(GetObservedObject());
	if (representationObjectPtr != nullptr){
		ilog::IMessageContainer::Messages messages = representationObjectPtr->GetMessages();

		if (m_eventItemFactoryCompPtr.IsValid() && m_positionProviderCompPtr.IsValid()){
			for (int i = 0; i < messages.count(); i++){
				IEventItem* itemPtr = nullptr;
				int messageId = messages[i]->GetInformationId();
				QDateTime messageTimeStamp = messages[i]->GetInformationTimeStamp();

				itemPtr = m_eventItemFactoryCompPtr->CreateInstance(messages[i]);
				if (itemPtr != nullptr){
					QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(itemPtr);
					graphicsItemPtr->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIgnoresTransformations);
					graphicsItemPtr->setPos(m_positionProviderCompPtr->GetScenePositionFromTime(messageTimeStamp), 0);

					GraphicsItem item(graphicsItemPtr);
					m_itemList.append(item);
				}
			}
		}

		Q_EMIT EmitRepresentationUpdated();
	}
}


} // namespace imtloggui


