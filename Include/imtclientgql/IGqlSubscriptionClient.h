#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IChangeable.h>


namespace imtclientgql
{


class IGqlSubscriptionClient: virtual public istd::IChangeable
{
public:
	enum SubscriptionStatus{
		SS_UNKNOWN = 0,
		SS_UNREGISTERED,
		SS_IN_REGISTRATION,
		SS_REGISTERED
	};

	virtual void OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData) = 0;
	virtual void OnSubscriptionStatusChanged(const QByteArray& subscriptionId, const SubscriptionStatus& status, const QString& message) = 0;
};


} // namespace imtgql


