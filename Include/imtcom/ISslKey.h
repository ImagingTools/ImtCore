#pragma once


// Acf includes
#include <iser/ISerializable.h>


namespace imtcom
{


class ISslKey: virtual public iser::ISerializable
{
public:
	enum KeyAlgorithm
	{
		KA_RSA,		///< The RSA algorithm.
		KA_DSA,		///< The DSA algorithm.
		KA_EC,		///< The Elliptic Curve algorithm.
		KA_DH,		///< The Diffie-Hellman algorithm.
		KA_OPAQUE	///< A key that should be treated as a 'black box'.
	};
	I_DECLARE_ENUM(KeyAlgorithm,
				KA_RSA,
				KA_DSA,
				KA_EC,
				KA_DH,
				KA_OPAQUE)

	enum EncodingFormat
	{
		EF_PEM,	///< The PEM format. Used by default.
		EF_DER  ///< The DER format.
	};
	I_DECLARE_ENUM(EncodingFormat,
				EF_PEM,
				EF_DER)
	enum KeyType
	{
		KT_PRIVATE_KEY,		///< A private key.
		KT_PUBLIC_KEY		///< A public key.
	};
	I_DECLARE_ENUM(KeyType,
				KT_PRIVATE_KEY,
				KT_PUBLIC_KEY)

	/// \note MUST NOT be empty if \c IsFile() = false
	virtual QByteArray GetEncodedData() const = 0;

	/// \note MUST NOT be empty if \c IsFile() = true
	virtual QString GetFilePath() const = 0;
	virtual bool IsFile() const = 0;
	virtual KeyAlgorithm GetKeyAlgorithm() const = 0;
	virtual EncodingFormat GetEncodingFormat() const = 0;
	virtual KeyType GetKeyType() const = 0;
	virtual QByteArray GetPassPhrase() const = 0;

};


} // namespace imtcom


