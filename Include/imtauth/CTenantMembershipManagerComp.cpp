// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantMembershipManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/ITenantMembership.h>
#include <imtauth/ITenantManager.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::ITenantMembershipManager)

ITenantMembershipManager::MembershipIds CTenantMembershipManagerComp::GetMembershipsByTenant(const QByteArray& tenantId) const
{
	MembershipIds result;

	if (!m_membershipCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Membership collection not configured", "CTenantMembershipManagerComp");
		return result;
	}

	QByteArrayList allIds = m_membershipCollectionCompPtr->GetElementIds();
	for (const QByteArray& id : allIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_membershipCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantMembership* membershipPtr = dynamic_cast<const ITenantMembership*>(dataPtr.GetPtr());
			if (membershipPtr != nullptr && membershipPtr->GetTenantId() == tenantId){
				result.append(id);
			}
		}
	}

	return result;
}


ITenantMembershipManager::MembershipIds CTenantMembershipManagerComp::GetMembershipsByUser(const QByteArray& userId) const
{
	MembershipIds result;

	if (!m_membershipCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Membership collection not configured", "CTenantMembershipManagerComp");
		return result;
	}

	QByteArrayList allIds = m_membershipCollectionCompPtr->GetElementIds();
	for (const QByteArray& id : allIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_membershipCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantMembership* membershipPtr = dynamic_cast<const ITenantMembership*>(dataPtr.GetPtr());
			if (membershipPtr != nullptr && membershipPtr->GetUserId() == userId){
				result.append(id);
			}
		}
	}

	return result;
}


ITenantMembershipUniquePtr CTenantMembershipManagerComp::GetMembership(const QByteArray& membershipId) const
{
	if (!m_membershipCollectionCompPtr.IsValid() || !m_membershipFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Membership collection or factory not configured", "CTenantMembershipManagerComp");
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_membershipCollectionCompPtr->GetObjectData(membershipId, dataPtr)){
		return nullptr;
	}

	const ITenantMembership* membershipPtr = dynamic_cast<const ITenantMembership*>(dataPtr.GetPtr());
	if (membershipPtr == nullptr){
		return nullptr;
	}

	ITenantMembershipUniquePtr clonedMembership = m_membershipFactoryCompPtr.CreateInstance();
	if (!clonedMembership.IsValid()){
		return nullptr;
	}

	if (!clonedMembership->CopyFrom(*membershipPtr)){
		return nullptr;
	}

	return clonedMembership;
}


ITenantMembershipUniquePtr CTenantMembershipManagerComp::FindMembership(const QByteArray& userId, const QByteArray& tenantId) const
{
	if (!m_membershipCollectionCompPtr.IsValid() || !m_membershipFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Membership collection or factory not configured", "CTenantMembershipManagerComp");
		return nullptr;
	}

	QByteArrayList allIds = m_membershipCollectionCompPtr->GetElementIds();
	for (const QByteArray& id : allIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_membershipCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantMembership* membershipPtr = dynamic_cast<const ITenantMembership*>(dataPtr.GetPtr());
			if (membershipPtr != nullptr && membershipPtr->GetUserId() == userId && membershipPtr->GetTenantId() == tenantId){
				ITenantMembershipUniquePtr clonedMembership = m_membershipFactoryCompPtr.CreateInstance();
				if (clonedMembership.IsValid() && clonedMembership->CopyFrom(*membershipPtr)){
					return clonedMembership;
				}
				return nullptr;
			}
		}
	}

	return nullptr;
}


