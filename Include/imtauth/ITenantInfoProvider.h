// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtauth
{


class ITenantInfoProvider: virtual public istd::IChangeable
{
public:
	/**
		Get list of all available tenants.
	*/
	virtual const imtbase::ICollectionInfo& GetTenantList() const = 0;

	/**
		Get information for tenant by ID.
	*/
	virtual imtauth::ITenantInfoSharedPtr GetTenant(const QByteArray& tenantId, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
};


} // namespace imtauth


