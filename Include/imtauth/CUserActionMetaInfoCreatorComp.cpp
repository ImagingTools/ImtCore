// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserActionMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtauth/IUserRecentAction.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CUserActionMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
		if (!m_objectTypeIdsAttrPtr.IsValid()){
		return false;
	}

	bool isSupported = false;
	int count = m_objectTypeIdsAttrPtr.GetCount();
	for (int i = 0; i < count; i++){
		if (m_objectTypeIdsAttrPtr[i] == typeId){
			isSupported = true;
			break;
		}
	}

	if (!isSupported){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const imtauth::IUserRecentAction* userRecentActionPtr = dynamic_cast<const imtauth::IUserRecentAction*>(dataPtr);
	if (userRecentActionPtr == nullptr){
		return false;
	}

	imtauth::IUserRecentAction::TargetInfo targetInfo = userRecentActionPtr->GetTargetInfo();
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_ID, targetInfo.id);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_ID, targetInfo.typeId);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_NAME, targetInfo.typeName);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_NAME, targetInfo.name);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_SOURCE, targetInfo.source);

	imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo = userRecentActionPtr->GetActionTypeInfo();
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE_ID, actionTypeInfo.id);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE_NAME, actionTypeInfo.name);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE_DESCRIPTION, actionTypeInfo.description);

	imtauth::IUserRecentAction::UserInfo userInfo = userRecentActionPtr->GetUserInfo();
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_USER_ID, userInfo.id);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_USER_NAME, userInfo.name);

	return true;
}


// public methods of embedded class MetaInfo

QString CUserActionMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace imtauth


