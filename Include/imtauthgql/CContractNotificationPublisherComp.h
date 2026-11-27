// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtauth/IContractProvider.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauthgql/ContractNotificationPlan.h>
#include <imtbase/IObjectCollection.h>
#include <imtnotify/INotificationSender.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtauthgql
{


/**
	Tells the parties about their contracts.
	Every change goes to the sessions of the parties (OnContractNotification), so their views read the contract again,
	and to the servers of the products (OnContractsChanged), so they drop the access they keep for their sessions.
	The events a party acts on go to the notification center of the user responsible for the contract there, or of
	its owner (ContractNotificationPlan decides what is told and withdrawn).
*/
class CContractNotificationPublisherComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::CMultiModelDispatcherBase
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CContractNotificationPublisherComp);
		I_ASSIGN(m_contractProviderCompPtr, "ContractManager", "Contracts whose events are told", true, "ContractManager");
		I_ASSIGN_TO(m_contractModelCompPtr, m_contractProviderCompPtr, true);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Organizations: their owners and names", true, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Members of the organizations: a responsible user who left is not told", true, "TenantMembershipManager");
		I_ASSIGN(m_notificationSenderCompPtr, "NotificationSender", "Notification center that keeps the notifications of the users", false, "NotificationSender");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Users: the name of the user who made the change", false, "UserCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool IsOpenFor(SubscriberKind subscriberKind, const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	void PushChange(const QByteArray& contractId, const QByteArray& productId, const QByteArrayList& tenantIds, const QByteArray& userId, const QString& userName) const;
	void PushChangeToProductServers(const QByteArray& contractId, const QByteArray& productId) const;
	void Execute(const ContractNotificationPlan::Plan& plan, const imtauth::IContractInfo& contract) const;
	QString FindTenantName(const QByteArray& tenantId) const;
	QString GetPushCommandId() const;

private:
	I_REF(imtauth::IContractProvider, m_contractProviderCompPtr);
	I_REF(imod::IModel, m_contractModelCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtnotify::INotificationSender, m_notificationSenderCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauthgql


