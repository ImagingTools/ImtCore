#include <imtloggui/CLayerViewComp.h>


namespace imtloggui
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CLayerViewComp::OnComponentCreated()
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_representationViewCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_representationViewCompPtr[i]);
	}
}


} // namespace imtloggui


