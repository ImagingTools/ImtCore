#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtcrypter/IAESKey.h>
#include <imtcrypter/IRSAKey.h>


namespace imtcrypter
{


/**
	Interface descibing cryptography algoritms.
	\ingroup Crypter
*/
class ICrypter: virtual public istd::IPolymorphic
{
public:
	virtual bool EncryptAES(QByteArray &data, const IAESKey &key) = 0;
	virtual bool DecryptAES(QByteArray &data, const IAESKey &key) = 0;

	virtual bool EncryptRSA(QByteArray &data, const IRSAKey &key) = 0;
	virtual bool DecryptRSA(QByteArray &data, const IRSAKey &key) = 0;
};


} //namespace imtcrypter


