#include "CAESKey.h"
#include <QCryptographicHash>
#include <QDebug>

namespace imtcrypter
{

	void CAESKey::CreateAESKeys(const QByteArray &password, const QByteArray &ext)
	{
		m_hashKey = QCryptographicHash::hash(password, QCryptographicHash::Sha256);
		m_hashIV = QCryptographicHash::hash(ext, QCryptographicHash::Md5);
	}

	QByteArray CAESKey::GetKey() const
	{
		return m_hashKey;
	}

	QByteArray CAESKey::GetIV() const
	{
		return m_hashIV;
	}

	void CAESKey::SetKey(const QByteArray &key)
	{
		m_hashKey = key;
	}

	void CAESKey::SetIV(const QByteArray &iv)
	{
		m_hashIV = iv;
	}


} //namespace imtcrypter

