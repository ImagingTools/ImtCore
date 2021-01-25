#include <imtloggui/CProductionRepresentationViewComp.h>

// Qt includes
#include <QtCore/QRect>

// Acf includes
#include <ilog/CMessageContainer.h>
#include <imeas/CGeneralDataSequence.h>

// ImtCore includes
#include <imtloggui/CProductionSpeedItem.h>
#include <imtloggui/CProductionQualityItem.h>
#include <imtloggui/CProductionRepresentationControllerComp.h>


namespace imtloggui
{


// public methods

//IRepresentationView::GraphicsItemList CProductionRepresentationViewComp::GetGraphicItems() const
//{
	//CRepresentationProductionModel* modelPtr = const_cast<CRepresentationProductionModel*>(
	//				dynamic_cast<const CRepresentationProductionModel*>(objectPtr.GetPtr()));

	//if (modelPtr == nullptr){
	//	if (m_slaveFactoryCompPtr.IsValid()){
	//		return m_slaveFactoryCompPtr->CreateGraphicItems(objectPtr, groupId);
	//	}

	//	return {};
	//}

	//if (modelPtr->GetTimeline().size() == 0){
	//	return {};
	//}

	//Q_ASSERT(m_positionProviderCompPtr.IsValid());
	//double begin = m_positionProviderCompPtr->GetScenePositionFromTime(m_positionProviderCompPtr->GetBeginTime());
	//double end = m_positionProviderCompPtr->GetScenePositionFromTime(m_positionProviderCompPtr->GetEndTime());
	//uint64_t width = end - begin;

	//if (m_items.contains(groupId)){
	//	//CProductionSpeedItem* itemPtr = dynamic_cast<CProductionSpeedItem*>(m_items[groupId].GetPtr());
	//	CProductionQualityItem* itemPtr = dynamic_cast<CProductionQualityItem*>(m_items[groupId].GetPtr());
	//	if (itemPtr->GetModel().GetPtr() == modelPtr){
	//		if (modelPtr->GetTimeline().count() > 0){
	//			itemPtr->setPos(begin, itemPtr->y());
	//			itemPtr->setRect(QRectF(0, -120, width, 240));

	//			return {m_items[groupId]};
	//		}
	//	}

	//	m_items.remove(groupId);
	//}

	//CProductionQualityItem* itemPtr = new CProductionQualityItem();
	//itemPtr->setPos(begin, 0);
	//itemPtr->setRect(QRectF(0, -120, width, 240));
	//itemPtr->SetModel(objectPtr);
	//itemPtr->SetScenePositionProvider(m_positionProviderCompPtr.GetPtr());
	//itemPtr->setAcceptHoverEvents(true);

	//GraphicsItem item(itemPtr);
	//m_items[groupId] = item;

//	return {}; //{item};
//}


// reimplemented (imtloggui::IRepresentationView)

IGraphicsItemProvider::GraphicsItemList CProductionRepresentationViewComp::GetItems() const
{
	return GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CProductionRepresentationViewComp::GetAddedItems() const
{
	return GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CProductionRepresentationViewComp::GetRemovedItems() const
{
	return GraphicsItemList();
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CProductionRepresentationViewComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{

}


} // namespace imtloggui


