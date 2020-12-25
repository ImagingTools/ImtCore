#pragma once


// ImtCore includes
#include <imtlog/IMessageHistoryConsumer.h>
#include <imtlog/IMessageFilter.h>


namespace imtlog
{


class IMessageHistoryProvider: virtual public istd::IPolymorphic
{
public:
	virtual IMessageHistoryConsumer::Messages GetMessages(const IMessageFilter* filterPtr) const = 0;
};


} // namespace imtlog

