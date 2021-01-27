#include <imtloggui/CProductionRepresentationViewComp.h>

// Qt includes
#include <QtCore/QRect>

// Acf includes
#include <ilog/CMessageContainer.h>
#include <imeas/CGeneralDataSequence.h>

// ImtCore includes
#include <imtloggui/CProductionSpeedItem.h>
#include <imtloggui/CProductionQualityItem.h>
#include <imtloggui/CProductionSpeedItem.h>
#include <imtloggui/CProductionRepresentationControllerComp.h>
#include <imtloggui/CProductionRepresentationComp.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationView)

IGraphicsItemProvider::GraphicsItemList CProductionRepresentationViewComp::GetItems() const
{
	if (m_item.IsValid()){
		return {m_item};
	}

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
	CProductionRepresentationComp* representationPtr = dynamic_cast<CProductionRepresentationComp*>(GetObservedObject());

	if (representationPtr == nullptr){
		return;
	}

	if (representationPtr->GetTimeline().size() == 0){
		return;
	}

	Q_ASSERT(m_positionProviderCompPtr.IsValid());
	double begin = m_positionProviderCompPtr->GetScenePositionFromTime(m_positionProviderCompPtr->GetBeginTime());
	double end = m_positionProviderCompPtr->GetScenePositionFromTime(m_positionProviderCompPtr->GetEndTime());
	uint64_t width = end - begin;

	if (m_item.IsValid()){
		CProductionQualityItem* itemPtr = dynamic_cast<CProductionQualityItem*>(m_item.GetPtr());
		//CProductionSpeedItem* itemPtr = dynamic_cast<CProductionSpeedItem*>(m_item.GetPtr());
		if (itemPtr->GetModel() == representationPtr){
			if (representationPtr->GetTimeline().count() > 0){
				itemPtr->setPos(begin, itemPtr->y());
				itemPtr->setRect(QRectF(0, -120, width, 240));
				itemPtr->update();
				return;
			}
		}
	}

	//imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(representationPtr);

	CProductionQualityItem* itemPtr = new CProductionQualityItem();
	//CProductionSpeedItem* itemPtr = new CProductionSpeedItem();
	itemPtr->setPos(begin, 0);
	itemPtr->setZValue(5);
	itemPtr->setRect(QRectF(0, -120, width, 240));
	itemPtr->SetModel(representationPtr);
	itemPtr->SetScenePositionProvider(m_positionProviderCompPtr.GetPtr());
	itemPtr->setAcceptHoverEvents(true);

	m_item.SetPtr(itemPtr);
}


} // namespace imtloggui


