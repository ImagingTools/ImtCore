// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/IModel.h>

// Qt includes
#include <QRecursiveMutex>
#include <QMap>

// ImtCore includes
#include <imtauth/ICrossTenantMessage.h>
#include <imtauth/ITenantManager.h>
#include <imtservergql/CGqlPublisherCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants_fwd.h>


namespace imtauthgql
{


/**
	Server-side publisher that broadcasts notifications when cross-tenant
	messages change state.

	The component observes the cross-tenant message broker
	(imtauth::ICrossTenantMessage) and publishes:
	- \c MessageReceived to the owner of the target tenant when a new incoming
	  message is detected (so the recipient learns about an inbound request);
	- \c MessageStatusChanged to the owner of the source tenant when the status
	  of an existing message changes (so the sender learns about confirmations,
	  rejections or status updates sent back through the reverse channel).

	Notifications are delivered only to subscribers whose authenticated user
	matches the resolved target userId.

	The subscription endpoint name is configurable via the \c CommandIds
	attribute and defaults to "OnCrossTenantMessageNotification".
*/
class CCrossTenantMessageNotificationPublisherComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::CMultiModelDispatcherBase
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CCrossTenantMessageNotificationPublisherComp);
		I_ASSIGN(m_messageBrokerCompPtr, "MessageBroker", "Cross-tenant message broker to observe for changes", true, "CrossTenantMessageBroker");
		I_ASSIGN_TO(m_messageBrokerModelCompPtr, m_messageBrokerCompPtr, true);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager for resolving tenant owners", false, "TenantManager");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet & changeSet) override;

protected:
	I_REF(imtauth::ICrossTenantMessage, m_messageBrokerCompPtr);
	I_REF(imod::IModel, m_messageBrokerModelCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);

private:
	void PublishNotification(
		const QByteArray& targetUserId,
		sdl::V1_0::imtauth::ECrossTenantMessageNotificationType notificationType,
		const imtauth::CrossTenantMessageInfo& message,
		const QString& tenantName) const;

	QByteArray FindTenantOwnerUserId(const QByteArray& tenantId) const;
	QString FindTenantName(const QByteArray& tenantId) const;

	// Cache of messageId → last known status for change detection.
	mutable QMap<QByteArray, imtauth::CrossTenantMessageStatus> m_cachedMessageStatus;
	// Protects the cache from concurrent access.
	mutable QRecursiveMutex m_cacheMutex;
};


} // namespace imtauthgql
