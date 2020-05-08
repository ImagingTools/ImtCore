#include <imtloggui/CEventGroup.h>


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


CEventGroup::CEventGroup(QGraphicsItem* parent)
	: BaseClass(parent)
{
}


const QByteArray& CEventGroup::GetGroupId()
{
	return m_groupId;
}


CEventGroup::~CEventGroup()
{
	for (QGraphicsItem* eventPtr : m_events){
		scene()->removeItem(eventPtr);
		delete eventPtr;
	}

	scene()->removeItem(this);
}


void CEventGroup::SetGroupId(const QByteArray& groupId)
{
	m_groupId = groupId;
}


void CEventGroup::SetTimeAxis(IEventScenePositionProvider* timeAxis)
{
	m_timeAxis = timeAxis;
}


void CEventGroup::AddEvent(CEventItemBase* event)
{
	QPointF origin(m_timeAxis->GetScenePosition(event->GetEventTimeStamp()), 0);
	origin = mapFromScene(origin);
	origin.setY(-100);

	event->setParentItem(this);
	event->setPos(origin);

	scene()->addItem(event);

	m_events.append(event);
}


} // namespace imtloggui


