#pragma once


// Acf includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


/**
	Interface for the event item factory.
	Item factory creates event item for given message.
*/
class IEventItemFactory
{
public:
	virtual QVector<int> GetSupportedMessageIds() const = 0;
	virtual CEventItemBase* CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const = 0;
};


} // namespace imtloggui


