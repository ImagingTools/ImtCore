#include <imtloggui/CRepresentationProductionSpeedViewFactoryComp.h>

// Qt includes
#include <QtCore/QRect>

// ACF includes
#include <ilog/CMessageContainer.h>
#include <imeas/CGeneralDataSequence.h>

// ImtCore includes
#include <imtloggui/CProductionSpeedItem.h>
#include <imtloggui/CRepresentationProductionSpeedFactoryComp.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationViewFactory)

IRepresentationViewFactory::GraphicsItemList CRepresentationProductionSpeedViewFactoryComp::CreateGraphicItems(const istd::TSmartPtr<istd::IChangeable> objectPtr, const QByteArray& groupId) const
{
	CRepresentationProductionSpeedModel* modelPtr = const_cast<CRepresentationProductionSpeedModel*>(
					dynamic_cast<const CRepresentationProductionSpeedModel*>(objectPtr.GetPtr()));

	if (modelPtr == nullptr){
		if (m_slaveFactoryCompPtr.IsValid()){
			return m_slaveFactoryCompPtr->CreateGraphicItems(objectPtr, groupId);
		}

		return {};
	}

	if (m_items.contains(groupId)){
		CProductionSpeedItem* itemPtr = dynamic_cast<CProductionSpeedItem*>(m_items[groupId].GetPtr());
		if (itemPtr->GetModel().GetPtr() == modelPtr){
			if (modelPtr->GetStatisticsItemCount() > 0){
				itemPtr->setPos(m_positionProviderCompPtr->GetScenePositionFromTime(QDateTime::fromMSecsSinceEpoch(modelPtr->GetStatisticsItem(0).time)), itemPtr->y());
				uint64_t width = modelPtr->GetStatisticsItemCount() * modelPtr->GetGranularity();
				itemPtr->setRect(QRectF(0, -120, width, 240));

				return {m_items[groupId]};
			}
		}

		m_items.remove(groupId);
	}

	if (!m_positionProviderCompPtr.IsValid()){
		return {};
	}

	if (modelPtr->GetStatisticsItemCount() == 0){
		return {};
	}

	CProductionSpeedItem* itemPtr = new CProductionSpeedItem();
	itemPtr->setPos(m_positionProviderCompPtr->GetScenePositionFromTime(QDateTime::fromMSecsSinceEpoch(modelPtr->GetStatisticsItem(0).time)), 0);
	uint64_t width = modelPtr->GetStatisticsItemCount() * modelPtr->GetGranularity();
	itemPtr->setRect(QRectF(0, -120, width, 240));
	itemPtr->SetModel(objectPtr);
	itemPtr->SetScenePositionProvider(m_positionProviderCompPtr.GetPtr());
	itemPtr->setAcceptHoverEvents(true);

	GraphicsItem item(itemPtr);
	m_items[groupId] = item;

	return {item};
}


} // namespace imtloggui


