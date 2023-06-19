#pragma once


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtgql
{


/**
	Common interface for a GraphQL context.
*/
class IGqlContext: virtual public iser::IObject
{
public:
	virtual QByteArray GetProductId() const = 0;
	virtual void SetProductId(const QByteArray& productId) = 0;

	virtual QByteArray GetLanguageId() const = 0;
	virtual void SetLanguageId(const QByteArray& languageId) = 0;

	virtual QByteArray GetDesignScheme() const = 0;
	virtual void SetDesignScheme(const QByteArray& designScheme) = 0;

	virtual QByteArray GetToken() const = 0;
	virtual void SetToken(const QByteArray& token) = 0;

	virtual imtauth::IUserInfo* GetUserInfo() const = 0;
	virtual void SetUserInfo(const imtauth::IUserInfo* userInfoPtr) = 0;
};


} // namespace imtgql


