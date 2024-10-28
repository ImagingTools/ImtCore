#include <imtservergql/CUserSettingsControllerComp.h>


// ACF includes
#include <istd/TOptDelPtr.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <imod/TModelWrap.h>


namespace imtservergql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CUserSettingsControllerComp::CreateRepresentationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_userSettingsRepresentationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsRepresentationControllerCompPtr' was not set", "CUserSettingsControllerComp");
		return nullptr;
	}

	if (!m_userSettingsCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsCollectionCompPtr' was not set", "CUserSettingsControllerComp");
		return nullptr;
	}

	if (!m_userSettingsInfoFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsInfoFactCompPtr' was not set", "CUserSettingsControllerComp");
		return nullptr;
	}

	QByteArray languageId;

	// Get user-ID from GqlContext
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	QByteArray userId;
	if (gqlContextPtr != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userId = userInfoPtr->GetId();
		}

		languageId = gqlContextPtr->GetLanguageId();
	}

	istd::TDelPtr<iprm::CParamsSet> paramsPtr = new imod::TModelWrap<iprm::CParamsSet>();

	iprm::CIdParam* languageIdParamPtr = new iprm::CIdParam();
	languageIdParamPtr->SetId(languageId);
	paramsPtr->SetEditableParameter("LanguageParam", languageIdParamPtr, true);

	istd::TOptDelPtr<imtauth::IUserSettings> userSettingsPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!userId.isEmpty()){
		if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
			userSettingsPtr.SetPtr(dynamic_cast<imtauth::IUserSettings*>(dataPtr.GetPtr()), false);
		}
	}

	if (!userSettingsPtr.IsValid()){
		userSettingsPtr.SetPtr(m_userSettingsInfoFactCompPtr.CreateInstance(), true);
		Q_ASSERT(userSettingsPtr.IsValid());
		if (!userSettingsPtr.IsValid()){
			errorMessage = QString("Unable to create representation for user settings. Error: User settings is invalid.");
			SendErrorMessage(0, errorMessage, "CUserSettingsControllerComp");

			return nullptr;
		}

		userSettingsPtr->SetUserId(userId);
	}

	iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
	Q_ASSERT(paramSetPtr != nullptr);
	if (paramSetPtr == nullptr){
		errorMessage = QString("Unable to create representation for user settings. Error: Params set from user settings is invalid.");
		SendErrorMessage(0, errorMessage, "CUserSettingsControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> userSettingsRepresentation(new imtbase::CTreeItemModel);
	imtbase::CTreeItemModel* dataModelPtr = userSettingsRepresentation->AddTreeModel("data");

	bool result = m_userSettingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(*paramSetPtr, *dataModelPtr, paramsPtr.GetPtr());
	if (!result){
		errorMessage = QString("Unable to create representation for user settings.");
		SendErrorMessage(0, errorMessage, "CUserSettingsControllerComp");

		return nullptr;
	}

	return userSettingsRepresentation.PopPtr();
}


bool CUserSettingsControllerComp::UpdateModelFromRepresentation(
			const imtgql::CGqlRequest& request,
			imtbase::CTreeItemModel* representationPtr) const
{
	if (!m_userSettingsRepresentationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsRepresentationControllerCompPtr' was not set", "CUserSettingsControllerComp");
		return false;
	}

	if (!m_userSettingsCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsCollectionCompPtr' was not set", "CUserSettingsControllerComp");
		return false;
	}

	if (!m_userSettingsInfoFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsInfoFactCompPtr' was not set", "CUserSettingsControllerComp");
		return false;
	}

	// Get user-ID from GqlContext
	const imtgql::IGqlContext* gqlContextPtr = request.GetRequestContext();
	QByteArray userId;
	if (gqlContextPtr != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userId = userInfoPtr->GetId();
		}
	}

	if (userId.isEmpty()){
		SendErrorMessage(0, QString("Unable to update model from representation. User-ID is empty!"));

		return false;
	}

	istd::TOptDelPtr<imtauth::IUserSettings> userSettingsPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userSettingsPtr.SetPtr(dynamic_cast<imtauth::IUserSettings*>(dataPtr.GetPtr()), false);
	}

	if (!userSettingsPtr.IsValid()){
		userSettingsPtr.SetPtr(m_userSettingsInfoFactCompPtr.CreateInstance(), true);
		userSettingsPtr->SetUserId(userId);
	}

	iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
	Q_ASSERT(paramSetPtr != nullptr);
	if (paramSetPtr == nullptr){
		SendErrorMessage(0, QString("Unable to get setting from user: '%1'.").arg(qPrintable(userId)));

		return false;
	}

	bool retVal = m_userSettingsRepresentationControllerCompPtr->GetDataModelFromRepresentation(*representationPtr, *paramSetPtr);
	if (retVal){
		imtbase::ICollectionInfo::Ids collectionIds = m_userSettingsCollectionCompPtr->GetElementIds();
		if (collectionIds.contains(userId)){
			m_userSettingsCollectionCompPtr->SetObjectData(userId, *userSettingsPtr);
		}
		else{
			m_userSettingsCollectionCompPtr->InsertNewObject("", "", "", userSettingsPtr.GetPtr(), userId);
		}
	}

	return retVal;
}


} // namespace imtservergql


