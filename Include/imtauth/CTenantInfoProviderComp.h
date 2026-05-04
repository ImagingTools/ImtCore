// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/ITenantInfoProvider.h>


namespace imtauth
{


class CTenantInfoProviderComp: public icomp::CComponentBase, virtual public ITenantInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantInfoProviderComp);
		I_REGISTER_INTERFACE(ITenantInfoProvider);
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection", true, "TenantCollection");
	I_END_COMPONENT;

	// reimplemented (ITenantInfoProvider)
	virtual const imtbase::ICollectionInfo& GetTenantList() const override;
	virtual imtauth::ITenantInfoSharedPtr GetTenant(const QByteArray& tenantId, const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
};


} // namespace imtauth


