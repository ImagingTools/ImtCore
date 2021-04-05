#include <imtcrypter/CAesKey.h>


// Qt includes
#include <QtCore/QCryptographicHash>


namespace imtcrypter
{


// reimplemented (IAesKey)

void CAesKey::CreateAesKeys(const QByteArray &password, const QByteArray &ext)
{
	m_hashKey = QCryptographicHash::hash(password, QCryptographicHash::Sha256);
	m_hashIV = QCryptographicHash::hash(ext, QCryptographicHash::Md5);
}


QByteArray CAesKey::GetKey() const
{
	return m_hashKey;
}


QByteArray CAesKey::GetIV() const
{
	return m_hashIV;
}


void CAesKey::SetKey(const QByteArray &key)
{
	m_hashKey = key;
}


void CAesKey::SetIV(const QByteArray &iv)
{
	m_hashIV = iv;
}


} //namespace imtcrypter


