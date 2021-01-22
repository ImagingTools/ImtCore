#include <imtloggui/CLayerComp.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::ILayerController)

QByteArray CLayerComp::GetActiveRepresentationId() const
{
	return m_activeRepresentationId;
}


bool CLayerComp::SetActiveRepresentationId(const QByteArray& representationId)
{
	if (GetElementIds().contains(representationId)){
		if (m_activeRepresentationId != representationId){
			istd::CChangeNotifier changeNotifier(this);
			m_activeRepresentationId = representationId;
			return true;
		}
	}

	return false;
}


// reimplemented (imtbase::IObjectProvider)

const istd::IChangeable* CLayerComp::GetDataObject() const
{
	return GetObjectPtr(m_activeRepresentationId);
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


