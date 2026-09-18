// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantMembershipManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/ITenantMembership.h>
#include <imtauth/ITenantEntityBindingManager.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/IUserInfo.h>


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


QByteArray CTenantMembershipManagerComp::AddMembership(const QByteArray& userId, const QByteArray& tenantId)
{
	if (!m_membershipCollectionCompPtr.IsValid() || !m_membershipFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Membership collection or factory not configured", "CTenantMembershipManagerComp");
		return QByteArray();
	}

	if (userId.isEmpty() || tenantId.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Cannot add membership: userId or tenantId is empty (userId='%1', tenantId='%2')")
			.arg(userId, tenantId), QStringLiteral("CTenantMembershipManagerComp"));
		return QByteArray();
	}

	// Check if membership already exists
	if (FindMembership(userId, tenantId).IsValid()){
		SendErrorMessage(0, QStringLiteral("User '%1' is already a member of tenant '%2'")
			.arg(userId, tenantId), QStringLiteral("CTenantMembershipManagerComp"));
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
	membershipPtr->SetActive(true);
	membershipPtr->SetJoinedAt(now);

	QByteArray retVal = m_membershipCollectionCompPtr->InsertNewObject("TenantMembership", QString(), QString(), membershipPtr.GetPtr(), membershipId);
	if (retVal.isEmpty()){
		SendErrorMessage(0, "Failed to store membership in collection", "CTenantMembershipManagerComp");
		return QByteArray();
	}

	// Create TenantEntityBinding for the new member
	if (m_bindingManagerCompPtr.IsValid()){
		QByteArray entityType = m_bindingEntityTypeAttrPtr.IsValid() ? *m_bindingEntityTypeAttrPtr : QByteArrayLiteral("Users");
		m_bindingManagerCompPtr->AddBinding(tenantId, entityType, userId);
	}

	SendInfoMessage(0, QStringLiteral("Added membership for user '%1' in tenant '%2'")
		.arg(userId, tenantId), QStringLiteral("CTenantMembershipManagerComp"));

	return membershipId;
}


bool CTenantMembershipManagerComp::RemoveMembership(const QByteArray& membershipId)
{
	if (!m_membershipCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Membership collection not configured", "CTenantMembershipManagerComp");
		return false;
	}

	if (IsOwnerMembership(membershipId)){
		SendErrorMessage(0, QStringLiteral("Cannot remove the tenant owner's membership (membership '%1')").arg(membershipId), QStringLiteral("CTenantMembershipManagerComp"));
		return false;
	}

	// Retrieve userId and tenantId before removing membership record
	QByteArray userId;
	QByteArray tenantId;
	{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_membershipCollectionCompPtr->GetObjectData(membershipId, dataPtr)){
			const ITenantMembership* membershipPtr = dynamic_cast<const ITenantMembership*>(dataPtr.GetPtr());
			if (membershipPtr != nullptr){
				userId = membershipPtr->GetUserId();
				tenantId = membershipPtr->GetTenantId();
			}
		}
	}

	istd::CChangeNotifier changeNotifier(this);

	if (!m_membershipCollectionCompPtr->RemoveElements({membershipId})){
		SendErrorMessage(0, QStringLiteral("Failed to remove membership '%1'").arg(membershipId), "CTenantMembershipManagerComp");
		return false;
	}

	// Clean up TenantEntityBindings for the removed user
	if (m_bindingManagerCompPtr.IsValid() && !userId.isEmpty() && !tenantId.isEmpty()){
		QByteArray entityType = m_bindingEntityTypeAttrPtr.IsValid() ? *m_bindingEntityTypeAttrPtr : QByteArrayLiteral("Users");
		if (!m_bindingManagerCompPtr->RemoveBinding(tenantId, entityType, userId)){
			SendWarningMessage(0, QStringLiteral("Failed to clean up TenantEntityBindings for user '%1' in tenant '%2'")
				.arg(QString::fromUtf8(userId), QString::fromUtf8(tenantId)), "CTenantMembershipManagerComp");
		}
	}

	if (!CleanupTenantAssignedUserData(userId, tenantId)) {
		SendWarningMessage(0, QStringLiteral("Failed to fully clean up tenant-assigned user data for user '%1' in tenant '%2'")
			.arg(QString::fromUtf8(userId), QString::fromUtf8(tenantId)), "CTenantMembershipManagerComp");
	}

	SendInfoMessage(0, QStringLiteral("Removed membership '%1'").arg(membershipId), "CTenantMembershipManagerComp");

	return true;
}


