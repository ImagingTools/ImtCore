// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantEntityBindingManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CTenantMembershipManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::ITenantMembershipManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantMembershipManagerComp);
		I_REGISTER_INTERFACE(imtauth::ITenantMembershipManager);
		I_ASSIGN(m_membershipCollectionCompPtr, "MembershipCollection", "Membership collection", true, "MembershipCollection");
		I_ASSIGN(m_membershipFactoryCompPtr, "MembershipFactory", "Membership info factory", true, "TenantMembershipInfo");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager for ownership checks", false, "TenantManager");
		I_ASSIGN(m_bindingManagerCompPtr, "BindingManager", "Tenant entity binding manager for add/remove bindings", false, "TenantEntityBindingManager");
		I_ASSIGN(m_bindingEntityTypeAttrPtr, "BindingEntityType", "EntityType value used in TenantEntityBindings (must match the Users table name)", false, "Users");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection for tenant-scoped role/group cleanup on membership removal", false, "UserCollection");
		I_ASSIGN(m_roleBindingEntityTypeAttrPtr, "RoleBindingEntityType", "EntityType value used in TenantEntityBindings for tenant-bound roles", false, "Roles");
		I_ASSIGN(m_groupBindingEntityTypeAttrPtr, "GroupBindingEntityType", "EntityType value used in TenantEntityBindings for tenant-bound groups", false, "Groups");
	I_END_COMPONENT;

	// reimplemented (imtauth::ITenantMembershipManager)
	virtual MembershipIds GetMembershipsByTenant(const QByteArray& tenantId) const override;
	virtual MembershipIds GetMembershipsByUser(const QByteArray& userId) const override;
	virtual ITenantMembershipUniquePtr GetMembership(const QByteArray& membershipId) const override;
	virtual ITenantMembershipUniquePtr FindMembership(const QByteArray& userId, const QByteArray& tenantId) const override;
	virtual QByteArray AddMembership(const QByteArray& userId, const QByteArray& tenantId) override;
	virtual bool RemoveMembership(const QByteArray& membershipId) override;
	virtual bool UpdateMembershipOrganizationPermissions(const QByteArray& membershipId, const QByteArrayList& permissions) override;
	virtual bool IsMember(const QByteArray& userId, const QByteArray& tenantId) const override;

private:
	bool IsOwnerMembership(const QByteArray& membershipId) const;
	bool CleanupTenantAssignedUserData(const QByteArray& userId, const QByteArray& tenantId);

	I_REF(imtbase::IObjectCollection, m_membershipCollectionCompPtr);
	I_FACT(imtauth::ITenantMembership, m_membershipFactoryCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantEntityBindingManager, m_bindingManagerCompPtr);
	I_ATTR(QByteArray, m_bindingEntityTypeAttrPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_ATTR(QByteArray, m_roleBindingEntityTypeAttrPtr);
	I_ATTR(QByteArray, m_groupBindingEntityTypeAttrPtr);
};


} // namespace imtauth
