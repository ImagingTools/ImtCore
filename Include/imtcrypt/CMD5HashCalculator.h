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
	bool ValidateHash(const QByteArray& inputData, const QByteArray& hashValue) const override;
};


} //namespace imtcrypt


