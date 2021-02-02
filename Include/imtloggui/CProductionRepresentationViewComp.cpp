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


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CProductionRepresentationViewComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	CProductionRepresentationComp* representationPtr = dynamic_cast<CProductionRepresentationComp*>(GetObservedObject());
	if (representationPtr == nullptr){
		return;
	}

	if (representationPtr->GetTimeline().isEmpty()){
		return;
	}

	QMutexLocker locker(&m_generatedItemsMutex);
	if (!m_generatedItems.isEmpty()){
		CProductionQualityItem* itemPtr = dynamic_cast<CProductionQualityItem*>(m_generatedItems.first().GetPtr());
		//CProductionSpeedItem* itemPtr = dynamic_cast<CProductionSpeedItem*>(m_item.GetPtr());
		m_generatedItems.push_back(GraphicsItemPtr(itemPtr));
		locker.unlock();

		Q_EMIT EmitRepresentationUpdated();
		return;
	}
	locker.unlock();

	CProductionQualityItem* itemPtr = new CProductionQualityItem();
	//CProductionSpeedItem* itemPtr = new CProductionSpeedItem();
	itemPtr->setZValue(5);
	itemPtr->SetModel(representationPtr);
	itemPtr->SetScenePositionProvider(m_positionProviderCompPtr.GetPtr());
	itemPtr->setAcceptHoverEvents(true);

	locker.relock();
	m_generatedItems.push_back(GraphicsItemPtr(itemPtr));
	locker.unlock();

	Q_EMIT EmitRepresentationUpdated();
}


// reimplemented (imtloggui::CRepresentationViewCompBase)

void CProductionRepresentationViewComp::OnRepresentationUpdated()
{
	CProductionQualityItem* itemPtr = dynamic_cast<CProductionQualityItem*>(m_generatedItems.first().GetPtr());
	CProductionRepresentationComp* representationPtr = dynamic_cast<CProductionRepresentationComp*>(itemPtr->GetModel());

	Q_ASSERT(m_positionProviderCompPtr.IsValid());
	double begin = m_positionProviderCompPtr->GetScenePositionFromTime(m_positionProviderCompPtr->GetBeginTime());
	double end = m_positionProviderCompPtr->GetScenePositionFromTime(m_positionProviderCompPtr->GetEndTime());
	uint64_t width = end - begin;

	if (representationPtr->GetTimeline().count() > 0){
		itemPtr->setPos(begin, 0);
		itemPtr->setRect(QRectF(0, -120, width, 240));
		itemPtr->update();
	}

	BaseClass::OnRepresentationUpdated();
}


} // namespace imtloggui


