#pragma once


// Acf includes
#include <ilog/IMessageConsumer.h>

// Qt includes
#include <QtWidgets/QGraphicsItem>

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
	virtual CEventItemBase* CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const = 0;
};


} // namespace imtloggui


