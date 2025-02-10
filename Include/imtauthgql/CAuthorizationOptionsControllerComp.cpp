#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::AuthorizationOptions::CGraphQlHandlerCompBase)

sdl::imtauth::AuthorizationOptions::CUserManagementPayload CAuthorizationOptionsControllerComp::OnGetUserMode(
	const sdl::imtauth::AuthorizationOptions::CGetUserModeGqlRequest& getUserModeRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	if (!m_selectionParamCompPtr.IsValid()){
		Q_ASSERT_X(false, "Unable to get an user mode. Error: Reference 'SelectionParam' was not set", "CAuthorizationOptionsControllerComp");
		return sdl::imtauth::AuthorizationOptions::CUserManagementPayload();
	}

	sdl::imtauth::AuthorizationOptions::CUserManagementPayload payload;
	int index = m_selectionParamCompPtr->GetSelectedOptionIndex();

	sdl::imtauth::AuthorizationOptions::UserMode userMode =
		sdl::imtauth::AuthorizationOptions::UserMode::STRONG_USER_MANAGEMENT;
	switch (index) {
	case UM_NO_USER_MANAGEMENT:
		userMode = sdl::imtauth::AuthorizationOptions::UserMode::NO_USER_MANAGEMENT;
		break;

	case UM_OPTIONAL_USER_MANAGEMENT:
		userMode = sdl::imtauth::AuthorizationOptions::UserMode::OPTIONAL_USER_MANAGEMENT;
		break;

	case UM_STRONG_USER_MANAGEMENT:
		userMode = sdl::imtauth::AuthorizationOptions::UserMode::STRONG_USER_MANAGEMENT;
		break;

	default:
		break;
	}

	payload.Version_1_0.emplace();
	payload.Version_1_0->UserMode = userMode;

	return payload;
}


} // namespace imtauthgql
