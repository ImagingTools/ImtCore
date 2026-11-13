// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantMembershipManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtauth/ITenantMembership.h>
#include <imtauth/ITenantEntityBindingManager.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/TenantEvents.h>
#include <imtauth/IUserInfo.h>
#include <imtauth/CollectionSelection.h>


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

	for (const QByteArray& id : GetCandidateMembershipIds(QByteArray(), tenantId)){
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

	for (const QByteArray& id : GetCandidateMembershipIds(userId, QByteArray())){
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

	for (const QByteArray& id : GetCandidateMembershipIds(userId, tenantId)){
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

	if (!imtbase::isValidUuid(userId) || !imtbase::isValidUuid(tenantId)){
		SendErrorMessage(0, QStringLiteral("Cannot add membership: userId or tenantId is not a UUID (userId='%1', tenantId='%2')")
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

	TenantEvent::Publish(this, {
				{TenantEvent::s_type, TenantEvent::s_membershipAdded},
				{TenantEvent::s_membershipId, membershipId},
				{TenantEvent::s_userId, userId},
				{TenantEvent::s_tenantId, tenantId}});

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

	// The user and the organization are read before the removal: without them the bindings and the notifications
	// would be lost, and a removal already done could not be repeated
	QByteArray userId;
	QByteArray tenantId;
	{
		imtbase::IObjectCollection::DataPtr dataPtr;
		const ITenantMembership* membershipPtr = m_membershipCollectionCompPtr->GetObjectData(membershipId, dataPtr)
					? dynamic_cast<const ITenantMembership*>(dataPtr.GetPtr())
					: nullptr;
		if (membershipPtr == nullptr){
			SendErrorMessage(0, QStringLiteral("Membership '%1' could not be read").arg(QString::fromUtf8(membershipId)), "CTenantMembershipManagerComp");
			return false;
		}

		userId = membershipPtr->GetUserId();
		tenantId = membershipPtr->GetTenantId();
	}

	// An organization that cannot be read may be owned by the member
	if (m_tenantManagerCompPtr.IsValid()){
		ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
		if (!tenantPtr.IsValid()){
			SendErrorMessage(0, QStringLiteral("The organization of membership '%1' could not be read").arg(QString::fromUtf8(membershipId)), "CTenantMembershipManagerComp");
			return false;
		}
		if (tenantPtr->GetOwnerId() == userId){
			SendErrorMessage(0, QStringLiteral("Cannot remove the tenant owner's membership (membership '%1')").arg(membershipId), QStringLiteral("CTenantMembershipManagerComp"));
			return false;
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

	TenantEvent::Publish(this, {
				{TenantEvent::s_type, TenantEvent::s_membershipRemoved},
				{TenantEvent::s_membershipId, membershipId},
				{TenantEvent::s_userId, userId},
				{TenantEvent::s_tenantId, tenantId}});

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

	QByteArrayList previousPermissions = membershipPtr->GetOrganizationPermissions();
	QByteArrayList newPermissions = permissions;
	std::sort(previousPermissions.begin(), previousPermissions.end());
	std::sort(newPermissions.begin(), newPermissions.end());

	membershipPtr->SetOrganizationPermissions(permissions);

	if (!m_membershipCollectionCompPtr->SetObjectData(membershipId, *membershipPtr)){
		SendErrorMessage(0, QStringLiteral("Failed to update membership '%1' permissions").arg(membershipId), "CTenantMembershipManagerComp");
		return false;
	}

	if (previousPermissions != newPermissions){
		TenantEvent::Publish(this, {
					{TenantEvent::s_type, TenantEvent::s_membershipPermissionsChanged},
					{TenantEvent::s_membershipId, membershipId},
					{TenantEvent::s_userId, membershipPtr->GetUserId()},
					{TenantEvent::s_tenantId, membershipPtr->GetTenantId()}});
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

QByteArrayList CTenantMembershipManagerComp::GetCandidateMembershipIds(const QByteArray& userId, const QByteArray& tenantId) const
{
	if (userId.isEmpty() && tenantId.isEmpty()){
		return QByteArrayList();
	}

	CollectionSelection::FilterExpression filterExpr;
	if (!userId.isEmpty()){
		filterExpr.fieldFilters << CollectionSelection::FieldFilter("UserId", QString::fromUtf8(userId));
	}
	if (!tenantId.isEmpty()){
		filterExpr.fieldFilters << CollectionSelection::FieldFilter("TenantId", QString::fromUtf8(tenantId));
	}

	return CollectionSelection::SelectIds(*m_membershipCollectionCompPtr, filterExpr);
}


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


} // namespace imtauth
