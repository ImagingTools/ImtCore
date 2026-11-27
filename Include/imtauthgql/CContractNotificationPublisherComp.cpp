// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CContractNotificationPublisherComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtauth/TenantEvents.h>
#include <imtauthgql/ContractNotificationPlan.h>
#include <imtauthgql/TenantNotifications.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Contracts.h>


namespace imtauthgql
{


namespace
{


QByteArrayList ToTenantIds(const QVariant& value)
{
	QByteArrayList retVal;
	for (const QString& tenantId : value.toStringList()){
		if (!tenantId.isEmpty()){
			retVal.append(tenantId.toUtf8());
		}
	}

	return retVal;
}


} // anonymous namespace


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CContractNotificationPublisherComp::IsOpenFor(SubscriberKind subscriberKind, const imtgql::CGqlRequest& gqlRequest) const
{
	// A session hears of the contracts of its own organization only, whatever its permissions
	if (subscriberKind == SK_RECIPIENT){
		return m_readPermissionsAttrPtr.GetCount() == 0 && gqlRequest.GetCommandId() == GetPushCommandId().toUtf8();
	}

	return BaseClass::IsOpenFor(subscriberKind, gqlRequest);
}


// reimplemented (icomp::CComponentBase)

void CContractNotificationPublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_contractModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_contractModelCompPtr.GetPtr(), 0);
	}
}


void CContractNotificationPublisherComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CContractNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& changeSet)
{
	for (const QVariantMap& event : imtauth::TenantEvent::GetEvents(changeSet)){
		const QByteArray contractId = event.value(imtauth::TenantEvent::s_contractId).toByteArray();
		if (contractId.isEmpty()){
			continue;
		}

		if (event.value(imtauth::TenantEvent::s_type).toString() == imtauth::TenantEvent::s_contractChanged){
			PushChange(
						contractId,
						ToTenantIds(event.value(imtauth::TenantEvent::s_recipientTenantIds)),
						event.value(imtauth::TenantEvent::s_userId).toByteArray(),
						event.value(imtauth::TenantEvent::s_userName).toString());

			continue;
		}

		if (!m_notificationSenderCompPtr.IsValid()){
			continue;
		}

		imtauth::IContractInfoUniquePtr contractPtr = m_contractProviderCompPtr->GetContract(contractId);
		if (!contractPtr.IsValid()){
			SendErrorMessage(0, QStringLiteral("The contract '%1' could not be read: its parties are not told").arg(QString::fromUtf8(contractId)), "CContractNotificationPublisherComp");

			continue;
		}

		Execute(ContractNotificationPlan::CreatePlan(event, *contractPtr), *contractPtr);
	}
}


// private methods

void CContractNotificationPublisherComp::PushChange(const QByteArray& contractId, const QByteArrayList& tenantIds, const QByteArray& userId, const QString& userName) const
{
	const QByteArray commandId = GetPushCommandId().toUtf8();

	for (const QByteArray& tenantId : tenantIds){
		sdl::V1_0::imtauth::CContractNotification notification;
		notification.notificationType = sdl::V1_0::imtauth::EContractNotificationType::ContractChanged;
		notification.contractId = contractId;
		notification.tenantId = tenantId;
		notification.sourceUserId = userId;
		notification.sourceUserName = userName;

		QJsonObject jsonObject;
		if (!notification.WriteToJsonObject(jsonObject)){
			Q_ASSERT(false);

			continue;
		}

		const QByteArray data = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
		PublishDataFiltered(commandId, data, [tenantId](const imtgql::CGqlRequest& gqlRequest){
			const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();

			return contextPtr != nullptr && !contextPtr->GetUserId().isEmpty() && contextPtr->GetTenantId() == tenantId;
		});
	}
}


void CContractNotificationPublisherComp::Execute(const ContractNotificationPlan::Plan& plan, const imtauth::IContractInfo& contract) const
{
	namespace tn = TenantNotification;

	for (const ContractNotificationPlan::Withdrawal& withdrawal : plan.withdrawals){
		tn::Withdraw(m_notificationSenderCompPtr.GetPtr(), withdrawal.tenantId, withdrawal.groupKey);
	}

	if (plan.deliveries.isEmpty()){
		return;
	}

	const QString subjectName = contract.GetNumber().isEmpty() ? contract.GetName() : contract.GetNumber() + QStringLiteral(" ") + contract.GetName();
	const imtnotify::ObjectLinkInfo actor = tn::RequestUserLink(m_userCollectionCompPtr.GetPtr());

	for (const ContractNotificationPlan::Delivery& delivery : plan.deliveries){
		imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(delivery.tenantId);
		if (!tenantPtr.IsValid()){
			SendErrorMessage(0, QStringLiteral("The organization '%1' could not be read: it is not told about its contract").arg(QString::fromUtf8(delivery.tenantId)), "CContractNotificationPublisherComp");

			continue;
		}

		imtauth::IContractPartyPrivateInfoUniquePtr privateInfoPtr = m_contractProviderCompPtr->GetPrivateInfo(contract.GetContractId(), delivery.tenantId);
		const QByteArray responsibleUserId = privateInfoPtr.IsValid() ? privateInfoPtr->GetResponsibleUserId() : QByteArray();
		bool isResponsibleMember = false;
		if (!responsibleUserId.isEmpty()){
			imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->FindMembership(responsibleUserId, delivery.tenantId);
			isResponsibleMember = membershipPtr.IsValid() && membershipPtr->IsActive();
		}

		// An organization that is no party any more only learns the name of the contract
		imtnotify::ObjectLinkInfo subject;
		subject.id = delivery.isSubjectLinked ? contract.GetContractId() : QByteArray();
		subject.typeId = delivery.isSubjectLinked ? QByteArrayLiteral("TenantContract") : QByteArray();
		subject.name = subjectName;

		imtnotify::NotificationArguments arguments;
		if (!delivery.partnerTenantId.isEmpty()){
			arguments.append(imtnotify::NotificationArgument::Object(QByteArrayLiteral("partner"), tn::TenantLink(delivery.partnerTenantId, FindTenantName(delivery.partnerTenantId))));
		}

		if (delivery.date.isValid()){
			arguments.append(imtnotify::NotificationArgument::DateTime(QByteArrayLiteral("date"), delivery.date));
		}

		const QByteArrayList userIds = ContractNotificationPlan::SelectRecipients(tenantPtr->GetOwnerId(), responsibleUserId, isResponsibleMember, actor.id);
		for (const QByteArray& userId : userIds){
			tn::Send(m_notificationSenderCompPtr.GetPtr(), userId, delivery.tenantId, delivery.typeId, subject, delivery.groupKey, delivery.severity, actor, arguments);
		}
	}
}


QString CContractNotificationPublisherComp::FindTenantName(const QByteArray& tenantId) const
{
	imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	return tenantPtr.IsValid() ? tenantPtr->GetTenantName() : QString();
}


QString CContractNotificationPublisherComp::GetPushCommandId() const
{
	return (m_commandIdsAttrPtr.IsValid() && m_commandIdsAttrPtr.GetCount() > 0)
				? QString::fromUtf8(m_commandIdsAttrPtr[0])
				: QStringLiteral("OnContractNotification");
}


} // namespace imtauthgql


