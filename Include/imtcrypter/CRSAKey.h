#pragma once

#include "openssl/rsa.h"

// ACF includes
#include <imtcrypter/IRSAKey.h>


namespace imtcrypter
{

/**
	Interface descibing cryptography algoritms.
	\ingroup Crypter
*/
class CRSAKey: virtual public IRSAKey
{
public:
	virtual void GenerateRSAKeys() override;
	virtual QByteArray GetPrivateKey() const override;
	virtual QByteArray GetPublicKey() const override;
	virtual void SetPrivateKey(const QByteArray &key) override;
	virtual void SetPublicKey(const QByteArray &key) override;
protected:
	QByteArray m_privateKey;
	QByteArray m_publicKey;
};


} //namespace imtcrypter


