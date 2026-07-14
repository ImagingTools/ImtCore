// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ITenantMembership.h>


namespace imtauth
{


class CTenantMembership: virtual public ITenantMembership, virtual public iser::ISerializable
{
public:
	CTenantMembership();

	// reimplemented (ITenantMembership)
	virtual QByteArray GetMembershipId() const override;
	virtual void SetMembershipId(const QByteArray& membershipId) override;
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& tenantId) override;
	virtual QString GetJoinedAt() const override;
	virtual void SetJoinedAt(const QString& joinedAt) override;
	virtual bool IsActive() const override;
	virtual void SetActive(bool isActive) override;
	virtual QString GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QString& updatedAt) override;

	virtual QByteArrayList GetOrganizationPermissions() const override;
	virtual void SetOrganizationPermissions(const QByteArrayList& permissions) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_membershipId;
	QByteArray m_userId;
	QByteArray m_tenantId;
	QString m_joinedAt;
	QString m_updatedAt;
	bool m_isActive;
	QByteArrayList m_organizationPermissions;
};


typedef imtbase::TIdentifiableWrap<CTenantMembership> CIdentifiableTenantMembership;


} // namespace imtauth
