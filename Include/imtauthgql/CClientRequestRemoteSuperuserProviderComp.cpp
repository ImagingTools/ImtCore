#include <imtauthgql/CClientRequestRemoteSuperuserProviderComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::ISuperuserProvider)

bool CClientRequestRemoteSuperuserProviderComp::SuperuserExists() const
{
	imtgql::CGqlRequest gqlRequest(imtgql::CGqlRequest::RT_QUERY, "UsersList");
	imtgql::CGqlObject queryFields("items");
	queryFields.InsertField("Id");
	queryFields.InsertField("UserId");
	gqlRequest.AddField(queryFields);

	imtbase::CTreeItemModel usersModel;
	bool ok = SendModelRequest(gqlRequest, usersModel);
	if (ok){
		imtbase::CTreeItemModel* dataModelPtr = usersModel.GetTreeItemModel("data");
		if (dataModelPtr == nullptr){
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
	}

	return false;
}


QByteArray CClientRequestRemoteSuperuserProviderComp::GetSuperuserId() const
{
	return *m_superuserIdAttrPtr;
}


} // namespace imtauthgql


