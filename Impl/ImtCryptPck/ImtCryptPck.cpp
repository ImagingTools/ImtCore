// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtCryptPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtCryptPck
{


I_EXPORT_PACKAGE(
			"ImtCryptPck",
			"Cryptography component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			Encryption,
			"Data encryption",
			"Cryptography Encrypt Decrypt");

I_EXPORT_COMPONENT(
			EncryptionBasePersistence,
			"Persistence based on encrypted data",
			"Persistence Encryption Decryption")

I_EXPORT_COMPONENT(
			StaticEncryptionKeyProvider,
			"Static provider of the encryption keys",
			"Keys AES RSA Encryption Decryption");

I_EXPORT_COMPONENT(
			HashCalculator,
			"MD5 Hash Calculator",
			"MD5 Hash Calculator");

I_EXPORT_COMPONENT(
			SystemSecretProtector,
			"Protection of a secret at rest by the operating system",
			"Secret Protection DPAPI");

I_EXPORT_COMPONENT(
			ProtectedTextParam,
			"Secret text parameter stored protected by a secret protector, e.g. an API key in the server settings",
			"Secret Text Parameter Protected Password API Key");


} // namespace ImtCryptPck


