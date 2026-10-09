// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


// ImtCore includes
#include <imtservergql/imtservergql.h>


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

	errorMessage = QStringLiteral("Invalid permissions for the user '%1'").arg(userName);
	SendWarningMessage(0, QStringLiteral("%1 (command '%2')").arg(errorMessage, QString::fromUtf8(gqlRequest.GetCommandId())));

	return QJsonObject();
}


// protected methods

bool CPermissibleGqlRequestHandlerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	QString refusalReason;
	if (CheckRequestPermissions(
				gqlRequest,
				m_commandPermissionsCompPtr.GetPtr(),
				m_checkPermissionCompPtr.GetPtr(),
				true,
				refusalReason)){
		return true;
	}

	SendWarningMessage(0, QString("Command '%1' refused: %2")
			.arg(QString::fromUtf8(gqlRequest.GetCommandId()), refusalReason));

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


