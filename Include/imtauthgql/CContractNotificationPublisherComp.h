// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtauth/IContractProvider.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtbase/IObjectCollection.h>
#include <imtnotify/INotificationSender.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtauthgql
{


/**
	Tells the parties about their contracts.
	Every change goes to the sessions of the parties (OnContractNotification), so their views read the contract again.
	The events a party acts on go to the notification center of its owner and of the user responsible for the contract there.
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
	virtual bool IsOpenFor(SubscriberKind subscriberKind, const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	/// The group of a contract a notification joins: one row per inbox, withdrawn when it is no longer true
	enum NotificationGroup
	{
		NG_NONE,
		/// The party is to answer: review, sign, change the terms
		NG_PENDING,
		/// The signatures the initiator collects
		NG_SIGNED
	};

	struct NotificationKind
	{
		QByteArray typeId;
		istd::IInformationProvider::InformationCategory severity = istd::IInformationProvider::IC_INFO;
		NotificationGroup group = NG_NONE;
	};

	/**
		The notification of a contract event; an empty type for an event the parties are not told about.
	*/
	static NotificationKind GetNotificationKind(const QString& eventType);

	/// The grouped notifications an event makes untrue
	enum StepEnd
	{
		SE_NONE,
		/// The party that acted answered its pending notification
		SE_ACTOR_ANSWERED,
		/// The initiator changed the terms: what it was asked and the signatures it collected are gone
		SE_TERMS_CHANGED,
		/// The parties told are out of the contract
		SE_RECIPIENTS_REMOVED,
		/// The negotiation ended for all parties
		SE_ALL
	};

	static StepEnd GetStepEnd(const QString& eventType);

	void PushChange(const QByteArray& contractId, const QByteArrayList& tenantIds, const QByteArray& userId, const QString& userName) const;
	void SendToNotificationCenter(const QVariantMap& event, const NotificationKind& kind, const QByteArrayList& tenantIds) const;
	void EndStep(const QVariantMap& event, StepEnd stepEnd, const QByteArrayList& recipientTenantIds) const;
	void WithdrawGroup(const QByteArray& groupKey, const QByteArrayList& tenantIds) const;
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


