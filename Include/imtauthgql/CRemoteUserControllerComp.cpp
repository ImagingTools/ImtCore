#include <imtauthgql/CRemoteUserControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtauth::ISuperuserProvider)

imtauth::ISuperuserProvider::ExistsStatus CRemoteUserControllerComp::SuperuserExists(QString& errorMessage) const
{
	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_QUERY, sdl::imtauth::Users::CCheckSuperuserExistsGqlRequest::GetCommandId());
	sdl::imtauth::Users::CCheckSuperuserExistsGqlRequest checkSuperuserExistsGqlRequest(gqlRequest, false);

	sdl::imtauth::Users::CCheckSuperuserPayload result = OnCheckSuperuserExists(checkSuperuserExistsGqlRequest, gqlRequest, errorMessage);
	if (!result.Version_1_0.has_value()){
		return imtauth::ISuperuserProvider::ES_UNKNOWN;
	}

	if (result.Version_1_0->message){
		errorMessage = *result.Version_1_0->message;
	}

	imtauth::ISuperuserProvider::ExistsStatus retVal = imtauth::ISuperuserProvider::ES_UNKNOWN;
	if (result.Version_1_0->status){
		sdl::imtauth::Users::ExistsStatus status = *result.Version_1_0->status;
		if (status == sdl::imtauth::Users::ExistsStatus::EXISTS){
			retVal = imtauth::ISuperuserProvider::ES_EXISTS;
		}
		else if (status == sdl::imtauth::Users::ExistsStatus::NOT_EXISTS){
			retVal = imtauth::ISuperuserProvider::ES_NOT_EXISTS;
		}
	}

	return retVal;
}


QByteArray CRemoteUserControllerComp::GetSuperuserId() const
{
	return QByteArray();
}


// reimplemented (sdl::imtauth::Users::CGraphQlHandlerCompBase)

sdl::imtauth::Users::CChangePasswordPayload CRemoteUserControllerComp::OnChangePassword(
			const sdl::imtauth::Users::CChangePasswordGqlRequest& /*changePasswordRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CChangePasswordPayload::V1_0 response;

	if (!SendModelRequest<sdl::imtauth::Users::CChangePasswordPayload::V1_0, sdl::imtauth::Users::CChangePasswordPayload>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote change password. Error: %1").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CRemoteUserControllerComp");
		return sdl::imtauth::Users::CChangePasswordPayload();
	}

	sdl::imtauth::Users::CChangePasswordPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtauth::Users::CRegisterUserPayload CRemoteUserControllerComp::OnRegisterUser(
			const sdl::imtauth::Users::CRegisterUserGqlRequest& /*registerUserRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CRegisterUserPayload::V1_0 response;

	if (!SendModelRequest<sdl::imtauth::Users::CRegisterUserPayload::V1_0, sdl::imtauth::Users::CRegisterUserPayload>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote register user. Error: %1").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CRemoteUserControllerComp");
		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	sdl::imtauth::Users::CRegisterUserPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtauth::Users::CCheckEmailPayload CRemoteUserControllerComp::OnCheckEmail(
			const sdl::imtauth::Users::CCheckEmailGqlRequest& /*checkEmailRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CCheckEmailPayload::V1_0 response;

	if (!SendModelRequest<sdl::imtauth::Users::CCheckEmailPayload::V1_0, sdl::imtauth::Users::CCheckEmailPayload>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote check email. Error: %1").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CRemoteUserControllerComp");
		return sdl::imtauth::Users::CCheckEmailPayload();
	}

	sdl::imtauth::Users::CCheckEmailPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtauth::Users::CSendEmailCodePayload CRemoteUserControllerComp::OnSendEmailCode(
			const sdl::imtauth::Users::CSendEmailCodeGqlRequest& /*sendEmailCodeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CSendEmailCodePayload::V1_0 response;

	if (!SendModelRequest<sdl::imtauth::Users::CSendEmailCodePayload::V1_0, sdl::imtauth::Users::CSendEmailCodePayload>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote send email code. Error: %1").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CRemoteUserControllerComp");
		return sdl::imtauth::Users::CSendEmailCodePayload();
	}

	sdl::imtauth::Users::CSendEmailCodePayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtauth::Users::CCheckEmailCodePayload CRemoteUserControllerComp::OnCheckEmailCode(
			const sdl::imtauth::Users::CCheckEmailCodeGqlRequest& /*checkEmailCodeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CCheckEmailCodePayload::V1_0 response;

	if (!SendModelRequest<sdl::imtauth::Users::CCheckEmailCodePayload::V1_0, sdl::imtauth::Users::CCheckEmailCodePayload>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote check email code. Error: %1").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CRemoteUserControllerComp");
		return sdl::imtauth::Users::CCheckEmailCodePayload();
	}

	sdl::imtauth::Users::CCheckEmailCodePayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtauth::Users::CCheckSuperuserPayload CRemoteUserControllerComp::OnCheckSuperuserExists(
			const sdl::imtauth::Users::CCheckSuperuserExistsGqlRequest& /*checkSuperuserExistsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CCheckSuperuserPayload::V1_0 response;

	if (!SendModelRequest<sdl::imtauth::Users::CCheckSuperuserPayload::V1_0, sdl::imtauth::Users::CCheckSuperuserPayload>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote check superuser exists. Error: %1").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CRemoteUserControllerComp");
		return sdl::imtauth::Users::CCheckSuperuserPayload();
	}

	sdl::imtauth::Users::CCheckSuperuserPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtauth::Users::CCreateSuperuserPayload CRemoteUserControllerComp::OnCreateSuperuser(
			const sdl::imtauth::Users::CCreateSuperuserGqlRequest& /*createSuperuserRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CCreateSuperuserPayload::V1_0 response;

	if (!SendModelRequest<sdl::imtauth::Users::CCreateSuperuserPayload::V1_0, sdl::imtauth::Users::CCreateSuperuserPayload>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote create superuser. Error: %1").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CRemoteUserControllerComp");
		return sdl::imtauth::Users::CCreateSuperuserPayload();
	}

	sdl::imtauth::Users::CCreateSuperuserPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtauth::Users::CRemoveUserPayload CRemoteUserControllerComp::OnUsersRemove(
			const sdl::imtauth::Users::CUsersRemoveGqlRequest& /*removeUserRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CRemoveUserPayload::V1_0 response;

	if (!SendModelRequest<sdl::imtauth::Users::CRemoveUserPayload::V1_0, sdl::imtauth::Users::CRemoveUserPayload>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote remove user. Error: %1").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CRemoteUserControllerComp");
		return sdl::imtauth::Users::CRemoveUserPayload();
	}

	sdl::imtauth::Users::CRemoveUserPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


} // namespace imtauthgql


