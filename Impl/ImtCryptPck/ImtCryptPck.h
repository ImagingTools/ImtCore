#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtcrypt/CEncryption.h>
#include <imtcrypt/CEncryptionBasedPersistenceComp.h>
#include <imtcrypt/CStaticEncryptionKeyProviderComp.h>


namespace ImtCryptPck
{


typedef icomp::TMakeComponentWrap<
			imtcrypt::CEncryption,
			imtcrypt::IEncryption> Encryption;
typedef imtcrypt::CEncryptionBasedPersistenceComp EncryptionBasePersistence;
typedef icomp::TModelCompWrap<imtcrypt::CStaticEncryptionKeyProviderComp> StaticEncryptionKeyProvider;


} // namespace ImtCryptPck