QByteArray CTenantMembershipManagerComp::AddMembership(const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole role)
{
	if (!m_membershipCollectionCompPtr.IsValid() || !m_membershipFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Membership collection or factory not configured", "CTenantMembershipManagerComp");
		return QByteArray();
	}

	if (userId.isEmpty() || tenantId.isEmpty()){
		SendErrorMessage(0, QString("Cannot add membership: userId or tenantId is empty (userId='%1', tenantId='%2')")
			.arg(QString::fromUtf8(userId), QString::fromUtf8(tenantId)), "CTenantMembershipManagerComp");
		return QByteArray();
	}

	// Check if membership already exists
	if (FindMembership(userId, tenantId).IsValid()){
		SendErrorMessage(0, QString("User '%1' is already a member of tenant '%2'")
			.arg(QString::fromUtf8(userId), QString::fromUtf8(tenantId)), "CTenantMembershipManagerComp");
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	QByteArray membershipId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	ITenantMembershipUniquePtr membershipPtr = m_membershipFactoryCompPtr.CreateInstance();
	if (!membershipPtr.IsValid()){
		SendErrorMessage(0, "Failed to create membership object", "CTenantMembershipManagerComp");
		return QByteArray();
	}

	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

	membershipPtr->SetMembershipId(membershipId);
	membershipPtr->SetUserId(userId);
	membershipPtr->SetTenantId(tenantId);
	membershipPtr->SetRole(role);
	membershipPtr->SetActive(true);
	membershipPtr->SetJoinedAt(now);

	QByteArray retVal = m_membershipCollectionCompPtr->InsertNewObject("TenantMembership", QString(), QString(), membershipPtr.GetPtr(), membershipId);
	if (retVal.isEmpty()){
		SendErrorMessage(0, "Failed to store membership in collection", "CTenantMembershipManagerComp");
		return QByteArray();
	}

	SendInfoMessage(0, QString("Added membership for user '%1' in tenant '%2' with role %3")
		.arg(QString::fromUtf8(userId), QString::fromUtf8(tenantId)).arg(static_cast<int>(role)), "CTenantMembershipManagerComp");

	return membershipId;
}


QByteArray CTenantMembershipManagerComp::InviteMembership(const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole role)
{
	Q_UNUSED(userId);
	Q_UNUSED(tenantId);
	Q_UNUSED(role);
	SendErrorMessage(0, "Tenant invitations are stored separately; use ITenantInvitationManager::CreateInvitation", "CTenantMembershipManagerComp");
	return QByteArray();
}


bool CTenantMembershipManagerComp::RemoveMembership(const QByteArray& membershipId)
{
	if (!m_membershipCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Membership collection not configured", "CTenantMembershipManagerComp");
		return false;
	}

	if (IsOwnerMembership(membershipId)){
		SendErrorMessage(0, QString("Cannot remove the tenant owner's membership (membership '%1')").arg(QString::fromUtf8(membershipId)), "CTenantMembershipManagerComp");
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	if (!m_membershipCollectionCompPtr->RemoveElements({membershipId})){
		SendErrorMessage(0, QString("Failed to remove membership '%1'").arg(QString::fromUtf8(membershipId)), "CTenantMembershipManagerComp");
		return false;
	}

	SendInfoMessage(0, QString("Removed membership '%1'").arg(QString::fromUtf8(membershipId)), "CTenantMembershipManagerComp");

	return true;
}


bool CTenantMembershipManagerComp::AcceptMembershipInvitation(const QByteArray& membershipId)
{
	if (!m_membershipCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Membership collection not configured", "CTenantMembershipManagerComp");
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_membershipCollectionCompPtr->GetObjectData(membershipId, dataPtr)){
		return false;
	}

	ITenantMembership* membershipPtr = dynamic_cast<ITenantMembership*>(dataPtr.GetPtr());
	if (membershipPtr == nullptr){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	membershipPtr->SetActive(true);
	membershipPtr->SetJoinedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
	membershipPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

	return m_membershipCollectionCompPtr->SetObjectData(membershipId, *membershipPtr);
}


bool CTenantMembershipManagerComp::RejectMembershipInvitation(const QByteArray& membershipId)
{
	return RemoveMembership(membershipId);
}


bool CTenantMembershipManagerComp::UpdateMembershipRole(const QByteArray& membershipId, ITenantMembership::TenantMemberRole newRole)
{
	if (!m_membershipCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Membership collection not configured", "CTenantMembershipManagerComp");
		return false;
	}

	if (IsOwnerMembership(membershipId)){
		SendErrorMessage(0, QString("Cannot change the role of the tenant owner (membership '%1')").arg(QString::fromUtf8(membershipId)), "CTenantMembershipManagerComp");
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_membershipCollectionCompPtr->GetObjectData(membershipId, dataPtr)){
		SendErrorMessage(0, QString("Membership '%1' not found").arg(QString::fromUtf8(membershipId)), "CTenantMembershipManagerComp");
		return false;
	}

	ITenantMembership* membershipPtr = dynamic_cast<ITenantMembership*>(dataPtr.GetPtr());
	if (membershipPtr == nullptr){
		SendErrorMessage(0, "Invalid membership object", "CTenantMembershipManagerComp");
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	membershipPtr->SetRole(newRole);

	if (!m_membershipCollectionCompPtr->SetObjectData(membershipId, *membershipPtr)){
		SendErrorMessage(0, QString("Failed to update membership '%1'").arg(QString::fromUtf8(membershipId)), "CTenantMembershipManagerComp");
		return false;
	}

	SendInfoMessage(0, QString("Updated membership '%1' role to %2").arg(QString::fromUtf8(membershipId)).arg(static_cast<int>(newRole)), "CTenantMembershipManagerComp");

	return true;
}


bool CTenantMembershipManagerComp::IsMember(const QByteArray& userId, const QByteArray& tenantId) const
{
	ITenantMembershipUniquePtr membershipPtr = FindMembership(userId, tenantId);
	return membershipPtr.IsValid() && membershipPtr->IsActive();
}


bool CTenantMembershipManagerComp::HasMinimumRole(const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole minimumRole) const
{
	ITenantMembershipUniquePtr membershipPtr = FindMembership(userId, tenantId);
	if (!membershipPtr.IsValid()){
		return false;
	}
	if (!membershipPtr->IsActive()){
		return false;
	}

	// Role hierarchy: Owner(0) > Admin(1) > Member(2) > Guest(3)
	// Lower enum value = higher privilege
	return static_cast<int>(membershipPtr->GetRole()) <= static_cast<int>(minimumRole);
}


// private methods

bool CTenantMembershipManagerComp::IsOwnerMembership(const QByteArray& membershipId) const
{
	if (!m_tenantManagerCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_membershipCollectionCompPtr->GetObjectData(membershipId, dataPtr)){
		return false;
	}

	const ITenantMembership* membershipPtr = dynamic_cast<const ITenantMembership*>(dataPtr.GetPtr());
	if (membershipPtr == nullptr){
		return false;
	}

	ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(membershipPtr->GetTenantId());
	if (!tenantPtr.IsValid()){
		return false;
	}

	return tenantPtr->GetOwnerId() == membershipPtr->GetUserId();
}


} // namespace imtauth
