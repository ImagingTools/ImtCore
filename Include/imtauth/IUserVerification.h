// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtauth
{


class IUserVerification: virtual public iser::ISerializable
{
public:
	virtual QByteArray GetUserId() const = 0;
	virtual void SetUserId(const QByteArray& userId) = 0;

	virtual QByteArray GetVerificationCode() const = 0;
	virtual void SetVerificationCode(const QByteArray& code) = 0;

	virtual QDateTime GetCreationDate() const = 0;
	virtual void SetCreationDate(const QDateTime& creationDate) = 0;

	virtual QDateTime GetExpirationDate() const = 0;
	virtual void SetExpirationDate(const QDateTime& expirationDate) = 0;

	virtual bool IsExpired() const = 0;
};


} // namespace imtauth
