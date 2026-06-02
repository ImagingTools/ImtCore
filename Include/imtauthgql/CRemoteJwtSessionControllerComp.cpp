// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteJwtSessionControllerComp.h>


// ACF includes
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtauth/ISession.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Sessions.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtauth::IJwtSessionController)

bool CRemoteJwtSessionControllerComp::ValidateSession(const QByteArray& sessionId) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::ValidateSessionRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->sessionId = sessionId;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CValidateSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef sdl::V1_0::imtauth::CValidateSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.isValid.has_value()){
		return *response.isValid;
	}

	return false;
}


imtauth::IJwtSessionController::JwtState CRemoteJwtSessionControllerComp::ValidateJwt(const QByteArray& token) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::ValidateJwtRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->jwt = token;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CValidateJwtGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return imtauth::IJwtSessionController::JS_NONE;
	}

	typedef sdl::V1_0::imtauth::CValidateJwtPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return imtauth::IJwtSessionController::JS_NONE;
	}

	if (response.state.has_value()){
		sessionsdl::JwtState state = *response.state;

		if (state == sessionsdl::JwtState::EXPIRED){
			return imtauth::IJwtSessionController::JS_EXPIRED;
		}
		if (state == sessionsdl::JwtState::INVALID){
			return imtauth::IJwtSessionController::JS_INVALID;
		}
		if (state == sessionsdl::JwtState::OK){
			return imtauth::IJwtSessionController::JS_OK;
		}
	}

	return imtauth::IJwtSessionController::JS_NONE;
}


bool CRemoteJwtSessionControllerComp::RefreshToken(
			const QByteArray& refreshToken,
			imtauth::IJwtSessionController::UserSession& userSession) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::RefreshTokenRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->refreshToken = refreshToken;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CRefreshTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef sdl::V1_0::imtauth::CRefreshTokenPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.ok.has_value() && *response.ok){
		if (response.userSession.has_value()){
			if (response.userSession->userId.has_value()){
				userSession.userId = *response.userSession->userId;
			}

			if (response.userSession->tenantId.has_value()){
				userSession.tenantId = *response.userSession->tenantId;
			}

			if (response.userSession->accessToken.has_value()){
				userSession.accessToken = *response.userSession->accessToken;
			}

			if (response.userSession->refreshToken.has_value()){
				userSession.refreshToken = *response.userSession->refreshToken;
			}

			return true;
		}
	}

	return false;
}


bool CRemoteJwtSessionControllerComp::CreateNewSession(
			const QByteArray& userId,
			const QByteArray& tenantId,
			imtauth::IJwtSessionController::UserSession& userSession) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::CreateNewSessionRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->userId = userId;
	if (!tenantId.isEmpty()){
		arguments.input->tenantId = tenantId;
	}

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CCreateNewSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef sdl::V1_0::imtauth::CCreateNewSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.ok.has_value() && *response.ok){
		if (response.userSession.has_value()){
			if (response.userSession->userId.has_value()){
				userSession.userId = *response.userSession->userId;
			}

			if (response.userSession->tenantId.has_value()){
				userSession.tenantId = *response.userSession->tenantId;
			}

			if (response.userSession->accessToken.has_value()){
				userSession.accessToken = *response.userSession->accessToken;
			}

			if (response.userSession->refreshToken.has_value()){
				userSession.refreshToken = *response.userSession->refreshToken;
			}

			return true;
		}
	}

	return false;
}


imtauth::ISessionSharedPtr CRemoteJwtSessionControllerComp::GetSession(const QByteArray& sessionId) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::GetSessionRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->sessionId = sessionId;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CGetSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	typedef sdl::V1_0::imtauth::CGetSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	if (response.sessionData.has_value() && m_sessionFactCompPtr.IsValid()){
		QByteArray sessionData = *response.sessionData;

		imtauth::ISessionUniquePtr sessionInfoPtr = m_sessionFactCompPtr.CreateInstance();

		iser::CMemoryReadArchive archive(sessionData.data(), sessionData.length());
		if (!sessionInfoPtr->Serialize(archive)){
			SendErrorMessage(0, QString("Unable to deserialize session info. Error: Deserialization failed"));
			return nullptr;
		}

		return imtauth::ISessionSharedPtr::CreateFromUnique(sessionInfoPtr);
	}

	return nullptr;
}


bool CRemoteJwtSessionControllerComp::RemoveSession(const QByteArray& sessionId) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::RemoveSessionRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->sessionId = sessionId;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CRemoveSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef sdl::V1_0::imtauth::CRemoveSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.ok.has_value()){
		return *response.ok;
	}

	return false;
}


QByteArray CRemoteJwtSessionControllerComp::GetUserFromJwt(const QByteArray& jwt) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::GetUserFromJwtRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->jwt = jwt;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CGetUserFromJwtGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArray();
	}

	typedef sdl::V1_0::imtauth::CGetUserFromJwtPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QByteArray();
	}

	if (response.userId.has_value()){
		return *response.userId;
	}

	return QByteArray();
}


QByteArray CRemoteJwtSessionControllerComp::GetSessionFromJwt(const QByteArray& /*jwt*/) const
{
	return QByteArray();
}


QByteArray CRemoteJwtSessionControllerComp::GetTenantFromJwt(const QByteArray& jwt) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::GetTenantFromJwtRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->jwt = jwt;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CGetTenantFromJwtGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArray();
	}

	typedef sdl::V1_0::imtauth::CGetTenantFromJwtPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QByteArray();
	}

	if (response.tenantId.has_value()){
		return *response.tenantId;
	}

	return QByteArray();
}


} // namespace imtauthgql
