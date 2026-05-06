// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/ITenantMembershipManager.h>
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
	I_END_COMPONENT;

	// reimplemented (imtauth::ITenantMembershipManager)
	virtual MembershipIds GetMembershipsByTenant(const QByteArray& tenantId) const override;
	virtual MembershipIds GetMembershipsByUser(const QByteArray& userId) const override;
	virtual const ITenantMembership* GetMembership(const QByteArray& membershipId) const override;
	virtual const ITenantMembership* FindMembership(const QByteArray& userId, const QByteArray& tenantId) const override;
	virtual QByteArray AddMembership(const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole role) override;
	virtual QByteArray InviteMembership(const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole role) override;
	virtual bool RemoveMembership(const QByteArray& membershipId) override;
	virtual bool UpdateMembershipRole(const QByteArray& membershipId, ITenantMembership::TenantMemberRole newRole) override;
	virtual bool AcceptMembershipInvitation(const QByteArray& membershipId) override;
	virtual bool RejectMembershipInvitation(const QByteArray& membershipId) override;
	virtual bool IsMember(const QByteArray& userId, const QByteArray& tenantId) const override;
	virtual bool HasMinimumRole(const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole minimumRole) const override;

private:
	I_REF(imtbase::IObjectCollection, m_membershipCollectionCompPtr);
	I_FACT(imtauth::ITenantMembership, m_membershipFactoryCompPtr);
};


} // namespace imtauth
