#include <imtqml/CCommandPermissionsFilterComp.h>


namespace imtqml
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CCommandPermissionsFilterComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if(!m_commandsProviderCompPtr.IsValid()){
		return nullptr;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		imtbase::CTreeItemModel* commandsModelPtr = m_commandsProviderCompPtr->CreateResponse(gqlRequest, errorMessage);
		if (commandsModelPtr != nullptr){
			const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
			if (userInfoPtr != nullptr){
				const QByteArray userId = userInfoPtr->GetId();
				if(m_commandPermissionsCompPtr.IsValid()){
					imtauth::IUserInfo::FeatureIds userPermissions = userInfoPtr->GetPermissions();
					istd::TDelPtr<imtbase::CTreeItemModel> filteredCommandsModelPtr(new imtbase::CTreeItemModel());

					for(int i = 0; i < commandsModelPtr->GetItemsCount(); i++){
						const QByteArray commandId = commandsModelPtr->GetData("Id", i).toByteArray();
						const QByteArrayList permissionIds = m_commandPermissionsCompPtr->GetCommandPermissions(commandId);

						if(userInfoPtr->IsAdmin() || m_checkPermissionCompPtr->CheckPermission(userPermissions, permissionIds)){
							int index = filteredCommandsModelPtr->InsertNewItem();

							commandsModelPtr->CopyItemDataToModel(i, filteredCommandsModelPtr.GetPtr(), index);
						}
					}

					return filteredCommandsModelPtr.PopPtr();
				}
			}
		}
	}

	return nullptr;
}


} // namespace imtqml


