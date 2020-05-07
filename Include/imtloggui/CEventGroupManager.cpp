#include <imtloggui/CEventGroupManager.h>


// Qt includes
#include <QDebug>
#include <QtCore/QRectF>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


CEventGroupManager::CEventGroupManager(QGraphicsItem *parent)
	: BaseClass(parent)
{
}


CEventGroupManager::~CEventGroupManager()
{
	for (CEventGroup* groupPtr : m_groups) {
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


void CEventGroupManager::RemoveGroup(int index)
{
	if (index < m_groups.count()) {
		m_groups.removeAt(index);
	}
}


CEventGroup* CEventGroupManager::GetGroup(QByteArray groupId)
{
	for (CEventGroup* groupPtr : m_groups) {
		if (groupPtr->GetGroupId() == groupId) {
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


void CEventGroupManager::SetVisible(bool isVisible)
{

}


} // namespace imtloggui


