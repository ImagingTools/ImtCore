#include <imtgql/CUserSettingsControllerComp.h>


// ACF includes
#include <istd/TOptDelPtr.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CUserSettingsControllerComp::CreateRepresentationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	if (!m_userSettingsRepresentationControllerCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_userSettingsCollectionCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_userSettingsInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	// Get user-ID from GqlContext
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	QByteArray userId;
	if (gqlContextPtr != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userId = userInfoPtr->GetId();
		}
	}

	istd::TDelPtr<imtbase::CTreeItemModel> userSettingsRepresentation(new imtbase::CTreeItemModel);
	imtbase::CTreeItemModel* dataModelPtr = userSettingsRepresentation->AddTreeModel("data");

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
			return nullptr;
		}

		userSettingsPtr->SetUserId(userId);
	}

	iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
	Q_ASSERT(paramSetPtr != nullptr);
	if (paramSetPtr == nullptr){
		return nullptr;
	}

	bool result = m_userSettingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(*paramSetPtr, *dataModelPtr);
	if (!result){
		return nullptr;
	}

	return userSettingsRepresentation.PopPtr();
}


bool CUserSettingsControllerComp::UpdateModelFromRepresentation(
			const imtgql::CGqlRequest& request,
			imtbase::CTreeItemModel* representationPtr) const
{
	if (!m_userSettingsRepresentationControllerCompPtr.IsValid()){
		SendErrorMessage(0, QString("Internal error. m_userSettingsRepresentationControllerCompPtr is invalid."));

		return false;
	}

	if (!m_userSettingsCollectionCompPtr.IsValid()){
		SendErrorMessage(0, QString("Internal error. m_userSettingsCollectionCompPtr is invalid."));

		return false;
	}

	if (!m_userSettingsInfoFactCompPtr.IsValid()){
		SendErrorMessage(0, QString("Internal error. m_userSettingsInfoFactCompPtr is invalid."));

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

	istd::TDelPtr<imtauth::IUserSettings> userSettingsPtr = m_userSettingsInfoFactCompPtr.CreateInstance();
	Q_ASSERT(userSettingsPtr.IsValid());
	if (!userSettingsPtr.IsValid()){
		return false;
	}

	userSettingsPtr->SetUserId(userId);

	iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
	Q_ASSERT(paramSetPtr != nullptr);
	if (paramSetPtr == nullptr){
		SendErrorMessage(0, QString("Unable to get setting from user %1.").arg(qPrintable(userId)));

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


} // namespace imtgql


