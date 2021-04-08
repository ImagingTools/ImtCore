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
			CAesEncryption aes;
			CAesKey key;
			key.CreateAesKeys(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PASSWORD),keys.GetEncryptionKey(IEncryptionKeysProvider::KT_VECTOR));
//			key.SetKey(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PASSWORD));
		
			return aes.Encrypt(input, key, output);
		}

		case EA_RSA:{
			CRsaEncryption rsa;
			CRsaKey key;
			key.SetPublicKey(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PUBLIC));
			key.SetPrivateKey(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PRIVATE));
		
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
			CAesEncryption aes;
			CAesKey key;
			key.CreateAesKeys(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PASSWORD),keys.GetEncryptionKey(IEncryptionKeysProvider::KT_VECTOR));
//			key.SetKey(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PASSWORD));
		
			return aes.Decrypt(input, key, output);
		}

		case EA_RSA:{
			CRsaEncryption rsa;
			CRsaKey key;
			key.SetPublicKey(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PUBLIC));
			key.SetPrivateKey(keys.GetEncryptionKey(IEncryptionKeysProvider::KT_PRIVATE));
		
			return rsa.Decrypt(input, key, output);
		}
	}

	return false;
}


} //namespace imtcrypt

