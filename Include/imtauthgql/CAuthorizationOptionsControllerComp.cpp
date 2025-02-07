#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

sdl::imtauth::Authorization::CAuthorizationPayload CAuthorizationOptionsControllerComp::OnAuthorization(
			const sdl::imtauth::Authorization::CAuthorizationGqlRequest& /*authorizationRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtauth::Authorization::CAuthorizationPayload();
}


sdl::imtauth::Authorization::CAuthorizationPayload CAuthorizationOptionsControllerComp::OnUserToken(
			const sdl::imtauth::Authorization::CUserTokenGqlRequest& /*userTokenRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtauth::Authorization::CAuthorizationPayload();
}


sdl::imtauth::Authorization::CUserManagementPayload CAuthorizationOptionsControllerComp::OnGetUserMode(
			const sdl::imtauth::Authorization::CGetUserModeGqlRequest& /*getUserModeRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	if (!m_selectionParamCompPtr.IsValid()){
		Q_ASSERT_X(false, "Unable to get an user mode. Error: Reference 'SelectionParam' was not set", "CAuthorizationOptionsControllerComp");

		return sdl::imtauth::Authorization::CUserManagementPayload();
	}

	sdl::imtauth::Authorization::CUserManagementPayload::V1_0 payload;
	int index = m_selectionParamCompPtr->GetSelectedOptionIndex();

	QString userMode = sdl::imtauth::Authorization::CUserMode::V1_0::UserModeFields::STRONG_USER_MANAGEMENT;
	switch (index) {
	case UM_NO_USER_MANAGEMENT:
		userMode = sdl::imtauth::Authorization::CUserMode::V1_0::UserModeFields::NO_USER_MANAGEMENT;
		break;

	case UM_OPTIONAL_USER_MANAGEMENT:
		userMode = sdl::imtauth::Authorization::CUserMode::V1_0::UserModeFields::OPTIONAL_USER_MANAGEMENT;
		break;

	case UM_STRONG_USER_MANAGEMENT:
		userMode = sdl::imtauth::Authorization::CUserMode::V1_0::UserModeFields::STRONG_USER_MANAGEMENT;
		break;

	default:
		break;
	}

	payload.UserMode = std::make_optional<QString>(userMode);

	sdl::imtauth::Authorization::CUserManagementPayload retVal;
	retVal.Version_1_0 = std::make_optional(payload);

	return retVal;
}


} // namespace imtauthgql


