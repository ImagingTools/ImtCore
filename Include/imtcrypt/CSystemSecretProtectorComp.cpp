// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcrypt/CSystemSecretProtectorComp.h>


#ifdef Q_OS_WIN
// Windows includes
#include <windows.h>
#include <dpapi.h>

#pragma comment(lib, "crypt32.lib")
#endif


namespace imtcrypt
{


// public methods

// reimplemented (ISecretProtector)

bool CSystemSecretProtectorComp::Protect(const QByteArray& secret, QByteArray& protectedSecret) const
{
#ifdef Q_OS_WIN
	DATA_BLOB input;
	input.pbData = reinterpret_cast<BYTE*>(const_cast<char*>(secret.constData()));
	input.cbData = DWORD(secret.size());

	DATA_BLOB output = {};
	const DWORD flags = CRYPTPROTECT_UI_FORBIDDEN | (*m_isMachineScopeAttrPtr ? CRYPTPROTECT_LOCAL_MACHINE : 0);
	if (!CryptProtectData(&input, nullptr, nullptr, nullptr, nullptr, flags, &output)){
		return false;
	}

	protectedSecret = QByteArray(reinterpret_cast<const char*>(output.pbData), int(output.cbData));
	LocalFree(output.pbData);

	return true;
#else
	protectedSecret = secret;

	return true;
#endif
}


bool CSystemSecretProtectorComp::Unprotect(const QByteArray& protectedSecret, QByteArray& secret) const
{
#ifdef Q_OS_WIN
	DATA_BLOB input;
	input.pbData = reinterpret_cast<BYTE*>(const_cast<char*>(protectedSecret.constData()));
	input.cbData = DWORD(protectedSecret.size());

	DATA_BLOB output = {};
	if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &output)){
		return false;
	}

	secret = QByteArray(reinterpret_cast<const char*>(output.pbData), int(output.cbData));
	SecureZeroMemory(output.pbData, output.cbData);
	LocalFree(output.pbData);

	return true;
#else
	secret = protectedSecret;

	return true;
#endif
}


} // namespace imtcrypt


