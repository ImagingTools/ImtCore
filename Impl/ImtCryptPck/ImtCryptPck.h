#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtcrypt/IHashCalculator.h>
#include <imtcrypt/CMD5HashCalculator.h>
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
typedef icomp::TMakeComponentWrap<imtcrypt::CMD5HashCalculator, imtcrypt::IHashCalculator> HashCalculator;


} // namespace ImtCryptPck


