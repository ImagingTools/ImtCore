#pragma once


// ImtCore includes
#include <imtcrypt/IHashCalculator.h>


namespace imtcrypt
{


class CMD5HashCalculator: virtual public IHashCalculator
{
public:
	// reimplemented (IHashCalculator)
	virtual QByteArray Calculate(const QByteArray& input) const override;
};


} //namespace imtcrypt


