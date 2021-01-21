#include <imtloggui/CLayerComp.h>


namespace imtloggui
{


// public methods

// reimplemented (icomp::CComponentBase)

QByteArray CLayerComp::GetActiveRepresentationId() const
{
	return m_activerRepresentationId;
}


bool CLayerComp::SetActiveRepresentationId(const QByteArray& representationId)
{
	if (GetElementIds().contains(representationId)){
		m_activerRepresentationId = representationId;
		imod::IModel* modelPtr =
					const_cast<imod::IModel*>(
								dynamic_cast<const imod::IModel*>(GetObjectPtr(m_activerRepresentationId)));

		m_representationProxy.SetModelPtr(modelPtr);

		return true;
	}

	return false;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CLayerComp::OnComponentCreated()
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_representationCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_representationCompPtr[i]);
	}

	if (count > 0){
		SetActiveRepresentationId(m_idAttrPtr[0]);
	}
}


} // namespace imtloggui


