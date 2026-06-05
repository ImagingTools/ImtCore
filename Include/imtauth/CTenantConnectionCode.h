// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ITenantConnectionCodeData.h>


namespace imtauth
{


class CTenantConnectionCode: virtual public ITenantConnectionCodeData, virtual public iser::ISerializable
{
public:
	CTenantConnectionCode();

	virtual TenantConnectionCodeInfo GetCodeInfo() const override;
	virtual void SetCodeInfo(const TenantConnectionCodeInfo& codeInfo) override;
	virtual QByteArray GetTenantId() const override;

	virtual bool Serialize(iser::IArchive& archive) override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	TenantConnectionCodeInfo m_info;
};


typedef imtbase::TIdentifiableWrap<CTenantConnectionCode> CIdentifiableTenantConnectionCode;


} // namespace imtauth
