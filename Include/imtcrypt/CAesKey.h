#pragma once


// Qt includes
#include <QtCore/QByteArray>


namespace imtcrypt
{


class CAesKey
{
public:
	void CreateAesKeys(const QByteArray &password, const QByteArray &ext);
	QByteArray GetKey() const;
	QByteArray GetIV() const;
	void SetKey(const QByteArray &key);
	void SetIV(const QByteArray &iv);

protected:
	QByteArray m_hashKey;
	QByteArray m_hashIV;
};


} //namespace imtcrypt


