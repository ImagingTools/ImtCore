#pragma once


// Acf includes
#include <ilog/IMessageConsumer.h>
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


/**
	Interface for event controller
*/
class IEventController: virtual public istd::IPolymorphic
{
public:
	typedef QMultiMap<QDateTime, CEventItemBase*> EventMap;
	typedef QVector<int> MessageIds;

	/**
		Create and add group graphics item to scene
	*/
	virtual QGraphicsItem* CreateGraphicsItem() = 0;

	/**
		Remove and destroy group graphics item from scene
	*/
	virtual void DestroyGraphicsItem() = 0;

	/**
		Get group graphics item
	*/
	virtual QGraphicsItem* GetGraphicsItem() const = 0;

	/**
		Get group id
	*/
	virtual QByteArray GetGroupId() const = 0;

	/**
		Get group name
	*/
	virtual QString GetGroupName() const = 0;

	/**
		Get group minimum height in pixels on screen
	*/
	virtual int GetGroupMinimalHeight() const = 0;

	/**
		Get message ids supported by group
	*/
	virtual MessageIds GetSupportedMessageIds() const = 0;

	/**
		Get events
	*/
	virtual const EventMap* GetEvents() const = 0;

	/**
		Get event count with gived category
	*/
	virtual int GetEventCount(istd::IInformationProvider::InformationCategory category) = 0;

	/**
		Add event for gived message
	*/
	virtual CEventItemBase* AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr) = 0;

	/**
		Remova all events
	*/
	virtual void ClearEvents() = 0;
};


} // namespace imtloggui


