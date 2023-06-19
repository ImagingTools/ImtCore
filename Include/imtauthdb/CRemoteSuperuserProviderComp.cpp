#include <imtauthdb/CRemoteSuperuserProviderComp.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtauth::ISuperuserProvider)

bool CRemoteSuperuserProviderComp::SuperuserExists() const
{
	if (!m_gqlRequestCompPtr.IsValid()){
		return false;
	}

	imtgql::CGqlRequest gqlRequest(imtgql::CGqlRequest::RT_QUERY, "UsersList");
	imtgql::CGqlObject queryFields("items");
	queryFields.InsertField("Id");
	queryFields.InsertField("UserId");
	gqlRequest.AddField(queryFields);

	QString errorMessage;
	imtbase::CTreeItemModel* usersModelPtr = m_gqlRequestCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (usersModelPtr == nullptr){
		return false;
	}

	if (!usersModelPtr->ContainsKey("data")){
		return false;
	}

	imtbase::CTreeItemModel* dataModelPtr = usersModelPtr->GetTreeItemModel("data");
	if (dataModelPtr == nullptr){
		return false;
	}

	if (!dataModelPtr->ContainsKey("items")){
		return false;
	}

	imtbase::CTreeItemModel* itemsModelPtr = dataModelPtr->GetTreeItemModel("items");
	if (itemsModelPtr == nullptr){
		return false;
	}

	for (int i = 0; i < itemsModelPtr->GetItemsCount(); i++){
		if (itemsModelPtr->ContainsKey("UserId", i)){
			QByteArray username = itemsModelPtr->GetData("UserId", i).toByteArray();
			if (*m_superuserIdAttrPtr == username){
				return true;
			}
		}
	}

	return false;
}


QByteArray CRemoteSuperuserProviderComp::GetSuperuserId() const
{
	return *m_superuserIdAttrPtr;
}


} // namespace imtauthdb


