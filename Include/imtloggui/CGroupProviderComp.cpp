#include <imtloggui/CGroupProviderComp.h>


namespace imtloggui
{


// reimplemented (imtloggui::IGroupProvider)
IGroup* CGroupProviderComp::GetGroup(const QByteArray& id) const
{
	if (m_groups.contains(id)){
		return m_groups[id];
	}

	return nullptr;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CGroupProviderComp::GetElementIds() const
{
	return m_groups.keys().toVector();
}


QVariant CGroupProviderComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	QVariant retVal;

	if (m_groups.contains(elementId)){
		switch (infoType){
		case EIT_NAME:
			retVal = m_groups[elementId]-> GetName();
			break;
		}
	}

	return retVal;
}


// reimplemented (icomp::CComponentBase)
void CGroupProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_generalGroupCompPtr.IsValid()){
		m_groups[m_generalGroupCompPtr->GetId()] = m_generalGroupCompPtr.GetPtr();
	}

	for (int i = 0; i < m_groupsCompPtr.GetCount(); i++){
		m_groups[m_groupsCompPtr[i]->GetId()] = m_groupsCompPtr[i];
	}
}


} // namespace imtloggui


