// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IUserVerification.h>


namespace imtauth
{


class IUserVerificationController: virtual public istd::IPolymorphic
{
public:
	virtual const imtauth::IUserVerification* GetVerificationData(const QByteArray& verificationId) const = 0;
	virtual bool GenerateVerificationData(const QByteArray& userId, imtauth::IUserVerification* userVerificationPtr = nullptr) = 0;
	virtual bool VerifyUser(const QByteArray& userId, const QByteArray& verificationCode) const = 0;
};


} // namespace imtauth


