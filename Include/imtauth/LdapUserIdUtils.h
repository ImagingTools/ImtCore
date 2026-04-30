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


inline QByteArray TrimmedDomainName(const QByteArray& domain)
{
	QByteArray normalizedDomain = domain.trimmed();
	while (normalizedDomain.endsWith('.')){
		normalizedDomain.chop(1);
	}

	return normalizedDomain;
}


inline QByteArray NormalizedDomainName(const QByteArray& domain)
{
	QByteArray normalizedDomain = TrimmedDomainName(domain);
	if (normalizedDomain.contains('.')){
		return normalizedDomain.toLower();
	}

	return normalizedDomain.toUpper();
}


inline bool IsLocalDomain(const QByteArray& domain)
{
#ifdef Q_OS_WIN
	QByteArray normalizedDomain = TrimmedDomainName(domain);
	if (normalizedDomain.isEmpty()){
		return true;
	}

	if (normalizedDomain.compare("localhost", Qt::CaseInsensitive) == 0){
		return true;
	}

	WCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1] = {};
	DWORD computerNameSize = MAX_COMPUTERNAME_LENGTH + 1;
	if (GetComputerNameW(computerName, &computerNameSize)){
		QByteArray localComputerName = QString::fromWCharArray(computerName).toUtf8();
		if (normalizedDomain.compare(localComputerName, Qt::CaseInsensitive) == 0){
			return true;
		}
	}

	WCHAR dnsHostName[256] = {};
	DWORD dnsHostNameSize = 256;
	if (GetComputerNameExW(ComputerNameDnsHostname, dnsHostName, &dnsHostNameSize)){
		QByteArray localDnsHostName = QString::fromWCharArray(dnsHostName).toUtf8();
		if (normalizedDomain.compare(localDnsHostName, Qt::CaseInsensitive) == 0){
			return true;
		}
	}

	WCHAR dnsFullName[256] = {};
	DWORD dnsFullNameSize = 256;
	if (GetComputerNameExW(ComputerNameDnsFullyQualified, dnsFullName, &dnsFullNameSize)){
		QByteArray localDnsFullName = QString::fromWCharArray(dnsFullName).toUtf8();
		if (normalizedDomain.compare(localDnsFullName, Qt::CaseInsensitive) == 0){
			return true;
		}
	}
#else
	Q_UNUSED(domain)
#endif

	return false;
}


inline bool SplitRawDomainUserId(const QByteArray& userId, QByteArray& domain, QByteArray& username)
{
	int separatorIndex = userId.indexOf('\\');
	if (separatorIndex < 0){
		return false;
	}

	domain = userId.left(separatorIndex);
	username = userId.mid(separatorIndex + 1);

	// Preserve malformed values like "domain\\" so authentication fails for the original login instead of an empty username.
	return !username.isEmpty();
}


inline bool SplitRawPrincipalUserId(const QByteArray& userId, QByteArray& domain, QByteArray& username)
{
	int separatorIndex = userId.lastIndexOf('@');
	if (separatorIndex <= 0 || separatorIndex == userId.size() - 1){
		return false;
	}

	username = userId.left(separatorIndex);
	domain = userId.mid(separatorIndex + 1);

	return true;
}


inline QByteArray JoinDomainUserId(const QByteArray& domain, const QByteArray& username)
{
	return NormalizedDomainName(domain) + "\\" + username;
}


inline QByteArray NormalizeUserId(const QByteArray& userId)
{
#ifdef Q_OS_WIN
	QByteArray domain;
	QByteArray username;
	if (SplitRawDomainUserId(userId, domain, username)){
		if (IsLocalDomain(domain)){
			return username;
		}

		return JoinDomainUserId(domain, username);
	}

	if (SplitRawPrincipalUserId(userId, domain, username)){
		if (IsLocalDomain(domain)){
			return username;
		}

		return JoinDomainUserId(domain, username);
	}
#endif

	return userId;
}


inline void SplitUserId(const QByteArray& userId, QByteArray& domain, QByteArray& username)
{
	domain = ".";
	username = userId;

	QByteArray prefix;
	QByteArray suffix;
	if (SplitRawDomainUserId(userId, prefix, suffix) || SplitRawPrincipalUserId(userId, prefix, suffix)){
		if (IsLocalDomain(prefix)){
			username = suffix;
		}
		else{
			domain = NormalizedDomainName(prefix);
			username = suffix;
		}
	}
}


} // namespace LdapUserIdUtils

} // namespace imtauth
