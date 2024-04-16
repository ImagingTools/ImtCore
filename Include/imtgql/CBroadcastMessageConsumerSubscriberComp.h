#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtgql/CGqlSubscriberControllerCompBase.h>


namespace imtgql
{


class CBroadcastMessageConsumerSubscriberComp:
			public imtgql::CGqlSubscriberControllerCompBase,
			virtual public ilog::IMessageConsumer
{
public:
	typedef imtgql::CGqlSubscriberControllerCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<istd::IChangeable> BaseClass2;

	I_BEGIN_COMPONENT(CBroadcastMessageConsumerSubscriberComp);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer)
	I_END_COMPONENT;

protected:

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
		int messageCategory = -1,
		int messageId = -1,
		const istd::IInformationProvider* messagePtr = NULL) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;
};


} // namespace imtgql


