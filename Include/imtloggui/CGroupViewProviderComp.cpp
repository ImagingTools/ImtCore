#include <imtloggui/CGroupViewProviderComp.h>


namespace imtloggui
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CGroupViewProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_groupViewCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		Q_ASSERT(!GetElementIds().contains(m_idAttrPtr[i]));
		RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_groupViewCompPtr[i]);
	}
}


} // namespace imtloggui


