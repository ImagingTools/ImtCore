// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcrypt/CEncryption.h>


// ImtCore includes
#include <imtcrypt/IEncryptionKeysProvider.h>
#include <imtcrypt/CRsaEncryption.h>
#include <imtcrypt/CAesEncryption.h>


namespace imtcrypt
{


// reimplemented (IEncryption)

bool CEncryption::EncryptData(
			const QByteArray& input,
			EncryptionAlgorithm algorithm,
			const IEncryptionKeysProvider& keys,
			QByteArray& output) const
{
	switch (algorithm){
		case EA_AES:{
			CAesKey key;
			key.CreateAesKeys(
						keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PASSWORD),
						keys.GetEncryptionKey(IEncryptionKeysProvider::KT_INIT_VECTOR));

			CAesEncryption aes;
			return aes.Encrypt(input, key, output);
		}

		case EA_RSA:{
			CRsaKey key;
			key.SetPublicKey(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PUBLIC));
			key.SetPrivateKey(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PRIVATE));
		
			CRsaEncryption rsa;
			return rsa.Encrypt(input, key, output);
		}
	}

	return false;
}


bool CEncryption::DecryptData(
			const QByteArray& input,
			EncryptionAlgorithm algorithm,
			const IEncryptionKeysProvider& keys,
			QByteArray& output) const
{
	switch (algorithm){
		case EA_AES:{
			CAesKey key;
			key.CreateAesKeys(
						keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PASSWORD),
						keys.GetEncryptionKey(IEncryptionKeysProvider::KT_INIT_VECTOR));
		
			CAesEncryption aes;
			return aes.Decrypt(input, key, output);
		}

		case EA_RSA:{
			CRsaKey key;
			key.SetPublicKey(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PUBLIC));
			key.SetPrivateKey(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PRIVATE));

			CRsaEncryption rsa;
			return rsa.Decrypt(input, key, output);
		}
	}

	return false;
}


} //namespace imtcrypt


