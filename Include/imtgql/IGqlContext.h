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
	virtual QByteArray GetLanguageId() const = 0;
	virtual QByteArray GetDesignScheme() const = 0;
	virtual QUuid GetToken() const = 0;
	virtual imtauth::IUserInfo* GetUserInfo() const = 0;
};


} // namespace imtgql


