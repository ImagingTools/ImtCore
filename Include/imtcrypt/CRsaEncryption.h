// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcrypt/CRsaKey.h>


namespace imtcrypt
{


class CRsaEncryption
{
public:
	bool Encrypt(const QByteArray& inputData, const CRsaKey& key, QByteArray& outputData) const;
	bool Decrypt(const QByteArray& inputData, const CRsaKey& key, QByteArray& outputData) const;
};


} //namespace imtcrypt


