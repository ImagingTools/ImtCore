#include <imtloggui/CRepresentationViewFactoryComp.h>

#include <qdebug>

// ACF includes
#include <ilog/CMessageContainer.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationViewFactory)

IRepresentationViewFactory::GraphicsItemList CRepresentationViewFactoryComp::CreateGraphicItems(const istd::IChangeable* objectPtr) const
{
	GraphicsItemList itemList;

	QTime time;
	time.start();

	if (m_eventItemFactoryCompPtr.IsValid() && m_positionProviderCompPtr.IsValid()){
		const ilog::CMessageContainer* messageContainerPtr = dynamic_cast<const ilog::CMessageContainer*>(objectPtr);
		if (messageContainerPtr != nullptr){
			ilog::CMessageContainer::Messages messages = messageContainerPtr->GetMessages();
			for (int i = 0; i < messages.count(); i++){
				IEventItem* itemPtr = m_eventItemFactoryCompPtr->CreateInstance(messages[i]);
				QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(itemPtr);
				if (graphicsItemPtr != nullptr){
					graphicsItemPtr->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIgnoresTransformations);
					graphicsItemPtr->setPos(m_positionProviderCompPtr->GetScenePositionFromTime(messages[i]->GetInformationTimeStamp()), 0);
					itemList.append(graphicsItemPtr);
				}
			}
		}
	}

	qDebug() << "Create items" << time.elapsed();


	return itemList;
}


} // namespace imtloggui


