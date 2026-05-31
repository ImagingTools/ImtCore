// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CUserGroupParamsSetJoinerComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CUserGroupFilter.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <imtauth/IUserInfo.h>


namespace imtservergql
{


// public methods

// reimplemented (imtservergql::IParamsSetJoiner)

bool CUserGroupParamsSetJoinerComp::JoinParamsSet(
			const imtgql::CGqlRequest& gqlRequest,
			iprm::IParamsSet& paramsSet) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr || userInfoPtr->IsAdmin()){
		return false;
	}

	iprm::CParamsSet* paramsSetPtr = dynamic_cast<iprm::CParamsSet*>(&paramsSet);
	if (paramsSetPtr == nullptr){
		return false;
	}

	istd::TDelPtr<imtauth::CUserGroupFilter> groupFilterPtr = new imtauth::CUserGroupFilter();
	groupFilterPtr->SetUserId(gqlContextPtr->GetUserId());
	groupFilterPtr->SetGroupIds(userInfoPtr->GetGroups());
	paramsSetPtr->SetEditableParameter("GroupFilter", groupFilterPtr.PopPtr(), true);

	return true;
}


} // namespace imtservergql


