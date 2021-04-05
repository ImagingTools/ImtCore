#pragma once


// ImtCore includes
#include <imtcrypter/IRsaKey.h>


namespace imtcrypter
{


class CRsaKey: virtual public IRsaKey
{
public:
	virtual void GenerateRsaKeys() override;
	virtual QByteArray GetPrivateKey() const override;
	virtual QByteArray GetPublicKey() const override;
	virtual void SetPrivateKey(const QByteArray &key) override;
	virtual void SetPublicKey(const QByteArray &key) override;

protected:
	QByteArray m_privateKey;
	QByteArray m_publicKey;
};


} //namespace imtcrypter


