// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CLdapCredentialControllerComp.h>


// Windows includes
#ifdef Q_OS_WIN
#include <windows.h>
#include <LMaccess.h>
#include <Lmapibuf.h>
#pragma comment(lib, "netapi32.lib")
#endif


namespace imtauth
{


// protected methods

// reimplemented (imtauth::ICredentialController)

bool CLdapCredentialControllerComp::CheckCredential(const QByteArray& login, const QByteArray& password) const
{
	int result = 0;
#ifdef Q_OS_WIN
	// Local domain by default
	QByteArray domain = ".";
	QByteArray username = login;

	QByteArrayList data = login.split('\\');
	if (data.size() >= 2){
		domain = data[0];
		username = data[1];
	}

	HANDLE  hUser;
	result = LogonUser(qUtf16Printable(username), qUtf16Printable(domain), qUtf16Printable(password), LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hUser);
#endif

	return result > 0;
}


} // namespace imtauth


