#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

sdl::imtauth::Authorization::CAuthorizationPayload::V1_0 CAuthorizationOptionsControllerComp::OnAuthorization(
			const sdl::imtauth::Authorization::V1_0::CAuthorizationGqlRequest& /*authorizationRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtauth::Authorization::CAuthorizationPayload::V1_0();
}


sdl::imtauth::Authorization::CAuthorizationPayload::V1_0 CAuthorizationOptionsControllerComp::OnUserToken(
			const sdl::imtauth::Authorization::V1_0::CUserTokenGqlRequest& /*userTokenRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtauth::Authorization::CAuthorizationPayload::V1_0();
}


sdl::imtauth::Authorization::CUserManagementPayload::V1_0 CAuthorizationOptionsControllerComp::OnGetUserMode(
			const sdl::imtauth::Authorization::V1_0::CGetUserModeGqlRequest& /*getUserModeRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	if (!m_selectionParamCompPtr.IsValid()){
		Q_ASSERT_X(false, "Unable to get an user mode. Error: Reference 'SelectionParam' was not set", "CAuthorizationOptionsControllerComp");

		return sdl::imtauth::Authorization::CUserManagementPayload::V1_0();
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

	return payload;
}


} // namespace imtauthgql


