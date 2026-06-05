// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ITenantConnectionCodeInfo.h>


namespace imtauth
{


class CTenantConnectionCode: virtual public ITenantConnectionCodeInfo, virtual public iser::ISerializable
{
public:
CTenantConnectionCode();

virtual QByteArray GetTenantId() const override;
virtual void SetTenantId(const QByteArray& tenantId) override;

virtual QString GetConnectionCode() const override;
virtual void SetConnectionCode(const QString& connectionCode) override;

virtual bool GetAllowConnectionsByCode() const override;
virtual void SetAllowConnectionsByCode(bool allow) override;

virtual QString GetCreatedAt() const override;
virtual void SetCreatedAt(const QString& createdAt) override;

virtual bool Serialize(iser::IArchive& archive) override;
virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
QByteArray m_tenantId;
QString m_connectionCode;
bool m_allowConnectionsByCode;
QString m_createdAt;
};


typedef imtbase::TIdentifiableWrap<CTenantConnectionCode> CIdentifiableTenantConnectionCode;


} // namespace imtauth
