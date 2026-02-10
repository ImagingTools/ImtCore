// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauth
{


/**
	Interface for describing an session.
	\ingroup Session
*/
class ISession:
		virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_TOKEN = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_USER_ID
	};

	/**
		Get token of the session.
	*/
	virtual QByteArray GetToken() const = 0;

	/**
		Set token of the session.
	*/
	virtual void SetToken(const QByteArray& token) = 0;

	/**
		Get user-ID of the session.
	*/
	virtual QByteArray GetUserId() const = 0;

	/**
		Set user info of the session.
	*/
	virtual void SetUserId(const QByteArray& userId) = 0;

	/**
		Get session creation date.
	*/
	virtual QDateTime GetCreationDate() const = 0;

	/**
		Set session creation date.
	*/
	virtual void SetCreationDate(const QDateTime& creationDate) = 0;

	/**
		Get session expiration date.
	*/
	virtual QDateTime GetExpirationDate() const = 0;

	/**
		Set session expiration date.
	*/
	virtual void SetExpirationDate(const QDateTime& expirationDate) = 0;
};


typedef istd::TUniqueInterfacePtr<ISession> ISessionUniquePtr;
typedef istd::TSharedInterfacePtr<ISession> ISessionSharedPtr;


} // namespace imtauth


