// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IDelegatedAccess.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ICrossOrgGrant.h>


namespace imtauth
{


class CDelegatedAccessResolverComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::IDelegatedAccess
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedAccessResolverComp);
		I_REGISTER_INTERFACE(imtauth::IDelegatedAccess);
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager", true, "TenantMembershipManager");
		I_ASSIGN(m_crossOrgGrantCompPtr, "CrossOrgGrantManager", "Cross-org grant manager", true, "CrossOrgGrantManager");
	I_END_COMPONENT;

	// reimplemented (imtauth::IDelegatedAccess)
	virtual DelegatedTenantList GetAccessibleTenants(
				const QByteArray& userId,
				const QByteArray& homeTenantId) const override;
	virtual bool HasDelegatedRole(
				const QByteArray& userId,
				const QByteArray& homeTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& roleId) const override;
	virtual QByteArrayList GetDelegatedRoles(
				const QByteArray& homeTenantId,
				const QByteArray& targetTenantId) const override;
	virtual bool IsDelegatedAccess(
				const QByteArray& userId,
				const QByteArray& homeTenantId,
				const QByteArray& targetTenantId) const override;

private:
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtauth::ICrossOrgGrant, m_crossOrgGrantCompPtr);
};


} // namespace imtauth
