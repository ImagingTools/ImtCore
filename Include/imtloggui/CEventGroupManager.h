#pragma once


// ImtCore includes
#include <imtloggui/CEventGroup.h>
#include <imtloggui/CTimeAxis.h>


namespace imtloggui
{


class CEventGroupManager: public QGraphicsItemGroup
{
public:
	typedef QGraphicsItemGroup BaseClass;

	CEventGroupManager(QGraphicsItem* parent = nullptr);
	~CEventGroupManager();

	int AddGroup(CEventGroup* groupPtr);
	bool RemoveGroup(QByteArray groupId);

	CEventGroup* GetGroup(QByteArray groupId);
	const QList<CEventGroup*>& GetGroups();

	bool MoveGroup(int fromIndex, int toIndex);
	bool SetVisible(QByteArray groupId, bool isVisible);

private:
	QList<CEventGroup*> m_groups;
};


} // namespace imtloggui


