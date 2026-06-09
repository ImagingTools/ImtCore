// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>

// ImtCore includes
#include <imtgql/IGqlContext.h>


namespace imtservergql
{


// public methods

QJsonObject CPermissibleGqlRequestHandlerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
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

	return QJsonObject();
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
			if (m_checkPermissionCompPtr->CheckPermission(permissions, permissionIds)){
				return true;
			}

			// Fallback: check delegated roles as additional permissions
			if (m_delegatedAccessCompPtr.IsValid()){
				QByteArray userId = gqlContextPtr->GetUserId();
				QByteArray homeTenantId = gqlContextPtr->GetTenantId();
				if (!userId.isEmpty() && !homeTenantId.isEmpty()){
					// Get all accessible tenants and collect delegated role IDs
					imtauth::DelegatedTenantList accessibleTenants =
						m_delegatedAccessCompPtr->GetAccessibleTenants(userId, homeTenantId);
					QByteArrayList delegatedPermissions;
					for (const imtauth::DelegatedTenantInfo& tenantInfo : accessibleTenants){
						if (tenantInfo.isDelegated){
							for (const QByteArray& roleId : tenantInfo.roleIds){
								if (!delegatedPermissions.contains(roleId)){
									delegatedPermissions.append(roleId);
								}
							}
						}
					}
					if (!delegatedPermissions.isEmpty()){
						return m_checkPermissionCompPtr->CheckPermission(delegatedPermissions, permissionIds);
					}
				}
			}
		}
	}

	return false;
}


QByteArray CPermissibleGqlRequestHandlerComp::GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			return userInfoPtr->GetId();
		}
	}

	return QByteArray();
}


// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

QJsonObject CPermissibleGqlRequestHandlerComp::CreateInternalResponse(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return QJsonObject();
}


} // namespace imtservergql


