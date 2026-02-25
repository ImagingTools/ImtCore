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
	namespace sessionsdl = sdl::imtauth::Sessions;

	sessionsdl::ValidateSessionRequestArguments arguments;
	arguments.input.Version_1_0 = sessionsdl::CValidateSessionInput::V1_0();
	arguments.input.Version_1_0->sessionId = sessionId;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CValidateSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef sdl::imtauth::Sessions::CValidateSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.Version_1_0 && response.Version_1_0->isValid.has_value()){
		return *response.Version_1_0->isValid;
	}

	return false;
}


imtauth::IJwtSessionController::JwtState CRemoteJwtSessionControllerComp::ValidateJwt(const QByteArray& token) const
{
	namespace sessionsdl = sdl::imtauth::Sessions;

	sessionsdl::ValidateJwtRequestArguments arguments;
	arguments.input.Version_1_0 = sessionsdl::CValidateJwtInput::V1_0();
	arguments.input.Version_1_0->jwt = token;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CValidateJwtGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return imtauth::IJwtSessionController::JS_NONE;
	}

	typedef sdl::imtauth::Sessions::CValidateJwtPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return imtauth::IJwtSessionController::JS_NONE;
	}

	if (response.Version_1_0 && response.Version_1_0->state.has_value()){
		sessionsdl::JwtState state = *response.Version_1_0->state;

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
	namespace sessionsdl = sdl::imtauth::Sessions;

	sessionsdl::RefreshTokenRequestArguments arguments;
	arguments.input.Version_1_0 = sessionsdl::CRefreshTokenInput::V1_0();
	arguments.input.Version_1_0->refreshToken = refreshToken;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CRefreshTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef sdl::imtauth::Sessions::CRefreshTokenPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.Version_1_0 && response.Version_1_0->ok.has_value() && *response.Version_1_0->ok){
		if (response.Version_1_0->userSession.has_value()){
			if (response.Version_1_0->userSession->userId.has_value()){
				userSession.userId = *response.Version_1_0->userSession->userId;
			}

			if (response.Version_1_0->userSession->accessToken.has_value()){
				userSession.accessToken = *response.Version_1_0->userSession->accessToken;
			}

			if (response.Version_1_0->userSession->refreshToken.has_value()){
				userSession.refreshToken = *response.Version_1_0->userSession->refreshToken;
			}

			return true;
		}
	}

	return false;
}


bool CRemoteJwtSessionControllerComp::CreateNewSession(
			const QByteArray& userId,
			imtauth::IJwtSessionController::UserSession& userSession) const
{
	namespace sessionsdl = sdl::imtauth::Sessions;

	sessionsdl::CreateNewSessionRequestArguments arguments;
	arguments.input.Version_1_0 = sessionsdl::CCreateNewSessionInput::V1_0();
	arguments.input.Version_1_0->userId = userId;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CCreateNewSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef sdl::imtauth::Sessions::CCreateNewSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.Version_1_0 && response.Version_1_0->ok.has_value() && *response.Version_1_0->ok){
		if (response.Version_1_0->userSession.has_value()){
			if (response.Version_1_0->userSession->userId.has_value()){
				userSession.userId = *response.Version_1_0->userSession->userId;
			}

			if (response.Version_1_0->userSession->accessToken.has_value()){
				userSession.accessToken = *response.Version_1_0->userSession->accessToken;
			}

			if (response.Version_1_0->userSession->refreshToken.has_value()){
				userSession.refreshToken = *response.Version_1_0->userSession->refreshToken;
			}

			return true;
		}
	}

	return false;
}


imtauth::ISessionSharedPtr CRemoteJwtSessionControllerComp::GetSession(const QByteArray& sessionId) const
{
	namespace sessionsdl = sdl::imtauth::Sessions;

	sessionsdl::GetSessionRequestArguments arguments;
	arguments.input.Version_1_0 = sessionsdl::CGetSessionInput::V1_0();
	arguments.input.Version_1_0->sessionId = sessionId;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CGetSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	typedef sdl::imtauth::Sessions::CGetSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	if (response.Version_1_0 && response.Version_1_0->sessionData.has_value() && m_sessionFactCompPtr.IsValid()){
		QByteArray sessionData = *response.Version_1_0->sessionData;

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
	namespace sessionsdl = sdl::imtauth::Sessions;

	sessionsdl::RemoveSessionRequestArguments arguments;
	arguments.input.Version_1_0 = sessionsdl::CRemoveSessionInput::V1_0();
	arguments.input.Version_1_0->sessionId = sessionId;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CRemoveSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef sdl::imtauth::Sessions::CRemoveSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.Version_1_0 && response.Version_1_0->ok.has_value()){
		return *response.Version_1_0->ok;
	}

	return false;
}


QByteArray CRemoteJwtSessionControllerComp::GetUserFromJwt(const QByteArray& jwt) const
{
	namespace sessionsdl = sdl::imtauth::Sessions;

	sessionsdl::GetUserFromJwtRequestArguments arguments;
	arguments.input.Version_1_0 = sessionsdl::CGetUserFromJwtInput::V1_0();
	arguments.input.Version_1_0->jwt = jwt;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CGetUserFromJwtGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArray();
	}

	typedef sdl::imtauth::Sessions::CGetUserFromJwtPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QByteArray();
	}

	if (response.Version_1_0 && response.Version_1_0->userId.has_value()){
		return *response.Version_1_0->userId;
	}

	return QByteArray();
}


QByteArray CRemoteJwtSessionControllerComp::GetSessionFromJwt(const QByteArray& /*jwt*/) const
{
	return QByteArray();
}


} // namespace imtauthgql
