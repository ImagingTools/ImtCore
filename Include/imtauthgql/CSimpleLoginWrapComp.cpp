// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CSimpleLoginWrapComp.h>


// ACF includes
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlContext.h>
#include <imtqml/CGqlModel.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Sessions.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtauthgql
{


// public methods

CSimpleLoginWrapComp::CSimpleLoginWrapComp()
{
}


// reimplemented (iauth::ILogin)

iauth::CUser* CSimpleLoginWrapComp::GetLoggedUser() const
{
	if (!m_loggedUserName.isEmpty() && m_userInfoPtr.IsValid()){
		m_loggedUser.SetUserName(m_loggedUserName);
		m_loggedUser.SetPassword(m_loggedUserPassword);

		return &m_loggedUser;
	}

	return nullptr;
}


bool CSimpleLoginWrapComp::Login(const QString& userName, const QString& password)
{
	if (!m_applicationInfoCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ApplicationInfo' was not set", "CSimpleLoginWrapComp");
		return false;
	}

	if (!m_loggedUserToken.isEmpty()){
		return false;
	}

	namespace authsdl = sdl::V1_0::imtauth;

	QByteArray productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	authsdl::AuthorizationRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->login = QString(userName);
	arguments.input->password = QString(password);
	arguments.input->productId = QByteArray(productId);

	imtgql::CGqlRequest gqlRequest;
	if (authsdl::CAuthorizationGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		typedef authsdl::CAuthorizationPayload Response;

		QString errorMessage;
		Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			return false;
		}

		m_loggedUserPassword = password.toUtf8();

		if (response.userId){
			m_loggedUserId = *response.userId;
		}

		m_userInfoPtr.FromUnique(m_userInfoFactCompPtr.CreateInstance());
		if (!m_userInfoPtr.IsValid()){
			return false;
		}

		if (response.username){
			m_userInfoPtr->SetId(*response.username);
		}
		if (response.permissions){
			QByteArray permissions = *response.permissions;
			if (!permissions.isEmpty()){
				m_userInfoPtr->SetLocalPermissions(productId, response.permissions->split(';'));
			}
		}

		m_userPermissionIds = m_userInfoPtr->GetLocalPermissions(productId);

		if (response.token){
			m_loggedUserToken = *response.token;
		}
		if (response.refreshToken){
			m_loggedUserRefreshToken = *response.refreshToken;
		}
		imtqml::CGqlModel::SetGlobalAccessToken(m_loggedUserToken);

		m_gqlContextSharedPtr.Reset();
		m_gqlContextSharedPtr = new imtgql::CGqlContext();
		m_gqlContextSharedPtr->SetToken(m_loggedUserToken);
		m_gqlContextSharedPtr->SetProductId(productId);

		imtgql::CGqlRequestContextManager::SetContext(m_gqlContextSharedPtr.GetPtr());

		if (response.username){
			istd::CChangeNotifier notifier(this);
			Q_UNUSED(notifier);

			m_loggedUserName = *response.username;
		}

		return true;
	}

	return false;
}


bool CSimpleLoginWrapComp::Logout()
{
	if (!m_loggedUserName.isEmpty()){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_loggedUserName.clear();
		m_loggedUserToken.clear();
		m_loggedUserPassword.clear();
		m_loggedUserRefreshToken.clear();
		m_userPermissionIds.clear();
		m_userInfoPtr.Reset();
		m_gqlContextSharedPtr.Reset();
		imtgql::CGqlRequestContextManager::SetContext(nullptr);

		return true;
	}

	return false;
}


