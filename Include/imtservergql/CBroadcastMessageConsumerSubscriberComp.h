#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtservergql/CGqlSubscriberControllerCompBase.h>


namespace imtservergql
{


class CBroadcastMessageConsumerSubscriberComp:
			public imtservergql::CGqlSubscriberControllerCompBase,
			virtual public ilog::IMessageConsumer
{
public:
	typedef imtservergql::CGqlSubscriberControllerCompBase BaseClass;

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


} // namespace imtservergql


