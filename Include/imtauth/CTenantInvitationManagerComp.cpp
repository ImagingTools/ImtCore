// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantInvitationManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtauth/TenantEvents.h>
#include <imtdb/imtdb.h>
#include <imtauth/CollectionSelection.h>


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
	if (idText.compare(imtdb::NULL_DATA_LITERAL, Qt::CaseInsensitive) == 0){
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

	for (const QByteArray& id : GetCandidateInvitationIds("TenantId", tenantId)){
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

	for (const QByteArray& id : GetCandidateInvitationIds("UserId", userId)){
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


QByteArray CTenantInvitationManagerComp::CreateInvitation(const QByteArray& invitedByUserId, const QByteArray& userId, const QByteArray& tenantId)
{
	if (!m_invitationCollectionCompPtr.IsValid() || !m_invitationFactoryCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		SendErrorMessage(0, "Invitation collection, factory or membership manager not configured", "CTenantInvitationManagerComp");
		return QByteArray();
	}

	if (!imtbase::isValidUuid(invitedByUserId) || !imtbase::isValidUuid(userId) || !imtbase::isValidUuid(tenantId)){
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
	invitationPtr->SetStatus(ITenantInvitation::TIS_PENDING);
	invitationPtr->SetInvitedByUserId(invitedByUserId);
	invitationPtr->SetCreatedAt(now);
	invitationPtr->SetUpdatedAt(now);
	invitationPtr->SetExpiresAt(expiresAt);
	invitationPtr->SetLastSentAt(now);

	QByteArray storedId = m_invitationCollectionCompPtr->InsertNewObject("TenantInvitation", QString(), QString(), invitationPtr.GetPtr(), invitationId);
	if (storedId.isEmpty()){
		return QByteArray();
	}

	PublishInvitationEvent(TenantEvent::s_invitationSent, *invitationPtr);

	return invitationId;
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
		invitationPtr->GetTenantId());
	if (membershipId.isEmpty()){
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	invitationPtr->SetStatus(ITenantInvitation::TIS_ACCEPTED);
	invitationPtr->SetAcceptedAt(now);
	invitationPtr->SetUpdatedAt(now);

	if (StoreInvitation(invitationId, *invitationPtr)){
		PublishInvitationEvent(TenantEvent::s_invitationStatusChanged, *invitationPtr);
	}

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

	if (!StoreInvitation(invitationId, *invitationPtr)){
		return false;
	}

	PublishInvitationEvent(TenantEvent::s_invitationStatusChanged, *invitationPtr);

	return true;
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

	if (!StoreInvitation(invitationId, *invitationPtr)){
		return false;
	}

	PublishInvitationEvent(TenantEvent::s_invitationStatusChanged, *invitationPtr);

	return true;
}


bool CTenantInvitationManagerComp::RevokeInvitationsSentBy(const QByteArray& tenantId, const QByteArray& invitedByUserId, const QByteArray& revokedByUserId)
{
	if (!m_invitationCollectionCompPtr.IsValid()){
		return false;
	}

	if (tenantId.isEmpty() || invitedByUserId.isEmpty()){
		return true;
	}

	// An invitation that cannot be read may be a pending one of the member
	bool isRevoked = true;
	for (const QByteArray& invitationId : GetCandidateInvitationIds("TenantId", tenantId, &isRevoked)){
		ITenantInvitationUniquePtr invitationPtr = GetInvitation(invitationId);
		if (!invitationPtr.IsValid()){
			isRevoked = false;
			continue;
		}

		if (invitationPtr->GetTenantId() == tenantId && invitationPtr->GetInvitedByUserId() == invitedByUserId
					&& GetEffectiveStatus(*invitationPtr) == ITenantInvitation::TIS_PENDING && !RevokeInvitation(invitationId, revokedByUserId)){
			isRevoked = false;
		}
	}

	return isRevoked;
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

	if (!StoreInvitation(invitationId, *invitationPtr)){
		return false;
	}

	PublishInvitationEvent(TenantEvent::s_invitationSent, *invitationPtr);

	return true;
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


QByteArrayList CTenantInvitationManagerComp::GetCandidateInvitationIds(const QByteArray& columnId, const QByteArray& value, bool* isReadPtr) const
{
	return CollectionSelection::SelectIds(*m_invitationCollectionCompPtr, CollectionSelection::ColumnEquals(columnId, value), QByteArrayList(), isReadPtr);
}


bool CTenantInvitationManagerComp::StatusMatches(const ITenantInvitation& invitation, const Statuses& statuses) const
{
	if (statuses.isEmpty()){
		return true;
	}

	return statuses.contains(GetEffectiveStatus(invitation));
}


void CTenantInvitationManagerComp::PublishInvitationEvent(const QString& eventType, const ITenantInvitation& invitation)
{
	TenantEvent::Publish(this, {
				{TenantEvent::s_type, eventType},
				{TenantEvent::s_invitationId, invitation.GetInvitationId()},
				{TenantEvent::s_userId, invitation.GetUserId()},
				{TenantEvent::s_tenantId, invitation.GetTenantId()},
				{TenantEvent::s_invitedByUserId, invitation.GetInvitedByUserId()},
				{TenantEvent::s_status, int(invitation.GetStatus())}});
}


bool CTenantInvitationManagerComp::StoreInvitation(const QByteArray& invitationId, ITenantInvitation& invitation)
{
	if (!m_invitationCollectionCompPtr.IsValid()){
		return false;
	}

	return m_invitationCollectionCompPtr->SetObjectData(invitationId, invitation);
}


} // namespace imtauth
