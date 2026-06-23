// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantInvitationManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtauth
{


namespace
{


constexpr int TenantInvitationExpirationDays = 7;


QString CreateInvitationExpirationTime()
{
	return QDateTime::currentDateTimeUtc().addDays(TenantInvitationExpirationDays).toString(Qt::ISODateWithMs);
}


bool IsValidInvitationId(const QByteArray& invitationId)
{
	if (invitationId.isEmpty()){
		return false;
	}

	QString idText = QString::fromUtf8(invitationId).trimmed();
	if (idText.compare(QStringLiteral("null"), Qt::CaseInsensitive) == 0){
		return false;
	}

	if (!idText.startsWith('{')){
		idText.prepend('{');
	}
	if (!idText.endsWith('}')){
		idText.append('}');
	}

	const QUuid uuid(idText);
	return !uuid.isNull();
}


} // anonymous namespace


ITenantInvitationManager::InvitationIds CTenantInvitationManagerComp::GetInvitationsByTenant(const QByteArray& tenantId, const Statuses& statuses) const
{
	InvitationIds result;
	if (!m_invitationCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_invitationCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_invitationCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantInvitation* invitationPtr = dynamic_cast<const ITenantInvitation*>(dataPtr.GetPtr());
			if (invitationPtr != nullptr && invitationPtr->GetTenantId() == tenantId && StatusMatches(*invitationPtr, statuses)){
				result.append(id);
			}
		}
	}

	return result;
}


ITenantInvitationManager::InvitationIds CTenantInvitationManagerComp::GetInvitationsByUser(const QByteArray& userId, const Statuses& statuses) const
{
	InvitationIds result;
	if (!m_invitationCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_invitationCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_invitationCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantInvitation* invitationPtr = dynamic_cast<const ITenantInvitation*>(dataPtr.GetPtr());
			if (invitationPtr != nullptr && invitationPtr->GetUserId() == userId && StatusMatches(*invitationPtr, statuses)){
				result.append(id);
			}
		}
	}

	return result;
}


ITenantInvitationUniquePtr CTenantInvitationManagerComp::GetInvitation(const QByteArray& invitationId) const
{
	if (!m_invitationCollectionCompPtr.IsValid() || !m_invitationFactoryCompPtr.IsValid()){
		return nullptr;
	}
	if (!IsValidInvitationId(invitationId)){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_invitationCollectionCompPtr->GetObjectData(invitationId, dataPtr)){
		return nullptr;
	}

	const ITenantInvitation* invitationPtr = dynamic_cast<const ITenantInvitation*>(dataPtr.GetPtr());
	if (invitationPtr == nullptr){
		return nullptr;
	}

	ITenantInvitationUniquePtr clonePtr = m_invitationFactoryCompPtr.CreateInstance();
	if (clonePtr.IsValid() && clonePtr->CopyFrom(*invitationPtr)){
		return clonePtr;
	}

	return nullptr;
}


ITenantInvitationUniquePtr CTenantInvitationManagerComp::FindPendingInvitation(const QByteArray& userId, const QByteArray& tenantId) const
{
	Statuses statuses;
	statuses.append(ITenantInvitation::TIS_PENDING);
	for (const QByteArray& id : GetInvitationsByTenant(tenantId, statuses)){
		ITenantInvitationUniquePtr invitationPtr = GetInvitation(id);
		if (invitationPtr.IsValid() && invitationPtr->GetUserId() == userId){
			return invitationPtr;
		}
	}

	return nullptr;
}


