#pragma once


// ImtCore includes
#include <imtcom/IConnectionStatusProvider.h>
#include <imtclientgql/CSubscriberCompBase.h>


namespace imtclientgql
{


class CConnectionStatusSubscriberComp:
			public CSubscriberCompBase,
			virtual public imtcom::IConnectionStatusProvider
{
public:
	typedef CSubscriberCompBase BaseClass;

	I_BEGIN_COMPONENT(CConnectionStatusSubscriberComp);
		I_REGISTER_INTERFACE(imtcom::IConnectionStatusProvider);
	I_END_COMPONENT;

	CConnectionStatusSubscriberComp();

protected:
	// reimplemented (CSubscriberCompBase)
	virtual void OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData) override;

	// reimplemented (imtcom::IConnectionStatusProvider)
	virtual ConnectionStatus GetConnectionStatus() const override;

protected:
	imtcom::IConnectionStatusProvider::ConnectionStatus m_connectionStatus;
};


} // namespace imtclientgql


