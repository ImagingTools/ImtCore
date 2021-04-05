#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtcrypt/IAesKey.h>
#include <imtcrypt/IRsaKey.h>


namespace imtcrypt
{


/**
	Interface descibing encryption/decryption of data.
	\ingroup Crypter
*/
class ICrypter: virtual public istd::IPolymorphic
{
public:
	virtual bool EncryptAes(QByteArray& data, const IAesKey& key) = 0;
	virtual bool DecryptAes(QByteArray& data, const IAesKey& key) = 0;
	virtual bool EncryptRsa(QByteArray& data, const IRsaKey& key) = 0;
	virtual bool DecryptRsa(QByteArray& data, const IRsaKey& key) = 0;
};


} //namespace imtcrypt


