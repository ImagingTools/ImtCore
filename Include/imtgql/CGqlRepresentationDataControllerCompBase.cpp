#include <imtgql/CGqlRepresentationDataControllerCompBase.h>


namespace imtgql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CGqlRepresentationDataControllerCompBase::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if(CheckPermissions(gqlRequest, errorMessage)){
		return CreateInternalResponse(gqlRequest, errorMessage);
	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetGqlContext()->GetUserInfo();
	QString userName = userInfoPtr->GetName();

	errorMessage = QT_TR_NOOP("Invalid permissions for " + userName);

	imtbase::CTreeItemModel* errorsItemModelPtr = rootModelPtr->AddTreeModel("errors");
	rootModelPtr->SetExternTreeModel("errors", errorsItemModelPtr);
	errorsItemModelPtr->SetData("message", errorMessage);

	return rootModelPtr;
}


QByteArrayList CGqlRepresentationDataControllerCompBase::GetModelIds() const
{
	QByteArrayList retVal;

	if (m_modelIdsCompPtr.IsValid()){
		for (int i = 0; i < m_modelIdsCompPtr.GetCount(); i++){
			retVal << m_modelIdsCompPtr[i];
		}
	}

	return retVal;
}


QByteArrayList CGqlRepresentationDataControllerCompBase::GetContextIds() const
{
	QByteArrayList retVal;

	if (m_contextIdsCompPtr.IsValid()){
		for (int i = 0; i < m_contextIdsCompPtr.GetCount(); i++){
			retVal << m_contextIdsCompPtr[i];
		}
	}

	return retVal;
}


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerCompBase)

imtbase::CTreeItemModel* CGqlRepresentationDataControllerCompBase::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return nullptr;
}


bool CGqlRepresentationDataControllerCompBase::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	bool result = true;
	if (gqlRequest.GetGqlContext() != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetGqlContext()->GetUserInfo();

		if (userInfoPtr != nullptr){
			QByteArray userId = userInfoPtr->GetUserId();
			if (!userInfoPtr->IsAdmin()){
				if(m_commandPermissionsCompPtr.IsValid())
				{
					imtauth::IUserInfo::FeatureIds permissions = userInfoPtr->GetPermissions();
					QByteArray gqlCommand = gqlRequest.GetCommandId();
					QByteArrayList commandIds = m_commandPermissionsCompPtr->GetCommandIds();
					if(commandIds.contains(gqlCommand))
					{
						QByteArrayList permissionIds = m_commandPermissionsCompPtr->GetCommandPermissions(gqlCommand);
						if (m_checkPermissionCompPtr.IsValid()){
							result = m_checkPermissionCompPtr->CheckPermission(permissions, permissionIds);
						}
					}
				}
			}
		}
	}

	return result;
}


} // namespace imtgql


