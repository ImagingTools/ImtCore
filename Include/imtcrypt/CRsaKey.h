#pragma once


// Qt includes
#include <QtCore/QByteArray>


namespace imtcrypt
{


class CRsaKey
{
public:
	void GenerateRsaKeys();
	QByteArray GetPrivateKey() const;
	QByteArray GetPublicKey() const;
	void SetPrivateKey(const QByteArray &key);
	void SetPublicKey(const QByteArray &key);

protected:
	QByteArray m_privateKey;
	QByteArray m_publicKey;
};


} //namespace imtcrypt


