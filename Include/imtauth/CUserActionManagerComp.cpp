// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserActionManagerComp.h>


// ImtCore includes
#include <imtauth/CUserRecentAction.h>


namespace imtauth
{


// protected methods

// reimplemented (imtauth::IUserActionManager)

bool CUserActionManagerComp::CreateUserAction(
			imtauth::IUserRecentAction::UserInfo userInfo,
			imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo,
			imtauth::IUserRecentAction::TargetInfo targetInfo,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!m_userActionCollectionCompPtr.IsValid()){
		return false;
	}

	istd::TDelPtr<imtauth::CIdentifiableUserRecentAction> userRecentActionPtr;
	userRecentActionPtr.SetPtr(new imtauth::CIdentifiableUserRecentAction);
	if (!userRecentActionPtr.IsValid()){
		return false;
	}

	userRecentActionPtr->SetUserInfo(userInfo);
	userRecentActionPtr->SetActionTypeInfo(actionTypeInfo);
	userRecentActionPtr->SetTargetInfo(targetInfo);
	userRecentActionPtr->SetTimestamp(QDateTime::currentDateTime());

	QByteArray retVal = m_userActionCollectionCompPtr->InsertNewObject(QByteArrayLiteral("UserAction"), actionTypeInfo.name, actionTypeInfo.description, userRecentActionPtr.GetPtr());
	return !retVal.isEmpty();
}


} // namespace imtauth
