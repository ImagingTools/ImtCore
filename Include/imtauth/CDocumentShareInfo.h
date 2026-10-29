// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IDocumentShareInfo.h>


namespace imtauth
{


class CDocumentShareInfo: virtual public IDocumentShareInfo
{
public:
	CDocumentShareInfo();

	// reimplemented (imtauth::IDocumentShareInfo)
	virtual QByteArray GetShareId() const override;
	virtual void SetShareId(const QByteArray& shareId) override;
	virtual QByteArray GetCollectionId() const override;
	virtual void SetCollectionId(const QByteArray& collectionId) override;
	virtual QByteArray GetEntityId() const override;
	virtual void SetEntityId(const QByteArray& entityId) override;
	virtual QString GetEntityName() const override;
	virtual void SetEntityName(const QString& entityName) override;
	virtual QByteArray GetOwnerTenantId() const override;
	virtual void SetOwnerTenantId(const QByteArray& tenantId) override;
	virtual QString GetOwnerTenantName() const override;
	virtual void SetOwnerTenantName(const QString& tenantName) override;
	virtual QByteArray GetTargetTenantId() const override;
	virtual void SetTargetTenantId(const QByteArray& tenantId) override;
	virtual QString GetTargetTenantName() const override;
	virtual void SetTargetTenantName(const QString& tenantName) override;
	virtual QByteArrayList GetTargetGroupIds() const override;
	virtual void SetTargetGroupIds(const QByteArrayList& groupIds) override;
	virtual AccessLevel GetAccessLevel() const override;
	virtual void SetAccessLevel(AccessLevel accessLevel) override;
	virtual ShareStatus GetStatus() const override;
	virtual void SetStatus(ShareStatus status) override;
	virtual QDateTime GetValidUntil() const override;
	virtual void SetValidUntil(const QDateTime& validUntil) override;
	virtual QString GetMessage() const override;
	virtual void SetMessage(const QString& message) override;
	virtual QByteArray GetCreatedByUserId() const override;
	virtual void SetCreatedByUserId(const QByteArray& userId) override;
	virtual QDateTime GetCreatedAt() const override;
	virtual void SetCreatedAt(const QDateTime& createdAt) override;
	virtual QDateTime GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QDateTime& updatedAt) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_shareId;
	QByteArray m_collectionId;
	QByteArray m_entityId;
	QString m_entityName;
	QByteArray m_ownerTenantId;
	QString m_ownerTenantName;
	QByteArray m_targetTenantId;
	QString m_targetTenantName;
	QByteArrayList m_targetGroupIds;
	AccessLevel m_accessLevel;
	ShareStatus m_status;
	QDateTime m_validUntil;
	QString m_message;
	QByteArray m_createdByUserId;
	QDateTime m_createdAt;
	QDateTime m_updatedAt;
};


typedef imtbase::TIdentifiableWrap<CDocumentShareInfo> CIdentifiableDocumentShareInfo;


} // namespace imtauth


