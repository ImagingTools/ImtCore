// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IDelegatedAccess.h>
#include <imtauth/ICrossOrgGrant.h>
#include <imtauth/ITenantMembershipManager.h>


namespace imtauth
{


/**
	Default implementation of IDelegatedAccess.
	Composes ITenantMembershipManager (direct memberships) and ICrossOrgGrant
	(delegated grants) to resolve organization visibility, delegated context
	switching, and delegated permission checks.

	\ingroup Permission
*/
class CDelegatedAccessResolverComp:
			public icomp::CComponentBase,
			virtual public imtauth::IDelegatedAccess
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedAccessResolverComp);
		I_REGISTER_INTERFACE(imtauth::IDelegatedAccess);
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager", true, "MembershipManager");
		I_ASSIGN(m_crossOrgGrantCompPtr, "CrossOrgGrant", "Cross-org grant manager", true, "CrossOrgGrant");
	I_END_COMPONENT;

	// reimplemented (imtauth::IDelegatedAccess)
	virtual TenantAccessList GetAccessibleTenants(
				const QByteArray& userId,
				const QByteArray& currentTenantId) const override;
	virtual TenantAccessInfo ResolveTenantAccess(
				const QByteArray& userId,
				const QByteArray& currentTenantId,
				const QByteArray& targetTenantId) const override;
	virtual bool IsAccessAllowed(
				const QByteArray& userId,
				const QByteArray& currentTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& requiredRoleId) const override;

private:
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtauth::ICrossOrgGrant, m_crossOrgGrantCompPtr);
};


} // namespace imtauth
