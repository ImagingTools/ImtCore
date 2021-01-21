#include <imtloggui/CGroupComp.h>


// ImtCore includes
#include <imtbase/IObjectProvider.h>


namespace imtloggui
{


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CGroupComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_arrangedIds.isEmpty()){
		m_representationProxy.SetModelPtr(nullptr);
		return;
	}

	imtlog::CTimeRange timeRange = GetObservedObject()->GetTimeRange();
	if (!timeRange.IsClosed()){
		m_representationProxy.SetModelPtr(nullptr);
		return;
	}

	uint64_t timespan = timeRange.GetEndTime().toMSecsSinceEpoch() - timeRange.GetBeginTime().toMSecsSinceEpoch();
	QByteArray layerId;

	if (timespan < m_arrangedIds.firstKey()){
		layerId = m_arrangedIds.first();
	}
	else{
		QList<uint64_t> keys = m_arrangedIds.keys();
		uint64_t foundKey = m_arrangedIds.firstKey();
		for (int i = 0; i < keys.count(); i++){
			if (keys[i] < timespan){
				foundKey = keys[i];
			}
		}

		layerId = m_arrangedIds[foundKey];
	}
	
	if (layerId.isEmpty()){
		m_representationProxy.SetModelPtr(nullptr);
	}

	if (GetElementIds().count() > 0){
		Q_ASSERT(!layerId.isEmpty());

		const imtbase::IObjectProvider* activeRepresentationProviderPtr = dynamic_cast<const imtbase::IObjectProvider*>(BaseClass2::GetObjectPtr(layerId));
		if (activeRepresentationProviderPtr != nullptr){
			imod::IModel* modelPtr =const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(activeRepresentationProviderPtr->GetDataObject()));

			m_representationProxy.SetModelPtr(modelPtr);
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CGroupComp::OnComponentCreated()
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_minTimespanAttrPtr.GetCount());
	count = qMin(count, m_layerCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		Q_ASSERT(!m_arrangedIds.contains(m_minTimespanAttrPtr[i]));

		RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_layerCompPtr[i]);
		m_arrangedIds[m_minTimespanAttrPtr[i]] = m_idAttrPtr[i];
	}

	if (!m_arrangedIds.isEmpty()){
		QByteArray id = m_arrangedIds.first();
		m_arrangedIds.remove(m_arrangedIds.firstKey());
		m_arrangedIds[0] = id;
	}
}


} // namespace imtloggui


