// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IAccessTokenProvider.h>


namespace imtauth
{


class IAccessTokenController: virtual public imtauth::IAccessTokenProvider
{
public:
	virtual bool SetToken(const QByteArray& userId, const QByteArray& accessToken) = 0;
};


} // namespace imtauth


