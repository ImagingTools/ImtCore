// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantExclusionParamsSetJoinerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CTenantFilterParam.h>


namespace imtauthgql
{


// reimplemented (imtservergql::IParamsSetJoiner)

bool CTenantExclusionParamsSetJoinerComp::JoinParamsSet(
		const imtgql::CGqlRequest& gqlRequest,
		iprm::IParamsSet& paramsSet) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return false;
	}

	QByteArray tenantId = gqlContextPtr->GetTenantId();
	if (tenantId.isEmpty()){
		return false;
	}

	iprm::CParamsSet* paramsSetPtr = dynamic_cast<iprm::CParamsSet*>(&paramsSet);
	if (paramsSetPtr != nullptr){
		imtauth::CTenantFilterParam* tenantFilterPtr = new imtauth::CTenantFilterParam();
		tenantFilterPtr->SetTenantId(tenantId);
		tenantFilterPtr->SetFilterMode(imtauth::TFM_EXCLUDE);

		paramsSetPtr->SetEditableParameter("TenantFilter", tenantFilterPtr, true);

		return true;
	}

	return false;
}


} // namespace imtauthgql
