// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ITenantInfo.h>


namespace imtauth
{


class CTenantInfo: virtual public ITenantInfo, virtual public iser::ISerializable
{
public:
	CTenantInfo();

	// reimplemented (ITenantInfo)
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& tenantId) override;
	virtual QString GetTenantName() const override;
	virtual void SetTenantName(const QString& name) override;
	virtual QString GetTenantDescription() const override;
	virtual void SetTenantDescription(const QString& description) override;
	virtual QByteArray GetOwnerId() const override;
	virtual void SetOwnerId(const QByteArray& ownerId) override;
	virtual QByteArray GetCreatorId() const override;
	virtual void SetCreatorId(const QByteArray& creatorId) override;
	virtual bool IsActive() const override;
	virtual void SetActive(bool isActive) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;
	virtual QString GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QString& updatedAt) override;
	virtual QByteArrayList GetRelationshipIds() const override;
	virtual void SetRelationshipIds(const QByteArrayList& relationshipIds) override;
	virtual void AddRelationshipId(const QByteArray& relationshipId) override;
	virtual bool RemoveRelationshipId(const QByteArray& relationshipId) override;
	virtual QByteArrayList GetTenantPermissions() const override;
	virtual void SetTenantPermissions(const QByteArrayList& permissions) override;

	virtual QByteArray GetParentTenantId() const override;
	virtual void SetParentTenantId(const QByteArray& parentTenantId) override;

	// Hierarchy fields (internal, for DB/serialization)
	int GetDepth() const;
	void SetDepth(int depth);
	QString GetMaterializedPath() const;
	void SetMaterializedPath(const QString& path);
	bool IsSystemTenant() const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_tenantId;
	QString m_name;
	QString m_description;
	QByteArray m_ownerId;
	QByteArray m_creatorId;
	bool m_isActive;
	QString m_createdAt;
	QString m_updatedAt;
	QByteArrayList m_relationshipIds;
	QByteArrayList m_tenantPermissions;
	QByteArray m_parentTenantId;
	int m_depth;
	QString m_materializedPath;
};


typedef imtbase::TIdentifiableWrap<CTenantInfo> CIdentifiableTenantInfo;


} // namespace imtauth

