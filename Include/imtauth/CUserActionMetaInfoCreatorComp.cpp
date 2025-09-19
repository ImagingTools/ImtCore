#include <imtauth/CUserActionMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
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
	if (typeId != *m_objectTypeIdAttrPtr){
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

	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_USER_ID, userRecentActionPtr->GetUserId());

	imtauth::IUserRecentAction::TargetInfo targetInfo = userRecentActionPtr->GetTargetInfo();
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_ID, targetInfo.id);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_ID, targetInfo.typeId);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_NAME, targetInfo.name);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_SOURCE, targetInfo.source);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE, userRecentActionPtr->GetActionType());

	return true;
}


// public methods of embedded class MetaInfo

QString CUserActionMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace imtauth


