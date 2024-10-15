#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

sdl::imtauth::Authorization::V1_0::CAuthorizationPayload CAuthorizationOptionsControllerComp::OnAuthorization(
			const sdl::imtauth::Authorization::V1_0::CAuthorizationGqlRequest& /*authorizationRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtauth::Authorization::V1_0::CAuthorizationPayload();
}


sdl::imtauth::Authorization::V1_0::CUserManagementPayload CAuthorizationOptionsControllerComp::OnGetUserMode(
			const sdl::imtauth::Authorization::V1_0::CGetUserModeGqlRequest& /*getUserModeRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	if (!m_selectionParamCompPtr.IsValid()){
		Q_ASSERT_X(false, "Unable to get an user mode. Error: Reference 'SelectionParam' was not set", "CAuthorizationOptionsControllerComp");

		return sdl::imtauth::Authorization::V1_0::CUserManagementPayload();
	}

	sdl::imtauth::Authorization::V1_0::CUserManagementPayload payload;
	int index = m_selectionParamCompPtr->GetSelectedOptionIndex();


	switch (index) {
	case UM_NO_USER_MANAGEMENT:
		payload.SetUserMode(sdl::imtauth::Authorization::V1_0::CUserMode::UserModeFields::NO_USER_MANAGEMENT);
		break;
	case UM_OPTIONAL_USER_MANAGEMENT:
		payload.SetUserMode(sdl::imtauth::Authorization::V1_0::CUserMode::UserModeFields::OPTIONAL_USER_MANAGEMENT);
		break;
	case UM_STRONG_USER_MANAGEMENT:
		payload.SetUserMode(sdl::imtauth::Authorization::V1_0::CUserMode::UserModeFields::STRONG_USER_MANAGEMENT);
		break;
	default:
		payload.SetUserMode(sdl::imtauth::Authorization::V1_0::CUserMode::UserModeFields::STRONG_USER_MANAGEMENT);

		break;
	}
	return payload;
}


} // namespace imtauthgql


