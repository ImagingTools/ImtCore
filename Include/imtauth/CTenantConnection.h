// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ITenantConnectionInfo.h>


namespace imtauth
{


class CTenantConnection: virtual public ITenantConnectionInfo, virtual public iser::ISerializable
{
public:
	CTenantConnection();

	// reimplemented (ITenantConnectionInfo)
	virtual QByteArray GetConnectionId() const override;
	virtual void SetConnectionId(const QByteArray& connectionId) override;
	virtual QByteArray GetTenantAId() const override;
	virtual void SetTenantAId(const QByteArray& tenantAId) override;
	virtual QByteArray GetTenantBId() const override;
	virtual void SetTenantBId(const QByteArray& tenantBId) override;
	virtual ConnectionStatus GetStatus() const override;
	virtual void SetStatus(ConnectionStatus status) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;
	virtual QString GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QString& updatedAt) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_connectionId;
	QByteArray m_tenantAId;
	QByteArray m_tenantBId;
	ConnectionStatus m_status;
	QString m_createdAt;
	QString m_updatedAt;
};


typedef imtbase::TIdentifiableWrap<CTenantConnection> CIdentifiableTenantConnection;


} // namespace imtauth
