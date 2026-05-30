// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTokenBasedPermissionsProviderComp.h>


// ImtCore includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::ITokenBasedPermissionsProvider)

const QByteArrayList CTokenBasedPermissionsProviderComp::GetPermissions(const QByteArray& token) const
{
	namespace authsdl = sdl::V1_0::imtauth;

	authsdl::GetPermissionsRequestArguments arguments;
	arguments.input.accessToken = token;

	imtgql::CGqlRequest gqlRequest;
	if (!authsdl::CGetPermissionsGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArrayList();
	}

	QString errorMessage;
	authsdl::CPermissionList payload = SendModelRequest<authsdl::CPermissionList>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QByteArrayList();
	}

	if (!payload.HasValue()){
		return QByteArrayList();
	}

	if (!payload.permissions.HasValue()){
		return QByteArrayList();
	}

	return payload.permissions->ToList();
}


} // namespace imtauthgql


