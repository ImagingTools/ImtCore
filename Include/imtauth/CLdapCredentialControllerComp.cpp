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

	if (result == 0){
		DWORD errorCode = GetLastError();
		LPWSTR messageBuffer = nullptr;
		DWORD size = FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&messageBuffer,
			0,
			NULL);

		QString errorMessage = QString("LogonUser failed for user '%1' in domain '%2', error code: %3")
			.arg(QString::fromUtf8(username), QString::fromUtf8(domain), QString::number(errorCode));

		if (size > 0 && messageBuffer != nullptr){
			errorMessage += QString(" - %1").arg(QString::fromWCharArray(messageBuffer).trimmed());
		}

		if (messageBuffer != nullptr){
			LocalFree(messageBuffer);
		}

		SendLogMessage(
			istd::IInformationProvider::IC_ERROR,
			0,
			errorMessage,
			"CLdapCredentialControllerComp");
	}
	else {
		CloseHandle(hUser);
	}
#endif

	return result > 0;
}


} // namespace imtauth


