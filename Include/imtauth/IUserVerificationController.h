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
	/**
		Check the code sent to the user. The verification ends after a few wrong codes, so the code cannot be guessed.
	*/
	virtual bool VerifyUser(const QByteArray& userId, const QByteArray& verificationCode) const = 0;
	/**
		End the verification of the user, e.g. after its code was used.
	*/
	virtual bool RemoveVerificationData(const QByteArray& userId) = 0;
	/**
		Check the code and end the verification in one step: of concurrent callers with the same code only one succeeds.
	*/
	virtual bool ConsumeVerification(const QByteArray& userId, const QByteArray& verificationCode) = 0;
};


} // namespace imtauth


