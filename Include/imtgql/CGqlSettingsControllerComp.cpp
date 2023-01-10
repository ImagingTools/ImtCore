#include <imtgql/CGqlSettingsControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <istd/TOptDelPtr.h>

// ImtCore includes
#include <imtauth/CUserSettings.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CGqlSettingsControllerComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_userSettingsRepresentationControllerCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_userSettingsCollectionCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_dataModelCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_representationControllerCompPtr.IsValid()){
		return nullptr;
	}

	// Get user-ID from GqlContext
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetGqlContext();
	QByteArray userId;
	if (gqlContextPtr != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userId = userInfoPtr->GetUserId();
		}
	}

	// Get representation for user related settings
	imtbase::CTreeItemModel userSettingsRepresentation;
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

	bool result = m_userSettingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(*paramSetPtr, userSettingsRepresentation);
	if (!result){
		return nullptr;
	}

	// Get representation for settings stored on the server
	imtbase::CTreeItemModel serverSettingsRepresentation;
	m_representationControllerCompPtr->GetRepresentationFromDataModel(*m_dataModelCompPtr, serverSettingsRepresentation);

	// Concatenation models
	istd::TDelPtr<imtbase::CTreeItemModel> representationModelPtr(new imtbase::CTreeItemModel);
	imtbase::CTreeItemModel* dataModelPtr = representationModelPtr->AddTreeModel("data");

	int index = dataModelPtr->InsertNewItem();
	dataModelPtr->CopyItemDataFromModel(index, &userSettingsRepresentation);

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->CopyItemDataFromModel(index, &serverSettingsRepresentation);

	return representationModelPtr.PopPtr();
}


bool CGqlSettingsControllerComp::UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const
{
	if (!m_userSettingsRepresentationControllerCompPtr.IsValid()){
		return false;
	}

	if (!m_userSettingsCollectionCompPtr.IsValid()){
		return false;
	}

	if (!m_dataModelCompPtr.IsValid()){
		return false;
	}

	if (!m_representationControllerCompPtr.IsValid()){
		return false;
	}

	// Get user-ID from GqlContext
	const imtgql::IGqlContext* gqlContextPtr = request.GetGqlContext();
	QByteArray userId;
	if (gqlContextPtr != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userId = userInfoPtr->GetUserId();
		}
	}

	if (m_representationControllerCompPtr.IsValid() && m_dataModelCompPtr.IsValid()){
		const QList<imtgql::CGqlObject>* inputParams = request.GetParams();
		if (inputParams->count() > 0){
			QByteArray itemData = inputParams->at(0).GetFieldArgumentValue("Item").toByteArray();
			if (!itemData.isEmpty()){
				imtbase::CTreeItemModel settingsModel;
				if (settingsModel.CreateFromJson(itemData)){
					bool retVal = true;
					for (int i = 0; i < settingsModel.GetItemsCount(); i++){
						QByteArray parameterId;
						if (settingsModel.ContainsKey("Id", i)){
							parameterId = settingsModel.GetData("Id", i).toByteArray();
						}

						if (!parameterId.isEmpty()){
							representationPtr = settingsModel.GetModelFromItem(i);

							if (m_userSettingsRepresentationControllerCompPtr->GetModelId() == parameterId){
								istd::TDelPtr<imtauth::IUserSettings> userSettingsPtr = m_userSettingsInfoFactCompPtr.CreateInstance();
								userSettingsPtr->SetUserId(userId);

								iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
								Q_ASSERT(paramSetPtr != nullptr);

								if (paramSetPtr == nullptr){
									return false;
								}

								retVal = retVal && m_userSettingsRepresentationControllerCompPtr->GetDataModelFromRepresentation(*representationPtr, *paramSetPtr);
								if (retVal){
									imtbase::ICollectionInfo::Ids collectionIds = m_userSettingsCollectionCompPtr->GetElementIds();
									if (collectionIds.contains(userId)){
										m_userSettingsCollectionCompPtr->SetObjectData(userId, *userSettingsPtr);
									}
									else{
										m_userSettingsCollectionCompPtr->InsertNewObject("", "", "", userSettingsPtr.GetPtr(), userId);
									}
								}
							}
							else if (m_representationControllerCompPtr->GetModelId() == parameterId){
								retVal = retVal && m_representationControllerCompPtr->GetDataModelFromRepresentation(*representationPtr, *m_dataModelCompPtr);
							}
						}
					}

					return retVal;
				}
			}
		}
	}

	return false;
}


} // namespace imtgql


