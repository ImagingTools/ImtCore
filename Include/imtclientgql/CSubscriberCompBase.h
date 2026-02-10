// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IGqlSubscriptionManager.h>


namespace imtclientgql
{


class CSubscriberCompBase:
			public ilog::CLoggerComponentBase,
			virtual public IGqlSubscriptionClient
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSubscriberCompBase);
		I_REGISTER_INTERFACE(imtclientgql::IGqlSubscriptionClient);
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager", "Subscription manager", true, "SubscriptionManager");
		I_ASSIGN_MULTI_0(m_commandAttrPtr, "Command", "Command for subscription", true);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(
				const QByteArray& subscriptionId,
				const QByteArray& subscriptionData) override;
	virtual void OnSubscriptionStatusChanged(
				const QByteArray& subscriptionId,
				const SubscriptionStatus& status,
				const QString& message) override;
	
	QByteArray GetCommandId(const QByteArray& subscriptionId) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	QByteArrayList m_subscriptionIds;

protected:
	I_REF(imtclientgql::IGqlSubscriptionManager, m_subscriptionManagerCompPtr);
	I_MULTIATTR(QByteArray, m_commandAttrPtr);
};


} // namespace imtclientgql


