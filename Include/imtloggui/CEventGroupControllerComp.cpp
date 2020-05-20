#include <imtloggui/CEventGroupControllerComp.h>


// Qt includes
#include <QtWidgets/QGraphicsLayoutItem>


namespace imtloggui
{


CEventGroupControllerComp::CEventGroupControllerComp()
	: m_scenePtr(nullptr),
	m_timeAxisPtr(nullptr),
	m_layoutPtr(nullptr)
{
}


void CEventGroupControllerComp::SetScene(QGraphicsScene* scenePtr)
{
	m_scenePtr = scenePtr;

	if (m_scenePtr != nullptr){
		m_layoutPtr = new QGraphicsLinearLayout(Qt::Vertical);
		m_containerPtr = new QGraphicsWidget();
		m_containerPtr->setLayout(m_layoutPtr);
		m_scenePtr->addItem(m_containerPtr);
		m_containerPtr->setPos(0, 0);
		m_containerPtr->setGeometry(QRectF(0, 0, 100000, - 1000));
	}
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

			QGraphicsLayoutItem* layoutItemPtr = eventItemController->GetGroupItem();
			if (layoutItemPtr != nullptr){
				m_layoutPtr->addItem(layoutItemPtr);
			}

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


