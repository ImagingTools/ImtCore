// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtloggui/IEventItem.h>


namespace imtloggui
{


/**
	Interface for the event item factory.
*/
class IEventItemFactory
{
public:
	/**
		Get supported message ids for factory and all slaves
	*/
	virtual QVector<int> GetSupportedMessageIds() const = 0;

	/**
		Create event item instance with factory or slaves
		Slaves are used if the factory cannot create an event item instance for the given message
	*/
	virtual IEventItem* CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const = 0;
};


} // namespace imtloggui


