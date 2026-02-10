// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauth
{


class IUserVerificationCodeSender: virtual public istd::IPolymorphic
{
public:
	virtual bool SendCode(const IUserInfo& userInfo, const QByteArray& verificationCode) const = 0;
};


} // namespace imtauth


