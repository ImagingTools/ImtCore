#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtcrypt/CEncryption.h>
#include <imtcrypt/CEncryptionBasePersistenceComp.h>


namespace ImtCryptPck
{


typedef icomp::TMakeComponentWrap<
			imtcrypt::CEncryption,
			imtcrypt::IEncryption> Encryption;

typedef imtcrypt::CEncryptionBasePersistenceComp EncryptionBasePersistence;


} // namespace ImtCryptPck