bool CTenantMembershipManagerComp::UpdateMembershipOrganizationPermissions(const QByteArray& membershipId, const QByteArrayList& permissions)
{
	if (!m_membershipCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Membership collection not configured", "CTenantMembershipManagerComp");
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_membershipCollectionCompPtr->GetObjectData(membershipId, dataPtr)){
		SendErrorMessage(0, QStringLiteral("Membership '%1' not found").arg(membershipId), "CTenantMembershipManagerComp");
		return false;
	}

	ITenantMembership* membershipPtr = dynamic_cast<ITenantMembership*>(dataPtr.GetPtr());
	if (membershipPtr == nullptr){
		SendErrorMessage(0, "Invalid membership object", "CTenantMembershipManagerComp");
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	membershipPtr->SetOrganizationPermissions(permissions);

	if (!m_membershipCollectionCompPtr->SetObjectData(membershipId, *membershipPtr)){
		SendErrorMessage(0, QStringLiteral("Failed to update membership '%1' permissions").arg(membershipId), "CTenantMembershipManagerComp");
		return false;
	}

	SendInfoMessage(0, QStringLiteral("Updated organization permissions for membership '%1'").arg(membershipId), "CTenantMembershipManagerComp");

	return true;
}


bool CTenantMembershipManagerComp::IsMember(const QByteArray& userId, const QByteArray& tenantId) const
{
	ITenantMembershipUniquePtr membershipPtr = FindMembership(userId, tenantId);
	return membershipPtr.IsValid() && membershipPtr->IsActive();
}


// private methods

bool CTenantMembershipManagerComp::CleanupTenantAssignedUserData(const QByteArray& userId, const QByteArray& tenantId)
{
	if (!m_bindingManagerCompPtr.IsValid() || !m_userCollectionCompPtr.IsValid() || userId.isEmpty() || tenantId.isEmpty()) {
		return true;
	}

	imtbase::IObjectCollection::DataPtr userDataPtr;
	if (!m_userCollectionCompPtr->GetObjectData(userId, userDataPtr)) {
		return false;
	}

	imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(userDataPtr.GetPtr());
	if (userInfoPtr == nullptr) {
		return false;
	}

	QByteArray roleEntityType = m_roleBindingEntityTypeAttrPtr.IsValid() ? *m_roleBindingEntityTypeAttrPtr : QByteArrayLiteral("Roles");
	QByteArray groupEntityType = m_groupBindingEntityTypeAttrPtr.IsValid() ? *m_groupBindingEntityTypeAttrPtr : QByteArrayLiteral("Groups");
	QByteArrayList tenantRoleIds = m_bindingManagerCompPtr->GetEntityIds(tenantId, roleEntityType);
	QByteArrayList tenantGroupIds = m_bindingManagerCompPtr->GetEntityIds(tenantId, groupEntityType);

	tenantRoleIds.removeAll("");
	tenantGroupIds.removeAll("");
	if (tenantRoleIds.isEmpty() && tenantGroupIds.isEmpty()) {
		return true;
	}

	bool hasChanges = false;

	for (const QByteArray& groupId : tenantGroupIds) {
		if (userInfoPtr->RemoveFromGroup(groupId)) {
			hasChanges = true;
		}
	}

	QByteArrayList productIds = userInfoPtr->GetProducts();
	for (const QByteArray& productId : productIds) {
		for (const QByteArray& roleId : tenantRoleIds) {
			if (userInfoPtr->RemoveRole(productId, roleId)) {
				hasChanges = true;
			}
		}
	}

	if (!hasChanges) {
		return true;
	}

	return m_userCollectionCompPtr->SetObjectData(userId, *userInfoPtr);
}


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
