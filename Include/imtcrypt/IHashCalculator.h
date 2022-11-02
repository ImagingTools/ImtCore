#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcrypt
{


/**
	Interface describing hash calculating of data.
	\ingroup Cryptography
*/
class IHashCalculator: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray Calculate(const QByteArray& input) const = 0;
};


} //namespace imtcrypt


