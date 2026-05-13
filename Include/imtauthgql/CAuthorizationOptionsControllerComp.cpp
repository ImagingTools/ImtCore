// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::AuthorizationOptions::CGraphQlHandlerCompBase)

sdl::imtauth::AuthorizationOptions::CUserManagementPayload CAuthorizationOptionsControllerComp::OnGetUserMode(
			const sdl::imtauth::AuthorizationOptions::CGetUserModeGqlRequest& /*getUserModeRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	if (!m_selectionParamCompPtr.IsValid()){
		Q_ASSERT_X(false, "Unable to get an user mode. Error: Reference 'SelectionParam' was not set", "CAuthorizationOptionsControllerComp");
		return sdl::imtauth::AuthorizationOptions::CUserManagementPayload();
	}

	sdl::imtauth::AuthorizationOptions::CUserManagementPayload payload;
	int index = m_selectionParamCompPtr->GetSelectedOptionIndex();

	sdl::imtauth::AuthorizationOptions::UserManagementMode userMode =
		sdl::imtauth::AuthorizationOptions::UserManagementMode::STRONG_USER_MANAGEMENT;
	switch (index){
	case UM_NO_USER_MANAGEMENT:
		userMode = sdl::imtauth::AuthorizationOptions::UserManagementMode::NO_USER_MANAGEMENT;
		break;

	case UM_OPTIONAL_USER_MANAGEMENT:
		userMode = sdl::imtauth::AuthorizationOptions::UserManagementMode::OPTIONAL_USER_MANAGEMENT;
		break;

	case UM_STRONG_USER_MANAGEMENT:
		userMode = sdl::imtauth::AuthorizationOptions::UserManagementMode::STRONG_USER_MANAGEMENT;
		break;

	default:
		break;
	}

	payload.Version_1_0.emplace();
	payload.Version_1_0->userMode = userMode;

	return payload;
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CAuthorizationOptionsControllerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == sdl::imtauth::AuthorizationOptions::CGetUserModeGqlRequest::GetCommandId()){
		return true;
	}

	return BaseClass::CheckPermissions(gqlRequest, errorMessage);
}


} // namespace imtauthgql


