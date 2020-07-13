#pragma once


// Acf includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtloggui/IGraphicsItemController.h>
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


/**
	Interface for event controller (group of events)
*/
class IEventController: virtual public IGraphicsItemController
{
public:
	typedef QMultiMap<QDateTime, CEventItemBase*> EventMap;
	typedef QVector<int> MessageIds;

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
	virtual int GetGroupMinimumHeight() const = 0;

	/**
		Get message ids supported by group
	*/
	virtual MessageIds GetSupportedMessageIds() const = 0;

	/**
		Get events added to group
	*/
	virtual const EventMap* GetEvents() const = 0;

	/**
		Get event count with gived information category
	*/
	virtual int GetEventCount(istd::IInformationProvider::InformationCategory category) = 0;

	/**
		Add event to group for gived message
	*/
	virtual CEventItemBase* AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr) = 0;

	/**
		Remova all events from group
	*/
	virtual void ClearEvents() = 0;
};


} // namespace imtloggui


