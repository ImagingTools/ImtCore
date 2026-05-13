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
	namespace authsdl = sdl::imtauth::Authorization;

	authsdl::GetPermissionsRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->accessToken = token;

	imtgql::CGqlRequest gqlRequest;
	if (!authsdl::CGetPermissionsGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArrayList();
	}

	QString errorMessage;
	authsdl::CPermissionList payload = SendModelRequest<authsdl::CPermissionList>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QByteArrayList();
	}

	if (!payload.Version_1_0.HasValue()){
		return QByteArrayList();
	}

	if (!payload.Version_1_0->permissions.HasValue()){
		return QByteArrayList();
	}

	return payload.Version_1_0->permissions->ToList();
}


} // namespace imtauthgql


