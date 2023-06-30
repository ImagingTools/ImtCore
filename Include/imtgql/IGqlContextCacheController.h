#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtgql
{

/**
	\todo Move to imtrest. This is a general interface, rename it to IClientContextCacheController
*/
class IGqlContextCacheController: virtual public istd::IPolymorphic
{
public:
	virtual bool TokenIsCached(const QByteArray& token) const = 0;
	virtual bool CacheToken(const QByteArray& token) = 0;
	virtual bool RemoveCache(const QByteArray& token) = 0;
	virtual const imtauth::IUserInfo* GetUserInfo(const QByteArray& token) const = 0;
};


} // namespace imtgql


