#include <imtloggui/CEventGroupManager.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QRectF>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


CEventGroupManager::CEventGroupManager(QGraphicsItem* parent)
	: BaseClass(parent)
{
}


CEventGroupManager::~CEventGroupManager()
{
	for (CEventGroup* groupPtr : m_groups){
		delete groupPtr;
	}
}


int CEventGroupManager::AddGroup(CEventGroup* groupPtr)
{
	groupPtr->setParentItem(this);
	groupPtr->setPos(0, 0);
	
	scene()->addItem(groupPtr);
	
	m_groups.append(groupPtr);

	return m_groups.count() - 1;
}


bool CEventGroupManager::RemoveGroup(QByteArray groupId)
{
	CEventGroup* groupPtr = GetGroup(groupId);
	if (groupPtr != nullptr){
		m_groups.removeOne(groupPtr);
		return true;
	}

	return false;
}


CEventGroup* CEventGroupManager::GetGroup(QByteArray groupId)
{
	for (CEventGroup* groupPtr : m_groups){
		if (groupPtr->GetGroupId() == groupId){
			return groupPtr;
		}
	}

	return nullptr;
}


const QList<CEventGroup*>& CEventGroupManager::GetGroups()
{
	return m_groups;
}


bool CEventGroupManager::MoveGroup(int fromIndex, int toIndex)
{
	return true;
}


bool CEventGroupManager::SetVisible(QByteArray groupId, bool isVisible)
{
	CEventGroup* groupPtr = GetGroup(groupId);
	if (groupPtr != nullptr){
		groupPtr->setVisible(isVisible);
		return true;
	}

	return false;
}


} // namespace imtloggui


