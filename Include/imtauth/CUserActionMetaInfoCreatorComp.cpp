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


	imtauth::IUserRecentAction::TargetInfo targetInfo = userRecentActionPtr->GetTargetInfo();
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_ID, targetInfo.id);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_ID, targetInfo.typeId);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_NAME, targetInfo.name);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_SOURCE, targetInfo.source);
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE, userRecentActionPtr->GetActionType());

	// Username
	metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_USER_NAME, userRecentActionPtr->GetUserId());

	if (m_userCollectionCompPtr.IsValid()){
		QByteArray userId = userRecentActionPtr->GetUserId();

		imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
		fieldFilter.fieldId = "Id";
		fieldFilter.filterValue = userId;

		imtbase::IComplexCollectionFilter::GroupFilter groupFilter;
		groupFilter.fieldFilters << fieldFilter;
	
		imtbase::CComplexCollectionFilter complexFilter;
		complexFilter.SetFieldsFilter(groupFilter);

		iprm::CParamsSet filterParam;
		filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

		QByteArrayList elementIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (!elementIds.isEmpty()){
			metaInfoPtr->SetMetaInfo(imtauth::IUserRecentAction::MIT_USER_ID, elementIds[0]);
		}
	}

	return true;
}


// public methods of embedded class MetaInfo

QString CUserActionMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace imtauth


