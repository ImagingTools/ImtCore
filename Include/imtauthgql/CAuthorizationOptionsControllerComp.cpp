#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CAuthorizationOptionsControllerComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	rootModelPtr.SetPtr(BaseClass::CreateRepresentationFromRequest(gqlRequest, errorMessage));
	if (!rootModelPtr.IsValid()){
		return nullptr;
	}

	if (rootModelPtr->ContainsKey("data")){
		imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->GetTreeItemModel("data");
		Q_ASSERT(dataModelPtr != nullptr);
		if (dataModelPtr != nullptr){
			QByteArray value = dataModelPtr->GetData("Value").toByteArray();
			if (value != "NO_USER_MANAGEMENT"){
				if (m_superuserProviderCompPtr.IsValid()){
					bool superuserExists = m_superuserProviderCompPtr->SuperuserExists();
					dataModelPtr->SetData("SuperUserExists", superuserExists);
				}
			}

			if (m_databaseServerConnectionChekerCompPtr.IsValid()){
				bool isConnected = m_databaseServerConnectionChekerCompPtr->IsDatabaseServerConnected();
				dataModelPtr->SetData("DatabaseConnectionState", isConnected);
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


