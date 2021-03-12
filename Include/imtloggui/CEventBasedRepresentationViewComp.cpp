#include <imtloggui/CEventBasedRepresentationViewComp.h>


// ACF inlcudes
#include <ilog/IMessageContainer.h>


namespace imtloggui
{


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CEventBasedRepresentationViewComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	m_generatedItems.clear();

	const ilog::IMessageContainer* representationObjectPtr = dynamic_cast<const ilog::IMessageContainer*>(GetObservedObject());
	if (representationObjectPtr != nullptr){
		ilog::IMessageContainer::Messages messages = representationObjectPtr->GetMessages();

		if (m_eventItemFactoryCompPtr.IsValid() && m_positionProviderCompPtr.IsValid()){
			for (int i = 0; i < messages.count(); i++){
				IEventItem* itemPtr = nullptr;
				QDateTime messageTimeStamp = messages[i]->GetInformationTimeStamp();

				itemPtr = m_eventItemFactoryCompPtr->CreateInstance(messages[i]);
				if (itemPtr != nullptr){
					QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(itemPtr);
					graphicsItemPtr->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIgnoresTransformations);
					graphicsItemPtr->setPos(m_positionProviderCompPtr->GetScenePositionFromTime(messageTimeStamp), 0);

					m_generatedItems.push_back(GraphicsItemPtr(graphicsItemPtr));
				}
			}
		}

		BaseClass::OnUpdate(changeSet);
	}
}


} // namespace imtloggui


