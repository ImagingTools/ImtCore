#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsItemGroup>
#include <QtWidgets/QStyleOptionGraphicsItem>

// ImtCore includes
#include <imtloggui/IEventScenePositionProvider.h>
#include <imtloggui/CEventItemBase.h>

namespace imtloggui
{


class CEventGroup: public QGraphicsItemGroup
{
public:
	typedef QGraphicsItemGroup BaseClass;

	CEventGroup(QGraphicsItem* parent = nullptr);
	~CEventGroup();

	const QByteArray& GetGroupId();
	void SetGroupId(const QByteArray& groupId);
	void SetTimeAxis(IEventScenePositionProvider* timeAxis);

	void AddEvent(CEventItemBase* event);

private:
	IEventScenePositionProvider* m_timeAxis;
	QByteArray m_groupId;
	QList<QGraphicsItem*> m_events;
};


} // namespace imtloggui


