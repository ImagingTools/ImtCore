// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtcrypt/IHashGenerator.h>
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
typedef icomp::TMakeComponentWrap<imtcrypt::CMD5HashCalculator, imtcrypt::IHashGenerator> HashCalculator;


} // namespace ImtCryptPck


