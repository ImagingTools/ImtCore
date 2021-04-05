#pragma once


// ImtCore includes
#include <imtcrypt/IEncryption.h>


namespace imtcrypt
{


class CEncryption: virtual public IEncryption
{
public:
	// reimplemented (IEncryption)
	virtual bool EncryptData(
				const QByteArray& input,
				EncryptionAlgorithm algorithm,
				const IEncryptionKeysProvider& keys,
				QByteArray& output) const override;
	virtual bool DecryptData(
				const QByteArray& input,
				EncryptionAlgorithm algorithm,
				const IEncryptionKeysProvider& keys,
				QByteArray& output) const  override;
};


} //namespace imtcrypt

