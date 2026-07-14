// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ITenantEntityBinding.h>


namespace imtauth
{


class CTenantEntityBinding: virtual public ITenantEntityBinding, virtual public iser::ISerializable
{
public:
	CTenantEntityBinding();

	// reimplemented (ITenantEntityBinding)
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& tenantId) override;
	virtual QByteArray GetEntityType() const override;
	virtual void SetEntityType(const QByteArray& entityType) override;
	virtual QByteArray GetEntityId() const override;
	virtual void SetEntityId(const QByteArray& entityId) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;
	virtual QByteArray GetCreatedByUserId() const override;
	virtual void SetCreatedByUserId(const QByteArray& userId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_tenantId;
	QByteArray m_entityType;
	QByteArray m_entityId;
	QString m_createdAt;
	QByteArray m_createdByUserId;
};


typedef imtbase::TIdentifiableWrap<CTenantEntityBinding> CIdentifiableTenantEntityBinding;


} // namespace imtauth
