#pragma once

#include <QtCore/QByteArray>

// ACF includes
#include <istd/IChangeable.h>


namespace imtcrypter
{


/**
	Interface descibing cryptography algoritms.
	\ingroup Crypter
*/
class IAesKey: virtual public istd::IChangeable
{
public:

	virtual void CreateAesKeys(const QByteArray &password, const QByteArray &ext) = 0;
	virtual QByteArray GetKey() const = 0;
	virtual QByteArray GetIV() const = 0;
	virtual void SetKey(const QByteArray &key) = 0;
	virtual void SetIV(const QByteArray &iv) = 0;
};


} //namespace imtcrypter


