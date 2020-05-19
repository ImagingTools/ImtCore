#include <imtloggui/CEventGroupControllerComp.h>


namespace imtloggui
{


CEventGroupControllerComp::CEventGroupControllerComp()
	: m_scenePtr(nullptr),
	m_timeAxisPtr(nullptr)
{
}


void CEventGroupControllerComp::SetScene(QGraphicsScene* scenePtr)
{
	m_scenePtr = scenePtr;
}


void CEventGroupControllerComp::SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr)
{
	m_timeAxisPtr = timeAxisPtr;
}


QByteArrayList CEventGroupControllerComp::GetAvailableGroupList() const
{
	QByteArrayList result;

	if (m_groupRefsCompPtr.IsValid()){
		for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
			result.append(m_groupRefsCompPtr[i]->GetGroupId());
		}
	}

	return result;
}


QByteArrayList CEventGroupControllerComp::GetActiveGroupList() const
{
	return m_groups.keys();
}


IEventItemController* CEventGroupControllerComp::GetGroup(const QByteArray& groupId) const
{
	if (m_groups.contains(groupId)){
		return m_groups[groupId];
	}

	return nullptr;
}


QString CEventGroupControllerComp::GetGroupName(const QByteArray& groupId) const
{
	if (m_groups.contains(groupId)){
		return m_groups[groupId]->GetGroupName();
	}

	return QString();
}


IEventItemController* CEventGroupControllerComp::CEventGroupControllerComp::AddGroup(const QByteArray& groupId, const QString& groupName)
{
	if (m_groups.contains(groupId)){
		return m_groups[groupId];
	}

	IEventItemController* eventItemController = nullptr;

	if (m_groupRefsCompPtr.IsValid()){
		for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
			if (m_groupRefsCompPtr[i]->GetGroupId() == groupId){
				eventItemController = m_groupRefsCompPtr[i];
				break;
			}
		}

		if (eventItemController != nullptr){
			eventItemController->SetGroupName(groupName);
			eventItemController->SetScene(m_scenePtr);
			eventItemController->SetTimeAxis(m_timeAxisPtr);
			eventItemController->CreateGroupItem();

			m_groups[groupId] = eventItemController;
		}
	}

	return eventItemController;
}


bool CEventGroupControllerComp::RemoveGroup(const QByteArray& groupId)
{
	if (m_groups.contains(groupId)){
		delete m_groups[groupId];
		m_groups.remove(groupId);
		return true;
	}

	return false;
}


bool CEventGroupControllerComp::SetVisible(QByteArray groupId, bool isVisible) const
{
	if (m_groups.contains(groupId)){
		m_groups[groupId]->SetVisible(isVisible);
		return true;
	}

	return false;
}


} // namespace imtloggui


