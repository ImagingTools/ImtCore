#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsItemGroup>
#include <QtWidgets/QStyleOptionGraphicsItem>

// ImtCore includes
#include <imtbase/IMessageGroupInfoProvider.h>
#include <imtloggui/CTimeAxis.h>
#include <imtloggui/CEventGroup.h>

namespace imtloggui
{


class CEventGroupManager: public QGraphicsItemGroup
{
public:
	typedef QGraphicsItemGroup BaseClass;

	CEventGroupManager(QGraphicsItem *parent = nullptr);
	~CEventGroupManager();

	int AddGroup(CEventGroup* groupPtr);
	void RemoveGroup(int index);

	CEventGroup* GetGroup(QByteArray groupId);
	const QList<CEventGroup*>& GetGroups();

	bool MoveGroup(int fromIndex, int toIndex);
	void SetVisible(bool isVisible);

private:
	QList<CEventGroup*> m_groups;
};


} // namespace imtloggui


