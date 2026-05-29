// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ITenantConnectionRequestData.h>


namespace imtauth
{


class CTenantConnectionRequest: virtual public ITenantConnectionRequestData, virtual public iser::ISerializable
{
public:
	CTenantConnectionRequest();

	virtual TenantConnectionRequestInfo GetRequestInfo() const override;
	virtual void SetRequestInfo(const TenantConnectionRequestInfo& requestInfo) override;
	virtual QByteArray GetRequestId() const override;

	virtual bool Serialize(iser::IArchive& archive) override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	TenantConnectionRequestInfo m_info;
};


typedef imtbase::TIdentifiableWrap<CTenantConnectionRequest> CIdentifiableTenantConnectionRequest;


} // namespace imtauth
