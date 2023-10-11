#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CAuthorizationOptionsControllerComp::CreateRepresentationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	rootModelPtr.SetPtr(BaseClass::CreateRepresentationFromRequest(gqlRequest, errorMessage));
	if (!rootModelPtr.IsValid()){
		return nullptr;
	}

	if (rootModelPtr->ContainsKey("data")){
		imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->GetTreeItemModel("data");
		Q_ASSERT(dataModelPtr != nullptr);
		if (dataModelPtr == nullptr){
			return nullptr;
		}

		imtbase::CTreeItemModel* parametersModelPtr = dataModelPtr->GetTreeItemModel("Parameters");
		if (parametersModelPtr == nullptr){
			return nullptr;
		}

		int value = dataModelPtr->GetData("Value").toInt();
		if (value < 0){
			return nullptr;
		}

		QByteArray userModeId = parametersModelPtr->GetData("Id", value).toByteArray();
		if (userModeId == "STRONG_USER_MANAGEMENT"){
			if (m_superuserProviderCompPtr.IsValid()){
				QString message;
				bool superuserExists = m_superuserProviderCompPtr->SuperuserExists(message);
				dataModelPtr->SetData("SuperUserExists", superuserExists);

				if (!errorMessage.isEmpty()){
					dataModelPtr->SetData("ErrorMessage", message);
				}
			}
		}

		if (m_databaseServerConnectionChekerCompPtr.IsValid()){
			bool isConnected = m_databaseServerConnectionChekerCompPtr->IsDatabaseServerConnected(errorMessage);
			dataModelPtr->SetData("DatabaseConnectionState", isConnected);
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


