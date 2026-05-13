// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauth
{


class ILoginInfoProvider: virtual public istd::IChangeable
{
public:
	virtual QByteArray GetLoggedUserId() const = 0;
	virtual imtauth::IUserInfoUniquePtr GetLoggedUserInfo() const = 0;
};


} // namespace imtauth


