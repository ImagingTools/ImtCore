// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtgql
{


/**
	Common interface for a GraphQL context.
	\todo Move to imtrest. This is a general interface, rename it to IClientRequestContext
*/
class IGqlContext: virtual public iser::IObject
{
public:
	typedef QMap<QByteArray,QByteArray> Headers;

	virtual QByteArray GetProductId() const = 0;
	virtual void SetProductId(const QByteArray& productId) = 0;

	virtual QByteArray GetLanguageId() const = 0;
	virtual void SetLanguageId(const QByteArray& languageId) = 0;

	virtual QByteArray GetDesignScheme() const = 0;
	virtual void SetDesignScheme(const QByteArray& designScheme) = 0;

	virtual QByteArray GetToken() const = 0;
	virtual void SetToken(const QByteArray& token) = 0;

	virtual QByteArray GetUserId() const = 0;
	virtual void SetUserId(const QByteArray& userId) = 0;

	virtual const imtauth::IUserInfo* GetUserInfo() const = 0;
	virtual void SetUserInfo(const imtauth::IUserInfo* userInfoPtr) = 0;

	virtual Headers GetHeaders() const = 0;
	virtual void SetHeaders(const Headers& headers) = 0;
};


typedef istd::TUniqueInterfacePtr<IGqlContext> IGqlContextUniquePtr;
typedef istd::TSharedInterfacePtr<IGqlContext> IGqlContextSharedPtr;


} // namespace imtgql


