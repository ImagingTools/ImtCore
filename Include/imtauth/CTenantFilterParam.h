// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantFilterParam.h>


namespace imtauth
{


class CTenantFilterParam: virtual public ITenantFilterParam
{
public:
	CTenantFilterParam();

	// reimplemented (imtauth::ITenantFilterParam)
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& tenantId) override;
	virtual QByteArray GetOwnerId() const override;
	virtual void SetOwnerId(const QByteArray& ownerId) override;
	virtual TenantFilterMode GetFilterMode() const override;
	virtual void SetFilterMode(TenantFilterMode mode) override;
	virtual SharingScope GetSharingScope() const override;
	virtual void SetSharingScope(SharingScope scope) override;
	virtual IDocumentShareInfo::AccessLevel GetMinimumAccessLevel() const override;
	virtual void SetMinimumAccessLevel(IDocumentShareInfo::AccessLevel accessLevel) override;
	virtual QByteArrayList GetUserGroupIds() const override;
	virtual bool IsGroupRestrictionEnabled() const override;
	virtual void SetUserGroupIds(const QByteArrayList& groupIds, bool restrictionEnabled) override;
	virtual QByteArrayList GetShareOwnerTenantIds() const override;
	virtual bool IsShareOwnerRestrictionEnabled() const override;
	virtual void SetShareOwnerTenantIds(const QByteArrayList& tenantIds, bool restrictionEnabled) override;
	virtual int GetSharesRevision() const override;
	virtual void SetSharesRevision(int revision) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_tenantId;
	QByteArray m_ownerId;
	TenantFilterMode m_filterMode;
	SharingScope m_sharingScope;
	IDocumentShareInfo::AccessLevel m_minimumAccessLevel;
	QByteArrayList m_userGroupIds;
	bool m_isGroupRestrictionEnabled;
	QByteArrayList m_shareOwnerTenantIds;
	bool m_isShareOwnerRestrictionEnabled;
	int m_sharesRevision;
};


} // namespace imtauth
