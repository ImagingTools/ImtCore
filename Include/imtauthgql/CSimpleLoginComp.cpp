// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CSimpleLoginComp.h>


namespace imtauthgql
{


// public methods

// reimplemented (iauth::ILogin)

iauth::CUser* CSimpleLoginComp::GetLoggedUser() const
{
	if (!m_loggedUserId.isEmpty()){
		m_loggedUser.SetUserName(m_loggedUserId);
		m_loggedUser.SetPassword(m_loggedUserPassword);

		return &m_loggedUser;
	}

	return nullptr;
}


bool CSimpleLoginComp::Login(const QString& userName, const QString& password)
{
	istd::CChangeNotifier notifier(this);
	Q_UNUSED(notifier);

	m_loggedUserId = userName.toUtf8();
	m_loggedUserPassword = password.toUtf8();

	return true;
}


bool CSimpleLoginComp::Logout()
{
	if (!m_loggedUserId.isEmpty()){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_loggedUserId.clear();
		m_loggedUserPassword.clear();
		m_loggedUserToken.clear();
		m_userPermissionIds.clear();

		return true;
	}

	return false;
}


// reimplemented (iauth::IRightsProvider)

bool CSimpleLoginComp::HasRight(
	const QByteArray& operationId,
	bool /*beQuiet*/) const
{
	return m_userPermissionIds.contains(operationId);
}


// reimplemented (imtauth::IUserPermissionsController)

QByteArrayList CSimpleLoginComp::GetPermissions(const QByteArray& /*userId*/) const
{
	return m_userPermissionIds;
}


void CSimpleLoginComp::SetPermissions(const QByteArray& /*userId*/, const QByteArrayList& permissions)
{
	if (m_userPermissionIds != permissions){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_userPermissionIds = permissions;
	}
}

// reimplemented (imtauth::IAccessTokenProvider)

QByteArray CSimpleLoginComp::GetToken(const QByteArray& /*userId*/) const
{
	return m_loggedUserToken;
}


// reimplemented (imtauth::IAccessTokenController)

bool CSimpleLoginComp::SetToken(const QByteArray& /*userId*/, const QByteArray& accessToken)
{
	if (m_loggedUserToken != accessToken){
		m_loggedUserToken = accessToken;

		return true;
	}

	return false;
}


} // namespace imtauthgql


