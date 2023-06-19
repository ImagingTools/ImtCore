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
			for (ilog::IMessageContainer::Messages::iterator it = messages.begin(); it != messages.end(); it++) {
				IEventItem* itemPtr = nullptr;
				QDateTime messageTimeStamp = (*it)->GetInformationTimeStamp();

				itemPtr = m_eventItemFactoryCompPtr->CreateInstance(*it);
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


