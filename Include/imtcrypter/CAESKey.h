#pragma once

// ACF includes
#include <imtcrypter/IAESKey.h>



namespace imtcrypter
{


/**
	Interface descibing cryptography algoritms.
	\ingroup Crypter
*/
class CAESKey: virtual public IAESKey
{
public:

	virtual void CreateAESKeys(const QByteArray &password, const QByteArray &ext) override;
	virtual QByteArray GetKey() const override;
	virtual QByteArray GetIV() const override;
	virtual void SetKey(const QByteArray &key) override;
	virtual void SetIV(const QByteArray &iv) override;

protected:

	QByteArray m_hashKey;
	QByteArray m_hashIV;
};


} //namespace imtcrypter


