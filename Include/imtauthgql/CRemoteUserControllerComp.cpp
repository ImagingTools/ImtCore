// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteUserControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtauth::ISuperuserProvider)

imtauth::ISuperuserProvider::ExistsStatus CRemoteUserControllerComp::SuperuserExists(QString& errorMessage) const
{
	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_QUERY, sdl::V1_0::imtauth::CCheckSuperuserExistsGqlRequest::GetCommandId());
	sdl::V1_0::imtauth::CCheckSuperuserExistsGqlRequest checkSuperuserExistsGqlRequest(gqlRequest, false);

	sdl::V1_0::imtauth::CCheckSuperuserPayload result = OnCheckSuperuserExists(checkSuperuserExistsGqlRequest, gqlRequest, errorMessage);
	if (result.message){
		errorMessage = *result.message;
	}

	imtauth::ISuperuserProvider::ExistsStatus retVal = imtauth::ISuperuserProvider::ES_UNKNOWN;
	if (result.status){
		sdl::V1_0::imtauth::ExistsStatus status = *result.status;
		if (status == sdl::V1_0::imtauth::ExistsStatus::EXISTS){
			retVal = imtauth::ISuperuserProvider::ES_EXISTS;
		}
		else if (status == sdl::V1_0::imtauth::ExistsStatus::NOT_EXISTS){
			retVal = imtauth::ISuperuserProvider::ES_NOT_EXISTS;
		}
	}

	return retVal;
}


QByteArray CRemoteUserControllerComp::GetSuperuserId() const
{
	return QByteArray();
}


// reimplemented (sdl::V1_0::imtauth::CUsersGqlHandlerCompBase)

sdl::V1_0::imtauth::CChangePasswordPayload CRemoteUserControllerComp::OnChangePassword(
			const sdl::V1_0::imtauth::CChangePasswordGqlRequest& /*changePasswordRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CChangePasswordPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRegisterUserPayload CRemoteUserControllerComp::OnRegisterUser(
			const sdl::V1_0::imtauth::CRegisterUserGqlRequest& /*registerUserRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRegisterUserPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCheckEmailPayload CRemoteUserControllerComp::OnCheckEmail(
			const sdl::V1_0::imtauth::CCheckEmailGqlRequest& /*checkEmailRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCheckEmailPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CSendEmailCodePayload CRemoteUserControllerComp::OnSendEmailCode(
			const sdl::V1_0::imtauth::CSendEmailCodeGqlRequest& /*sendEmailCodeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CSendEmailCodePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCheckEmailCodePayload CRemoteUserControllerComp::OnCheckEmailCode(
			const sdl::V1_0::imtauth::CCheckEmailCodeGqlRequest& /*checkEmailCodeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCheckEmailCodePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCheckSuperuserPayload CRemoteUserControllerComp::OnCheckSuperuserExists(
			const sdl::V1_0::imtauth::CCheckSuperuserExistsGqlRequest& /*checkSuperuserExistsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCheckSuperuserPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCreateSuperuserPayload CRemoteUserControllerComp::OnCreateSuperuser(
			const sdl::V1_0::imtauth::CCreateSuperuserGqlRequest& /*createSuperuserRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCreateSuperuserPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CUserObjectId CRemoteUserControllerComp::OnGetUserObjectId(
			const sdl::V1_0::imtauth::CGetUserObjectIdGqlRequest& /*getUserObjectIdRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CUserObjectId>(gqlRequest, errorMessage);
}


} // namespace imtauthgql


