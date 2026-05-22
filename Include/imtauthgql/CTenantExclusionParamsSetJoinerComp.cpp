// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantExclusionParamsSetJoinerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CTenantFilterParam.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


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
	if (paramsSetPtr == nullptr){
		return false;
	}

	imtauth::CTenantFilterParam* tenantExclusionFilterPtr = new imtauth::CTenantFilterParam();
	tenantExclusionFilterPtr->SetTenantId(tenantId);
	paramsSetPtr->SetEditableParameter("TenantExclusionFilter", tenantExclusionFilterPtr, true);

	return true;
}


} // namespace imtauthgql


