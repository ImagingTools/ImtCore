#include <imtloggui/CEventGroupControllerComp.h>


namespace imtloggui
{


CEventGroupControllerComp::CEventGroupControllerComp()
	: m_scenePtr(nullptr),
	m_timeAxisPtr(nullptr),
	m_graphicsItem(nullptr)
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


bool CEventGroupControllerComp::CreateGraphicsItem()
{
	if (m_graphicsItem == nullptr && m_scenePtr != nullptr && m_timeAxisPtr != nullptr){
		m_graphicsItem = new QGraphicsItemGroup();

		m_scenePtr->addItem(m_graphicsItem);
	}

	return false;
}


bool CEventGroupControllerComp::DestroyGraphicsItem()
{
	if (m_graphicsItem != nullptr && m_scenePtr != nullptr){
	

		for (IEventItemController* itemPtr : m_groups){
			RemoveGroup(itemPtr->GetGroupId());
		}
		m_groups.clear();

		m_scenePtr->removeItem(m_graphicsItem);
		delete m_graphicsItem;
		m_graphicsItem = nullptr;

		return true;
	}

	return false;
}


QGraphicsItem* CEventGroupControllerComp::GetGraphicsItem()
{
	return m_graphicsItem;
}


QByteArrayList CEventGroupControllerComp::GetAvailableGroupList() const
{
	QByteArrayList result;

	for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
		IEventItemController* eventItemControllerPtr = m_groupRefsCompPtr[i];
		if (eventItemControllerPtr != nullptr){
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
			eventItemController->CreateGraphicsItem();

			int totalHeight = 0;
			for (IEventItemController* item : m_groups){
				totalHeight += item->GetGroupHeight();
			}

			eventItemController->GetGraphicsItem()->setParentItem(m_graphicsItem);
			eventItemController->GetGraphicsItem()->setPos(0, -totalHeight);

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


void CEventGroupControllerComp::ViewPortChanged()
{
	if (m_groupRefsCompPtr.IsValid()){
		for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
			m_groupRefsCompPtr[i]->ViewPortChanged();
		}
	}
}


void CEventGroupControllerComp::TimeAxisChanged()
{
	if (m_groupRefsCompPtr.IsValid()){
		for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
			m_groupRefsCompPtr[i]->TimeAxisChanged();
		}
	}
}


} // namespace imtloggui


