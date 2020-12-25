#pragma once


// ACF includes
#include <istd/IInformationProvider.h>
#include <istd/TSmartPtr.h>


namespace imtlog
{


class IMessageHistoryConsumer: virtual public istd::IPolymorphic
{
public:
	struct Message
	{
		uint64_t id;
		istd::TSmartPtr<const istd::IInformationProvider> messagePtr;
	};

	typedef istd::TSmartPtr<Message> MessagePtr;
	typedef QList<IMessageHistoryConsumer::MessagePtr> Messages;

	virtual bool IsMessageSupported(const istd::IInformationProvider* messagePtr = nullptr) const = 0;
	virtual void AddMessage(const MessagePtr& messagePtr) = 0;
};


} // namespace imtlog

