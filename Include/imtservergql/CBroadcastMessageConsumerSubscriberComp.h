// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtservergql
{


class CBroadcastMessageConsumerSubscriberComp:
			public imtservergql::CGqlPublisherCompBase,
			virtual public ilog::IMessageConsumer
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;

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


