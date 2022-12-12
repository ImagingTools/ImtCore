#include <imtqml/CCommandPermissionsFilterComp.h>


// Qt includes
#include <QtCore/QTranslator>


namespace imtqml
{


// public methods

// reimplemented (imtgql::IItemBasedRepresentationProvider)

QByteArray CCommandPermissionsFilterComp::GetModelId() const
{
	return m_commandsProviderCompPtr->GetModelId();
}


imtbase::CTreeItemModel* CCommandPermissionsFilterComp::GetRepresentation(
		const QList<imtgql::CGqlObject>& params,
		const QByteArrayList& fields,
		const imtgql::IGqlContext* gqlContext)
{
	if(!m_commandsProviderCompPtr.IsValid()){
		return nullptr;
	}
	imtbase::CTreeItemModel* commandsModelPtr = m_commandsProviderCompPtr->GetRepresentation(params, fields, gqlContext);
	imtbase::CTreeItemModel* filteredCommandsModelPtr =new imtbase::CTreeItemModel();

	if (gqlContext != nullptr && commandsModelPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlContext->GetUserInfo();
		if (userInfoPtr != nullptr){
			const QByteArray userId = userInfoPtr->GetUserId();
			if(m_commandPermissionsProviderCompPtr.IsValid()){
				imtauth::IUserInfo::FeatureIds userPermissions = userInfoPtr->GetPermissions();
				for(int i = 0; i < commandsModelPtr->GetItemsCount(); i++){
					const QByteArray commandId = commandsModelPtr->GetData("Id", i).toByteArray();
					const QByteArrayList permissionIds = m_commandPermissionsProviderCompPtr->GetCommandPermissions(commandId);

					if(userInfoPtr->IsAdmin() || m_checkPermissionCompPtr->CheckPermission(userPermissions, permissionIds)){
						int index = filteredCommandsModelPtr->InsertNewItem();
						commandsModelPtr->CopyItemDataToModel(i,filteredCommandsModelPtr, index);
					}
				}
			}
		}
	}

	return filteredCommandsModelPtr;
}


} // namespace imtqml


