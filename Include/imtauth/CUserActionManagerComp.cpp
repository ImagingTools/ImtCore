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
			const iser::ISerializableSharedPtr actionDataPtr) const
{
	if (!m_userActionCollectionCompPtr.IsValid()){
		Q_ASSERT(false);
		return false;
	}

	if (!m_userRecentActionFactCompPtr.IsValid()){
		Q_ASSERT(false);
		return false;
	}

	imtauth::IUserActionInfoUniquePtr userRecentActionPtr = m_userRecentActionFactCompPtr.CreateInstance();

	userRecentActionPtr->SetUserInfo(userInfo);
	userRecentActionPtr->SetActionTypeInfo(actionTypeInfo);
	userRecentActionPtr->SetTargetInfo(targetInfo);
	userRecentActionPtr->SetTimestamp(QDateTime::currentDateTime());
	userRecentActionPtr->SetActionData(actionDataPtr);

	QByteArray retVal = m_userActionCollectionCompPtr->InsertNewObject(QByteArrayLiteral("UserAction"), actionTypeInfo.name, actionTypeInfo.description, userRecentActionPtr.GetPtr());
	return !retVal.isEmpty();
}


IUserActionInfoUniquePtr CUserActionManagerComp::GetUserAction(const QByteArray& actionId) const
{
	if (!m_userActionCollectionCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_userActionCollectionCompPtr->GetObjectData(actionId, dataPtr)){
		return nullptr;
	}

	const imtauth::IUserRecentAction* userActionPtr = dynamic_cast<const imtauth::IUserRecentAction*>(dataPtr.GetPtr());
	if (userActionPtr == nullptr){
		return nullptr;
	}

	imtauth::IUserActionInfoUniquePtr userActionInfoUniquePtr;
	userActionInfoUniquePtr.MoveCastedPtr(userActionPtr->CloneMe());

	return userActionInfoUniquePtr;
}


QByteArrayList CUserActionManagerComp::GetUserActionIds(
			int offset,
			int count,
			const iprm::IParamsSet* paramsSetPtr) const
{
	if (!m_userActionCollectionCompPtr.IsValid()){
		return QByteArrayList();
	}

	return m_userActionCollectionCompPtr->GetElementIds(offset, count, paramsSetPtr);
}


} // namespace imtauth

