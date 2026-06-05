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

	virtual TenantConnectionInfo GetConnectionInfo() const override;
	virtual void SetConnectionInfo(const TenantConnectionInfo& connectionInfo) override;
	virtual QByteArray GetConnectionId() const override;

	virtual bool Serialize(iser::IArchive& archive) override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	TenantConnectionInfo m_info;
};


typedef imtbase::TIdentifiableWrap<CTenantConnection> CIdentifiableTenantConnection;


} // namespace imtauth
