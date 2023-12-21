#include <imtgql/CPermissibleGqlRequestHandlerComp.h>


namespace imtgql
{


// public methods

imtbase::CTreeItemModel* CPermissibleGqlRequestHandlerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if(CheckPermissions(gqlRequest, errorMessage)){
		return CreateInternalResponse(gqlRequest, errorMessage);
	}

	QString userName;
	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userName = userInfoPtr->GetName();
		}
	}

	errorMessage = QString("Unable to create response for command %1 because invalid permissions for %1").arg(qPrintable(gqlRequest.GetCommandId())).arg(userName);
	SendErrorMessage(0, errorMessage);

	return nullptr;
}


// protected methods

bool CPermissibleGqlRequestHandlerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	bool retVal = true;

	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			QByteArray userId = userInfoPtr->GetId();
			if (!userInfoPtr->IsAdmin()){
				if(m_commandPermissionsCompPtr.IsValid()){
					imtauth::IUserInfo::FeatureIds permissions = userInfoPtr->GetPermissions();

					QByteArray requestedCommandId = gqlRequest.GetCommandId();
					QByteArrayList commandIds = m_commandPermissionsCompPtr->GetCommandIds();

					if(commandIds.contains(requestedCommandId)){
						QByteArrayList permissionIds = m_commandPermissionsCompPtr->GetCommandPermissions(requestedCommandId);
						if (m_checkPermissionCompPtr.IsValid()){
							retVal = m_checkPermissionCompPtr->CheckPermission(permissions, permissionIds);
						}
					}
				}
			}
		}
	}

	return retVal;
}


// reimplemented (imtgql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CPermissibleGqlRequestHandlerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return nullptr;
}


} // namespace imtgql


