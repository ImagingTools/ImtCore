#include <imtloggui/CEventBasedRepresentationViewComp.h>


// ACF inlcudes
#include <ilog/IMessageContainer.h>


namespace imtloggui
{


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CEventBasedRepresentationViewComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	GraphicsItemList items;

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

					items.push_back(GraphicsItemPtr(graphicsItemPtr));
				}
			}
		}

		QMutexLocker locker(&m_generatedItemsMutex);
		m_generatedItems = items;
		locker.unlock();

		Q_EMIT EmitRepresentationUpdated();
	}
}


} // namespace imtloggui


