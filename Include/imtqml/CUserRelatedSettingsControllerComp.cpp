#include <imtqml/CUserRelatedSettingsControllerComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtauth/CUserSettings.h>


namespace imtqml
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp

bool CUserRelatedSettingsControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	if (paramsPtr != nullptr){
		const imtgql::IGqlContext* gqlContextPtr = dynamic_cast<const imtgql::IGqlContext*>(paramsPtr->GetParameter("GqlContext"));
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

						return m_userSettingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(*settingsParamSetPtr, representation);
					}
				}
				else{
					return m_userSettingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(dataModel, representation);
				}
			}
		}
	}

	return false;
}


bool CUserRelatedSettingsControllerComp::GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	if (!m_userSettingsInfoFactCompPtr.IsValid()){
		return false;
	}

	QByteArray userId;

	istd::TDelPtr<imtauth::IUserSettings> userSettingsPtr = m_userSettingsInfoFactCompPtr.CreateInstance();
	userSettingsPtr->SetUserId(userId);

	iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
	Q_ASSERT(paramSetPtr != nullptr);

	if (paramSetPtr == nullptr){
		return false;
	}

	bool result = m_userSettingsRepresentationControllerCompPtr->GetDataModelFromRepresentation(representation, *paramSetPtr);
	if (result){
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


