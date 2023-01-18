#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcrypt
{


/**
	Interface describing the hash generation of data.
	\ingroup Cryptography
*/
class IHashGenerator: virtual public istd::IPolymorphic
{
public:
	/**
		Generates hash from input data
		\param input - data array to generate hash of it.
	 */
	virtual QByteArray GenerateHash(const QByteArray& input) const = 0;

	/**
		Valdidates the \c hashValue with the \c inputData
		\warning you should use this method because some hash generators generates different hashes for the same string
	 */
	virtual bool ValidateHash(const QByteArray& inputData, const QByteArray& hashValue) const = 0;

};


} //namespace imtcrypt


