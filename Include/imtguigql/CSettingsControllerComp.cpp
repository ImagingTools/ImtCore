#include <imtguigql/CSettingsControllerComp.h>


//ImtCore includes
#include <imtauth/CUserSettings.h>


namespace imtguigql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CSettingsControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return nullptr;
	}

	const QList<imtgql::CGqlObject>* inputParamsPtr = gqlRequest.GetParams();
	imtgql::CGqlObject gqlObject;

	int operationType = OT_UNKNOWN;
	if (!GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
		return nullptr;
	}

	Q_ASSERT(operationType != OT_UNKNOWN);

	switch (operationType){
	case OT_GET:
		return GetSettings(*inputParamsPtr, gqlObject, gqlRequest.GetGqlContext(), errorMessage);
	case OT_SAVE:
		return SaveSettings(*inputParamsPtr, gqlObject, gqlRequest.GetGqlContext(),  errorMessage);
	}

	return nullptr;
}


// protected methods
bool CSettingsControllerComp::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			imtgql::CGqlObject& gqlObject,
			QString& errorMessage,
			int& operationType) const
{
	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	if(fieldList == nullptr){
		return false;
	}

	int count = fieldList->count();
	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "Get"){
			gqlObject = fieldList->at(i);
			operationType = OT_GET;

			return true;
		}

		if (fieldList->at(i).GetId() == "Save"){
			gqlObject = fieldList->at(i);
			operationType = OT_SAVE;

			return true;
		}
	}

	return false;
}


imtbase::CTreeItemModel* CSettingsControllerComp::GetSettings(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		const imtgql::IGqlContext* gqlContext,
		QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;

	if (m_userSettingsCollectionCompPtr.IsValid() && gqlContext != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContext->GetUserInfo();
		if (userInfoPtr != nullptr){
			QByteArray userId = userInfoPtr->GetUsername();
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
				const imtauth::IUserSettings* userSettingsPtr = dynamic_cast<const imtauth::IUserSettings*>(dataPtr.GetPtr());
				if (userSettingsPtr != nullptr){
					dataModel = new imtbase::CTreeItemModel();
					QByteArray settingsData = userSettingsPtr->GetSettings();
					if (dataModel->Parse(settingsData)){
						rootModel->SetExternTreeModel("data", dataModel);

						return rootModel;
					}
				}
			}
		}
	}

	if (m_settingsDataProviderCompPtr.IsValid()){
		dataModel =  m_settingsDataProviderCompPtr->GetTreeItemModel(inputParams, QByteArrayList(), gqlContext);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CSettingsControllerComp::SaveSettings(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		const imtgql::IGqlContext* gqlContext,
		QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	if (m_userSettingsCollectionCompPtr.IsValid() && gqlContext != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContext->GetUserInfo();
		if (userInfoPtr != nullptr){
			QByteArray userId = userInfoPtr->GetUsername();
			QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
			if (!itemData.isEmpty()){
				QJsonDocument jsonResponse = QJsonDocument::fromJson(itemData);
				QJsonObject jsonObject = jsonResponse.object();
				if (jsonResponse.isArray()){
					QJsonArray jsonArray = jsonResponse.array();
					QJsonObject databaseObj;
					for (const QJsonValue & value : jsonArray){
						QJsonObject obj = value.toObject();

						if (obj["Id"] == "DBSettings"){
							databaseObj = obj;
							break;
						}
					}

					if (!databaseObj.isEmpty()){
						QJsonArray jsonElementsArray = databaseObj["Elements"].toArray();

						QJsonObject elementsObj;
						for (const QJsonValue & value : jsonElementsArray){
							QJsonObject obj = value.toObject();

							if (obj["Id"] == "LisaSettings"){
								elementsObj = obj;
								break;
							}
						}

						if (!elementsObj.isEmpty()){
							QJsonArray jsonParametersArray = elementsObj["Parameters"].toArray();

							for (const QJsonValue & value : jsonParametersArray){
								QJsonObject obj = value.toObject();
								QByteArray objectId = obj["Id"].toString().toUtf8();
								QByteArray parameterValue = obj["Value"].toString().toUtf8();
								if (objectId == "DBName"){
									m_databaseSettingsCompPtr->SetDatabaseName(parameterValue);
								}
								else if (objectId == "Host"){
									m_databaseSettingsCompPtr->SetHost(parameterValue);
								}
								else if (objectId == "Password"){
									m_databaseSettingsCompPtr->SetPassword(parameterValue);
								}
								else if (objectId == "Port"){
									m_databaseSettingsCompPtr->SetPort(parameterValue.toInt());
								}
								else if (objectId == "Username"){
									m_databaseSettingsCompPtr->SetUserName(parameterValue);
								}
							}
						}
					}
				}

				istd::TDelPtr<imtauth::CUserSettings> userSettingsPtr = new imtauth::CUserSettings();
				userSettingsPtr->SetUserId(userId);
				userSettingsPtr->SetSettings(itemData);

				imtbase::ICollectionInfo::Ids collectionIds = m_userSettingsCollectionCompPtr->GetElementIds();
				if (collectionIds.contains(userId)){
					m_userSettingsCollectionCompPtr->SetObjectData(userId, *userSettingsPtr);
				}
				else{
					m_userSettingsCollectionCompPtr->InsertNewObject("", "", "", userSettingsPtr.PopPtr(), userId);
				}

				rootModel->SetData("Status", "Ok");
			}
		}
	}

	return rootModel;
}

} // namespace imtgql


