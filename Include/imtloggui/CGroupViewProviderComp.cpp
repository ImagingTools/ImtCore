#include <imtloggui/CGroupViewProviderComp.h>


namespace imtloggui
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CGroupViewProviderComp::OnComponentCreated()
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_groupViewCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		Q_ASSERT(!GetElementIds().contains(m_idAttrPtr[i]));
		RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_groupViewCompPtr[i]);
	}

	ConnectObserversToModels();
}


// private methods

void CGroupViewProviderComp::ConnectObserversToModels()
{
	if (!m_groupProviderCompPtr.IsValid()){
		return;
	}

	// Iteration over groups
	QVector<QByteArray> groupIds = m_groupProviderCompPtr->GetElementIds();
	QVector<QByteArray> groupViewIds = GetElementIds();
	for (int groupIndex = 0; groupIndex < groupIds.count(); groupIndex++){
		QByteArray groupId = groupIds[groupIndex];

		if (!groupViewIds.contains(groupId)){
			continue;
		}

		imod::IModel* groupModelPtr = dynamic_cast<imod::IModel*>(
					const_cast<istd::IChangeable*>(m_groupProviderCompPtr->GetObjectPtr(groupId)));
		imod::IObserver* groupObserverPtr = dynamic_cast<imod::IObserver*>(
					const_cast<istd::IChangeable*>(GetObjectPtr(groupId)));
			
		if (groupModelPtr == nullptr || groupObserverPtr == nullptr){
			continue;
		}

		groupModelPtr->AttachObserver(groupObserverPtr);

		// Iteration over layers
		const imtbase::IObjectCollection* layerCollectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(
					m_groupProviderCompPtr->GetObjectPtr(groupId));
		const imtbase::IObjectCollection* layerViewCollectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(
					GetObjectPtr(groupId));

		QVector<QByteArray> layerIds = layerCollectionPtr->GetElementIds();
		QVector<QByteArray> layerViewIds = layerViewCollectionPtr->GetElementIds();
		for (int layerIndex = 0; layerIndex < layerIds.count(); layerIndex++){
			QByteArray layerId = layerIds[layerIndex];

			if (!layerViewIds.contains(layerId)){
				continue;
			}

			imod::IModel* layerModelPtr = dynamic_cast<imod::IModel*>(
						const_cast<istd::IChangeable*>(layerCollectionPtr->GetObjectPtr(layerId)));
			imod::IObserver* layerObserverPtr = dynamic_cast<imod::IObserver*>(
						const_cast<istd::IChangeable*>(layerViewCollectionPtr->GetObjectPtr(layerId)));

			if (layerModelPtr == nullptr || layerObserverPtr == nullptr){
				continue;
			}

			layerModelPtr->AttachObserver(layerObserverPtr);

			// Iteration over representations
			const imtbase::IObjectCollection* representationCollectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(
						layerCollectionPtr->GetObjectPtr(layerId));
			const imtbase::IObjectCollection* representationViewCollectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(
						layerViewCollectionPtr->GetObjectPtr(layerId));

			QVector<QByteArray> representationIds = representationCollectionPtr->GetElementIds();
			QVector<QByteArray> representationViewIds = representationViewCollectionPtr->GetElementIds();
			for (int representationIndex = 0; representationIndex < representationIds.count(); representationIndex++){
				QByteArray representationId = representationIds[representationIndex];

				if (!representationViewIds.contains(representationId)){
					continue;
				}

				imod::IModel* representationModelPtr = dynamic_cast<imod::IModel*>(
							const_cast<istd::IChangeable*>(representationCollectionPtr->GetObjectPtr(representationId)));
				imod::IObserver* representationObserverPtr = dynamic_cast<imod::IObserver*>(
							const_cast<istd::IChangeable*>(representationViewCollectionPtr->GetObjectPtr(representationId)));

				if (representationModelPtr == nullptr || representationObserverPtr == nullptr){
					continue;
				}

				representationModelPtr->AttachObserver(representationObserverPtr);
			}
		}
	}	
}


} // namespace imtloggui


