// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcrypt/IHashGenerator.h>


namespace imtcrypt
{


class CMD5HashCalculator: virtual public IHashGenerator
{
public:
	// reimplemented (IHashGenerator)
	virtual QByteArray GenerateHash(const QByteArray& input) const override;
	virtual bool ValidateHash(const QByteArray& inputData, const QByteArray& hashValue) const override;
};


} //namespace imtcrypt


