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
	const imtauth::IUserInfo* userInfoPtr = (gqlContextPtr != nullptr) ? gqlContextPtr->GetUserInfo() : nullptr;

	// Only a resolved administrator is served unfiltered. A request with no context
	// or no resolved user is an unauthenticated caller and must not be treated like
	// one: it still gets a filter, and the empty user id in it denies everything.
	if (userInfoPtr != nullptr && userInfoPtr->IsAdmin()){
		return false;
	}

	iprm::CParamsSet* paramsSetPtr = dynamic_cast<iprm::CParamsSet*>(&paramsSet);
	if (paramsSetPtr == nullptr){
		return false;
	}

	istd::TDelPtr<imtauth::CUserGroupFilter> groupFilterPtr = new imtauth::CUserGroupFilter();
	if (gqlContextPtr != nullptr){
		groupFilterPtr->SetUserId(gqlContextPtr->GetUserId());
	}
	if (userInfoPtr != nullptr){
		groupFilterPtr->SetGroupIds(userInfoPtr->GetGroups());
	}
	paramsSetPtr->SetEditableParameter("GroupFilter", groupFilterPtr.PopPtr(), true);

	return true;
}


} // namespace imtservergql


