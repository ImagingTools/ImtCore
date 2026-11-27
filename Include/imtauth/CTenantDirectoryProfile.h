// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ITenantDirectoryProfile.h>


namespace imtauth
{


class CTenantDirectoryProfile: virtual public ITenantDirectoryProfile
{
public:
	CTenantDirectoryProfile();

	// reimplemented (imtauth::ITenantDirectoryProfile)
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& value) override;
	virtual QString GetTenantName() const override;
	virtual void SetTenantName(const QString& value) override;
	virtual bool IsTenantActive() const override;
	virtual void SetTenantActive(bool value) override;
	virtual bool IsListed() const override;
	virtual void SetListed(bool value) override;
	virtual bool IsAcceptingInvitationsFromUnknown() const override;
	virtual void SetAcceptingInvitationsFromUnknown(bool value) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& value) override;
	virtual QByteArrayList GetOfferedRoleIds() const override;
	virtual void SetOfferedRoleIds(const QByteArrayList& value) override;
	virtual QString GetIndustry() const override;
	virtual void SetIndustry(const QString& value) override;
	virtual QString GetCountry() const override;
	virtual void SetCountry(const QString& value) override;
	virtual QString GetWebsite() const override;
	virtual void SetWebsite(const QString& value) override;
	virtual QDateTime GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QDateTime& value) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_tenantId;
	QString m_tenantName;
	bool m_tenantActive;
	bool m_listed;
	bool m_acceptingInvitationsFromUnknown;
	QString m_description;
	QByteArrayList m_offeredRoleIds;
	QString m_industry;
	QString m_country;
	QString m_website;
	QDateTime m_updatedAt;
};


typedef imtbase::TIdentifiableWrap<CTenantDirectoryProfile> CIdentifiableTenantDirectoryProfile;


} // namespace imtauth


