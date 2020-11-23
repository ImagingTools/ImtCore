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
		if (m_items[groupId]->GetModel().GetPtr() == modelPtr){
			return {m_items[groupId]};
		}

		m_items.remove(groupId);
	}

	if (!m_positionProviderCompPtr.IsValid()){
		return {};
	}

	if (modelPtr->GetStatisticsItemCount() == 0){
		return {};
	}

	CProductionSpeedItem* item = new CProductionSpeedItem();
	item->setPos(m_positionProviderCompPtr->GetScenePositionFromTime(QDateTime::fromMSecsSinceEpoch(modelPtr->GetStatisticsItem(0).time)), 0);
	uint64_t width = (m_positionProviderCompPtr->GetEndTime().toMSecsSinceEpoch() - m_positionProviderCompPtr->GetBeginTime().toMSecsSinceEpoch()) / 1000;
	item->setRect(QRectF(0, -120, width, 240));
	item->SetModel(objectPtr);
	item->SetScenePositionProvider(m_positionProviderCompPtr.GetPtr());

	return {item};
}


} // namespace imtloggui


