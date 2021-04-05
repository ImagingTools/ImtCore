#pragma once


// ImtCore includes
#include <imtcrypter/IAesKey.h>


namespace imtcrypter
{


class CAesKey: virtual public IAesKey
{
public:
	// reimplemented (IAesKey)
	virtual void CreateAesKeys(const QByteArray &password, const QByteArray &ext) override;
	virtual QByteArray GetKey() const override;
	virtual QByteArray GetIV() const override;
	virtual void SetKey(const QByteArray &key) override;
	virtual void SetIV(const QByteArray &iv) override;

protected:
	QByteArray m_hashKey;
	QByteArray m_hashIV;
};


} //namespace imtcrypter


