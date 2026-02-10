// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtauth
{


class ILoginInfo: virtual public istd::IChangeable
{
public:
	// TODO: Add GetLoggedUserId, GetLoggedUserName
	// TODO: Move to ILoginSettings
	virtual bool IsRememberUserEnabled() const = 0;
	virtual void EnableRememberUser(bool enable = true) = 0;

	// TODO: ILoginSession
	virtual QString GetLastUser() const = 0;
	virtual void SetLastUser(const QString& userName) = 0;
	virtual QByteArray GetRefreshToken() const = 0;
	virtual void SetRefreshToken(const QByteArray& refreshToken) = 0;
};


} // namespace imtauth


