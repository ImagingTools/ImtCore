#include <imtgql/CPermissionsControllerComp.h>


// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/CSessionInfo.h>


namespace imtgql
{


imtbase::CTreeItemModel* CPermissionsControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModelPtr = new imtbase::CTreeItemModel();

	imtbase::CTreeItemModel* userPermissionsModelPtr = dataModelPtr->AddTreeModel("UserPermissions");
	imtbase::CTreeItemModel* allPermissionsModelPtr = dataModelPtr->AddTreeModel("AllPermissions");

	if (gqlRequest.GetGqlContext() != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetGqlContext()->GetUserInfo();

		if (userInfoPtr != nullptr){
			QByteArray userId = userInfoPtr->GetUserId();
			if (!userInfoPtr->IsAdmin()){
				if(m_commandPermissionsProviderCompPtr.IsValid())
				{
					imtauth::IUserInfo::FeatureIds permissions = userInfoPtr->GetPermissions();
					for(const QByteArray &permission: permissions)
					{
						int index = userPermissionsModelPtr->InsertNewItem();
						userPermissionsModelPtr->SetData("UserPermissionId", permission, index);
					}
				}
			}
		}
	}

	if(m_commandPermissionsProviderCompPtr.IsValid()){
		for(int providerIndex = 0; providerIndex < m_commandPermissionsProviderCompPtr.GetCount(); providerIndex++){
			const ICommandPermissionsProvider* currentProviderPtr = m_commandPermissionsProviderCompPtr[providerIndex];
			if(currentProviderPtr != nullptr){
				const QByteArrayList currentProviderCommandIds = currentProviderPtr->GetCommandIds();
				const QSet<QByteArray> commandIdsSet = QSet<QByteArray>(currentProviderCommandIds.begin(), currentProviderCommandIds.end());
				for(const QByteArray& commandId: commandIdsSet)
				{
					int commandIndex = allPermissionsModelPtr->InsertNewItem();
					allPermissionsModelPtr->SetData("CommandId", commandId, commandIndex);
					imtbase::CTreeItemModel* commandPermissionsModelPtr = allPermissionsModelPtr->AddTreeModel("CommandPermissions", commandIndex);
					const QByteArrayList permissionsIds = currentProviderPtr->GetCommandPermissions(commandId);
					for(int permissionsIdx = 0; permissionsIdx < permissionsIds.count(); permissionsIdx++){
						const QByteArray permissionsId = permissionsIds[permissionsIdx];
						int permissionIndex = commandPermissionsModelPtr->InsertNewItem();
						commandPermissionsModelPtr->SetData("PermissionsId", permissionsId, permissionIndex);
					}
				}
			}
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModelPtr);

	return rootModelPtr;
}


} // namespace imtgql


