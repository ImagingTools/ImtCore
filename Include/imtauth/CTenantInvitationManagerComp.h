// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/ITenantInvitationManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CTenantInvitationManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::ITenantInvitationManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantInvitationManagerComp);
		I_REGISTER_INTERFACE(imtauth::ITenantInvitationManager);
		I_ASSIGN(m_invitationCollectionCompPtr, "InvitationCollection", "Tenant invitation collection", true, "InvitationCollection");
		I_ASSIGN(m_invitationFactoryCompPtr, "InvitationFactory", "Tenant invitation factory", true, "TenantInvitationInfo");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager", true, "TenantMembershipManager");
	I_END_COMPONENT;

	virtual InvitationIds GetInvitationsByTenant(const QByteArray& tenantId, const Statuses& statuses = Statuses()) const override;
	virtual InvitationIds GetInvitationsByUser(const QByteArray& userId, const Statuses& statuses = Statuses()) const override;
	virtual ITenantInvitationUniquePtr GetInvitation(const QByteArray& invitationId) const override;
	virtual ITenantInvitationUniquePtr FindPendingInvitation(const QByteArray& userId, const QByteArray& tenantId) const override;
	virtual QByteArray CreateInvitation(const QByteArray& invitedByUserId, const QByteArray& userId, const QByteArray& tenantId, const QByteArray& roleId) override;
	virtual QByteArray AcceptInvitation(const QByteArray& invitationId, const QByteArray& userId) override;
	virtual bool RejectInvitation(const QByteArray& invitationId, const QByteArray& userId) override;
	virtual bool RevokeInvitation(const QByteArray& invitationId, const QByteArray& revokedByUserId) override;
	virtual bool ResendInvitation(const QByteArray& invitationId) override;
	virtual ITenantInvitation::TenantInvitationStatus GetEffectiveStatus(const ITenantInvitation& invitation) const override;

private:
	bool StatusMatches(const ITenantInvitation& invitation, const Statuses& statuses) const;
	bool StoreInvitation(const QByteArray& invitationId, ITenantInvitation& invitation);

private:
	I_REF(imtbase::IObjectCollection, m_invitationCollectionCompPtr);
	I_FACT(imtauth::ITenantInvitation, m_invitationFactoryCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
};


} // namespace imtauth

