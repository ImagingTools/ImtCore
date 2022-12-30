#include <imtqml/CUserRelatedSettingsControllerComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtauth/CUserSettings.h>


namespace imtqml
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CUserRelatedSettingsControllerComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_userSettingsCollectionCompPtr.IsValid()){
		const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetGqlContext();
		if (gqlContextPtr != nullptr){
			imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
			if (userInfoPtr != nullptr){
				QByteArray userId = userInfoPtr->GetUserId();
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
					const imtauth::IUserSettings* userSettingsPtr = dynamic_cast<const imtauth::IUserSettings*>(dataPtr.GetPtr());
					if (userSettingsPtr != nullptr){
						iprm::IParamsSet* settingsParamSetPtr = userSettingsPtr->GetSettings();
						Q_ASSERT(settingsParamSetPtr != nullptr);

						istd::TDelPtr<imtbase::CTreeItemModel> representationModelPtr = new imtbase::CTreeItemModel();
						if (m_userSettingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(*settingsParamSetPtr, *representationModelPtr)){
							return representationModelPtr.PopPtr();
						}
					}
				}
			}
		}
	}

	return nullptr;
}


bool CUserRelatedSettingsControllerComp::UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const
{
	if (!m_userSettingsInfoFactCompPtr.IsValid()){
		return false;
	}

	// Get user-ID from client request
	const imtgql::IGqlContext* gqlContextPtr = request.GetGqlContext();
	QByteArray userId;
	if (gqlContextPtr != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userId = userInfoPtr->GetUserId();
		}
	}

	QByteArray parameterId = *m_paramIdAttrPtr;
	if (representationPtr != nullptr){
		istd::TDelPtr<imtauth::IUserSettings> userSettingsPtr = m_userSettingsInfoFactCompPtr.CreateInstance();
		userSettingsPtr->SetUserId(userId);

		iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
		Q_ASSERT(paramSetPtr != nullptr);

		m_userSettingsRepresentationControllerCompPtr->GetDataModelFromRepresentation(*representationPtr, *paramSetPtr);

		imtbase::ICollectionInfo::Ids collectionIds = m_userSettingsCollectionCompPtr->GetElementIds();
		if (collectionIds.contains(userId)){
			m_userSettingsCollectionCompPtr->SetObjectData(userId, *userSettingsPtr);
		}
		else{
			m_userSettingsCollectionCompPtr->InsertNewObject("", "", "", userSettingsPtr.GetPtr(), userId);
		}

		return true;
	}

	return false;
}


} // namespace imtqml


