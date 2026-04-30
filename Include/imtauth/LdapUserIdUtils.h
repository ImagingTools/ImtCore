// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QByteArray>
#include <QString>
#include <QtGlobal>

// Windows includes
#ifdef Q_OS_WIN
#include <windows.h>
#endif


namespace imtauth
{


namespace LdapUserIdUtils
{


inline bool IsLocalDomain(const QByteArray& domain)
{
#ifdef Q_OS_WIN
	QByteArray normalizedDomain = domain.trimmed();
	if (normalizedDomain.isEmpty() || normalizedDomain == "."){
		return true;
	}

	if (normalizedDomain.compare("localhost", Qt::CaseInsensitive) == 0){
		return true;
	}

	WCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD computerNameSize = MAX_COMPUTERNAME_LENGTH + 1;
	if (GetComputerNameW(computerName, &computerNameSize)){
		QByteArray localComputerName = QString::fromWCharArray(computerName).toUtf8();
		return normalizedDomain.compare(localComputerName, Qt::CaseInsensitive) == 0;
	}
#else
	Q_UNUSED(domain)
#endif

	return false;
}


inline QByteArray NormalizeUserId(const QByteArray& userId)
{
#ifdef Q_OS_WIN
	int separatorIndex = userId.indexOf('\\');
	if (separatorIndex < 0){
		return userId;
	}

	QByteArray domain = userId.left(separatorIndex);
	QByteArray username = userId.mid(separatorIndex + 1);
	if (username.isEmpty()){
		return userId;
	}

	if (IsLocalDomain(domain)){
		return username;
	}
#endif

	return userId;
}


inline void SplitUserId(const QByteArray& userId, QByteArray& domain, QByteArray& username)
{
	domain = ".";
	username = userId;

	int separatorIndex = userId.indexOf('\\');
	if (separatorIndex >= 0){
		QByteArray prefix = userId.left(separatorIndex);
		QByteArray suffix = userId.mid(separatorIndex + 1);
		if (suffix.isEmpty()){
			return;
		}

		if (IsLocalDomain(prefix)){
			username = suffix;
		}
		else{
			domain = prefix;
			username = suffix;
		}
	}
}


} // namespace LdapUserIdUtils


} // namespace imtauth
