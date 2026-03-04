// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace imtservergql
{


// public methods

imtbase::CTreeItemModel* CPermissibleGqlRequestHandlerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if(CheckPermissions(gqlRequest, errorMessage)){
		return CreateInternalResponse(gqlRequest, errorMessage);
	}

	QString userName;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userName = userInfoPtr->GetName();
		}
	}

	errorMessage = QString("Invalid permissions for the user '%1'").arg(userName);
	SendErrorMessage(0, errorMessage);

	return nullptr;
}


// protected methods

bool CPermissibleGqlRequestHandlerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	if(!m_commandPermissionsCompPtr.IsValid()){
		return true;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		return false;
	}

	if (userInfoPtr->IsAdmin()){
		return true;
	}

	imtauth::IUserInfo::FeatureIds permissions = userInfoPtr->GetPermissions();

	QByteArray requestedCommandId = gqlRequest.GetCommandId();
	QByteArrayList commandIds = m_commandPermissionsCompPtr->GetCommandIds();

	if(commandIds.contains(requestedCommandId)){
		QByteArrayList permissionIds = m_commandPermissionsCompPtr->GetCommandPermissions(requestedCommandId);
		if (m_checkPermissionCompPtr.IsValid()){
			return m_checkPermissionCompPtr->CheckPermission(permissions, permissionIds);
		}
	}

	return false;
}


// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CPermissibleGqlRequestHandlerComp::CreateInternalResponse(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return nullptr;
}


} // namespace imtservergql


