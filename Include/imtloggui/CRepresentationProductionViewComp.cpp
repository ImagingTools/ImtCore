#include <imtloggui/CRepresentationProductionViewComp.h>

// Qt includes
#include <QtCore/QRect>

// ACF includes
#include <ilog/CMessageContainer.h>
#include <imeas/CGeneralDataSequence.h>

// ImtCore includes
#include <imtloggui/CProductionSpeedItem.h>
#include <imtloggui/CProductionQualityItem.h>
#include <imtloggui/CRepresentationProductionControllerComp.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationView)

imod::IObserver* CRepresentationProductionViewComp::GetRepresentationObserver()
{
	return nullptr;
}


//IRepresentationView::GraphicsItemList CRepresentationProductionViewComp::GetGraphicItems() const
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

IGraphicsItemProvider::GraphicsItemList CRepresentationProductionViewComp::GetItems() const
{
	return GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CRepresentationProductionViewComp::GetAddedItems() const
{
	return GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CRepresentationProductionViewComp::GetRemovedItems() const
{
	return GraphicsItemList();
}


} // namespace imtloggui