bool CSimpleLoginWrapComp::LoginWithRefreshToken(const QString& userName, const QByteArray& refreshToken)
{
	if (!m_applicationInfoCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ApplicationInfo' was not set", "CSimpleLoginWrapComp");
		return false;
	}

	if (!m_loggedUserToken.isEmpty()){
		return false;
	}

	// Use Sessions SDL schema to refresh token
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::RefreshTokenRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->refreshToken = refreshToken;

	imtgql::CGqlRequest gqlRequest;
	if (sessionsdl::CRefreshTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		typedef sessionsdl::CRefreshTokenPayload Response;

		QString errorMessage;
		Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			return false;
		}

		if (!response || !response.ok.has_value() || !*response.ok){
			return false;
		}

		if (!response.userSession.has_value()){
			return false;
		}

		QByteArray productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();

		// Set up user session with refreshed tokens
		m_userInfoPtr.FromUnique(m_userInfoFactCompPtr.CreateInstance());
		if (!m_userInfoPtr.IsValid()){
			return false;
		}

		// Extract session data from response
		QByteArray userId;
		if (response.userSession->userId.has_value()){
			userId = *response.userSession->userId;
			m_loggedUserName = userId;
		}
		else{
			// Fallback to username if userId not in response.
			// This can occur when refreshing an older session token that
			// was created before userId was added to the response.
			// In such cases, the username parameter provides the login identifier.
			m_loggedUserName = userName.toUtf8();
		}

		m_userInfoPtr->SetId(m_loggedUserName);

		if (response.userSession->accessToken.has_value()){
			m_loggedUserToken = *response.userSession->accessToken;
		}
		if (response.userSession->refreshToken.has_value()){
			m_loggedUserRefreshToken = *response.userSession->refreshToken;
		}

		imtqml::CGqlModel::SetGlobalAccessToken(m_loggedUserToken);

		m_gqlContextSharedPtr.Reset();
		m_gqlContextSharedPtr = new imtgql::CGqlContext();
		m_gqlContextSharedPtr->SetToken(m_loggedUserToken);
		m_gqlContextSharedPtr->SetProductId(productId);

		imtgql::CGqlRequestContextManager::SetContext(m_gqlContextSharedPtr.GetPtr());

		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		return true;
	}

	return false;
}


QByteArray CSimpleLoginWrapComp::GetRefreshToken() const
{
	return m_loggedUserRefreshToken;
}


// reimplemented (iauth::IRightsProvider)

bool CSimpleLoginWrapComp::HasRight(
	const QByteArray& operationId,
	bool /*beQuiet*/) const
{
	if (m_userInfoPtr.IsValid()){
		bool isAdmin = m_userInfoPtr->IsAdmin();
		if (isAdmin){
			return true;
		}
	}

	if (!m_checkPermissionCompPtr.IsValid()){
		return false;
	}

	if (m_userPermissionIds.isEmpty()){
		return false;
	}

	bool retVal = m_checkPermissionCompPtr->CheckPermission(m_userPermissionIds, QByteArrayList() << operationId);

	return retVal;
}


// reimplemented (imtauth::IAccessTokenProvider)

QByteArray CSimpleLoginWrapComp::GetToken(const QByteArray& /*userId*/) const
{
	return m_loggedUserToken;
}


// reimplemented (imtauth::IUserPermissionsController)

QByteArrayList CSimpleLoginWrapComp::GetPermissions(const QByteArray& /*userId*/) const
{
	return m_userPermissionIds;
}


void CSimpleLoginWrapComp::SetPermissions(const QByteArray& /*userId*/, const QByteArrayList& permissions)
{
	if (m_userPermissionIds != permissions){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_userPermissionIds = permissions;
	}
}


// reimplemented (imtauth::ILoginInfoProvider)

QByteArray CSimpleLoginWrapComp::GetLoggedUserId() const
{
	return m_loggedUserId;
}


imtauth::IUserInfoUniquePtr CSimpleLoginWrapComp::GetLoggedUserInfo() const
{
	if (!m_userInfoPtr.IsValid()){
		return nullptr;
	}

	imtauth::IUserInfoUniquePtr retVal;

	retVal.MoveCastedPtr(m_userInfoPtr->CloneMe());

	return retVal;
}


} // namespace imtauthgql


