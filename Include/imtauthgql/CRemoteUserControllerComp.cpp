// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	return SendModelRequest<sdl::imtauth::Users::CChangePasswordPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Users::CRegisterUserPayload CRemoteUserControllerComp::OnRegisterUser(
			const sdl::imtauth::Users::CRegisterUserGqlRequest& /*registerUserRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Users::CRegisterUserPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Users::CCheckEmailPayload CRemoteUserControllerComp::OnCheckEmail(
			const sdl::imtauth::Users::CCheckEmailGqlRequest& /*checkEmailRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Users::CCheckEmailPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Users::CSendEmailCodePayload CRemoteUserControllerComp::OnSendEmailCode(
			const sdl::imtauth::Users::CSendEmailCodeGqlRequest& /*sendEmailCodeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Users::CSendEmailCodePayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Users::CCheckEmailCodePayload CRemoteUserControllerComp::OnCheckEmailCode(
			const sdl::imtauth::Users::CCheckEmailCodeGqlRequest& /*checkEmailCodeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Users::CCheckEmailCodePayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Users::CCheckSuperuserPayload CRemoteUserControllerComp::OnCheckSuperuserExists(
			const sdl::imtauth::Users::CCheckSuperuserExistsGqlRequest& /*checkSuperuserExistsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Users::CCheckSuperuserPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Users::CCreateSuperuserPayload CRemoteUserControllerComp::OnCreateSuperuser(
			const sdl::imtauth::Users::CCreateSuperuserGqlRequest& /*createSuperuserRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Users::CCreateSuperuserPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Users::CUserObjectId CRemoteUserControllerComp::OnGetUserObjectId(
			const sdl::imtauth::Users::CGetUserObjectIdGqlRequest& /*getUserObjectIdRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Users::CUserObjectId>(gqlRequest, errorMessage);
}


} // namespace imtauthgql