QByteArray CTenantInvitationManagerComp::CreateInvitation(const QByteArray& invitedByUserId, const QByteArray& userId, const QByteArray& tenantId, const QByteArray& roleId)
{
	if (!m_invitationCollectionCompPtr.IsValid() || !m_invitationFactoryCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		SendErrorMessage(0, "Invitation collection, factory or membership manager not configured", "CTenantInvitationManagerComp");
		return QByteArray();
	}

	if (invitedByUserId.isEmpty() || userId.isEmpty() || tenantId.isEmpty()){
		return QByteArray();
	}

	if (m_membershipManagerCompPtr->IsMember(userId, tenantId)){
		SendErrorMessage(0, "User is already an active tenant member", "CTenantInvitationManagerComp");
		return QByteArray();
	}

	if (FindPendingInvitation(userId, tenantId).IsValid()){
		SendErrorMessage(0, "Pending tenant invitation already exists", "CTenantInvitationManagerComp");
		return QByteArray();
	}

	ITenantInvitationUniquePtr invitationPtr = m_invitationFactoryCompPtr.CreateInstance();
	if (!invitationPtr.IsValid()){
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	QByteArray invitationId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	QString expiresAt = CreateInvitationExpirationTime();

	invitationPtr->SetInvitationId(invitationId);
	invitationPtr->SetUserId(userId);
	invitationPtr->SetTenantId(tenantId);
	invitationPtr->SetRoleId(roleId);
	invitationPtr->SetStatus(ITenantInvitation::TIS_PENDING);
	invitationPtr->SetInvitedByUserId(invitedByUserId);
	invitationPtr->SetCreatedAt(now);
	invitationPtr->SetUpdatedAt(now);
	invitationPtr->SetExpiresAt(expiresAt);
	invitationPtr->SetLastSentAt(now);

	QByteArray storedId = m_invitationCollectionCompPtr->InsertNewObject("TenantInvitation", QString(), QString(), invitationPtr.GetPtr(), invitationId);
	return storedId.isEmpty() ? QByteArray() : invitationId;
}


QByteArray CTenantInvitationManagerComp::AcceptInvitation(const QByteArray& invitationId, const QByteArray& userId)
{
	if (!m_invitationCollectionCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		return QByteArray();
	}
	if (!IsValidInvitationId(invitationId)){
		return QByteArray();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_invitationCollectionCompPtr->GetObjectData(invitationId, dataPtr)){
		return QByteArray();
	}

	ITenantInvitation* invitationPtr = dynamic_cast<ITenantInvitation*>(dataPtr.GetPtr());
	if (invitationPtr == nullptr || invitationPtr->GetUserId() != userId || GetEffectiveStatus(*invitationPtr) != ITenantInvitation::TIS_PENDING){
		return QByteArray();
	}

	QByteArray membershipId = m_membershipManagerCompPtr->AddMembership(
		invitationPtr->GetUserId(),
		invitationPtr->GetTenantId(),
		invitationPtr->GetRoleId().isEmpty() ? QByteArrayLiteral("Member") : invitationPtr->GetRoleId());
	if (membershipId.isEmpty()){
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	invitationPtr->SetStatus(ITenantInvitation::TIS_ACCEPTED);
	invitationPtr->SetAcceptedAt(now);
	invitationPtr->SetUpdatedAt(now);

	StoreInvitation(invitationId, *invitationPtr);
	return membershipId;
}


bool CTenantInvitationManagerComp::RejectInvitation(const QByteArray& invitationId, const QByteArray& userId)
{
	if (!m_invitationCollectionCompPtr.IsValid()){
		return false;
	}
	if (!IsValidInvitationId(invitationId)){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_invitationCollectionCompPtr->GetObjectData(invitationId, dataPtr)){
		return false;
	}

	ITenantInvitation* invitationPtr = dynamic_cast<ITenantInvitation*>(dataPtr.GetPtr());
	if (invitationPtr == nullptr || invitationPtr->GetUserId() != userId || GetEffectiveStatus(*invitationPtr) != ITenantInvitation::TIS_PENDING){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	invitationPtr->SetStatus(ITenantInvitation::TIS_REJECTED);
	invitationPtr->SetRejectedAt(now);
	invitationPtr->SetUpdatedAt(now);

	return StoreInvitation(invitationId, *invitationPtr);
}


bool CTenantInvitationManagerComp::RevokeInvitation(const QByteArray& invitationId, const QByteArray& revokedByUserId)
{
	if (!m_invitationCollectionCompPtr.IsValid()){
		return false;
	}
	if (!IsValidInvitationId(invitationId)){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_invitationCollectionCompPtr->GetObjectData(invitationId, dataPtr)){
		return false;
	}

	ITenantInvitation* invitationPtr = dynamic_cast<ITenantInvitation*>(dataPtr.GetPtr());
	if (invitationPtr == nullptr || GetEffectiveStatus(*invitationPtr) != ITenantInvitation::TIS_PENDING){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	invitationPtr->SetStatus(ITenantInvitation::TIS_REVOKED);
	invitationPtr->SetRevokedAt(now);
	invitationPtr->SetRevokedByUserId(revokedByUserId);
	invitationPtr->SetUpdatedAt(now);

	return StoreInvitation(invitationId, *invitationPtr);
}


bool CTenantInvitationManagerComp::ResendInvitation(const QByteArray& invitationId)
{
	if (!m_invitationCollectionCompPtr.IsValid()){
		return false;
	}
	if (!IsValidInvitationId(invitationId)){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_invitationCollectionCompPtr->GetObjectData(invitationId, dataPtr)){
		return false;
	}

	ITenantInvitation* invitationPtr = dynamic_cast<ITenantInvitation*>(dataPtr.GetPtr());
	if (invitationPtr == nullptr || GetEffectiveStatus(*invitationPtr) != ITenantInvitation::TIS_PENDING){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	invitationPtr->SetLastSentAt(now);
	invitationPtr->SetUpdatedAt(now);
	invitationPtr->SetExpiresAt(CreateInvitationExpirationTime());

	return StoreInvitation(invitationId, *invitationPtr);
}


ITenantInvitation::TenantInvitationStatus CTenantInvitationManagerComp::GetEffectiveStatus(const ITenantInvitation& invitation) const
{
	if (invitation.GetStatus() == ITenantInvitation::TIS_PENDING && !invitation.GetExpiresAt().isEmpty()){
		QDateTime expiresAt = QDateTime::fromString(invitation.GetExpiresAt(), Qt::ISODateWithMs);
		if (expiresAt.isValid() && expiresAt < QDateTime::currentDateTimeUtc()){
			return ITenantInvitation::TIS_EXPIRED;
		}
	}

	return invitation.GetStatus();
}


bool CTenantInvitationManagerComp::StatusMatches(const ITenantInvitation& invitation, const Statuses& statuses) const
{
	if (statuses.isEmpty()){
		return true;
	}

	return statuses.contains(GetEffectiveStatus(invitation));
}


bool CTenantInvitationManagerComp::StoreInvitation(const QByteArray& invitationId, ITenantInvitation& invitation)
{
	if (!m_invitationCollectionCompPtr.IsValid()){
		return false;
	}

	return m_invitationCollectionCompPtr->SetObjectData(invitationId, invitation);
}


} // namespace imtauth
