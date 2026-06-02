// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/AuthorizationOptions.h>
#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::V1_0::imtauth::CAuthorizationOptionsGqlHandlerCompBase)

sdl::V1_0::imtauth::CUserManagementPayload CAuthorizationOptionsControllerComp::OnGetUserMode(
			const sdl::V1_0::imtauth::CGetUserModeGqlRequest& /*getUserModeRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	if (!m_selectionParamCompPtr.IsValid()){
		Q_ASSERT_X(false, "Unable to get an user mode. Error: Reference 'SelectionParam' was not set", "CAuthorizationOptionsControllerComp");
		return sdl::V1_0::imtauth::CUserManagementPayload();
	}

	sdl::V1_0::imtauth::CUserManagementPayload payload;
	int index = m_selectionParamCompPtr->GetSelectedOptionIndex();

	sdl::V1_0::imtauth::UserManagementMode userMode =
		sdl::V1_0::imtauth::UserManagementMode::STRONG_USER_MANAGEMENT;
	switch (index){
	case UM_NO_USER_MANAGEMENT:
		userMode = sdl::V1_0::imtauth::UserManagementMode::NO_USER_MANAGEMENT;
		break;

	case UM_OPTIONAL_USER_MANAGEMENT:
		userMode = sdl::V1_0::imtauth::UserManagementMode::OPTIONAL_USER_MANAGEMENT;
		break;

	case UM_STRONG_USER_MANAGEMENT:
		userMode = sdl::V1_0::imtauth::UserManagementMode::STRONG_USER_MANAGEMENT;
		break;

	default:
		break;
	}

	payload.userMode = userMode;

	return payload;
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CAuthorizationOptionsControllerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == sdl::V1_0::imtauth::CGetUserModeGqlRequest::GetCommandId()){
		return true;
	}

	return BaseClass::CheckPermissions(gqlRequest, errorMessage);
}


} // namespace imtauthgql


