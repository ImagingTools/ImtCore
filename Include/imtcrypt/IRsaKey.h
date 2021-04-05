#pragma once

// Qt includes
#include <QtCore/QByteArray>


// ACF includes
#include <istd/IChangeable.h>


namespace imtcrypt
{

/**
	Interface descibing cryptography algoritms.
	\ingroup Crypter
*/
class IRsaKey: virtual public istd::IChangeable
{
public:

	virtual void GenerateRsaKeys() = 0;
	virtual QByteArray GetPrivateKey() const = 0;
	virtual QByteArray GetPublicKey() const = 0;
	virtual void SetPrivateKey(const QByteArray &key) = 0;
	virtual void SetPublicKey(const QByteArray &key) = 0;

};


} //namespace imtcrypt


