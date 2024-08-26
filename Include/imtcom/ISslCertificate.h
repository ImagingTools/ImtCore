#pragma once


// Acf includes
#include <iser/ISerializable.h>


namespace imtcom
{


class ISslCertificate: virtual public iser::ISerializable
{
public:
	enum EncodingFormat
	{
		EF_PEM,	///< (default) The PEM format.
		EF_DER  ///< The DER format.
	};
	I_DECLARE_ENUM(EncodingFormat,
				EF_PEM,
				EF_DER)

	/// \note MUST NOT be empty if \c IsFile() = false
	virtual QByteArray GetEncodedData() const = 0;

	/// \note MUST NOT be empty if \c IsFile() = true
	virtual QString GetFilePath() const = 0;
	virtual bool IsFile() const = 0;
	virtual EncodingFormat GetEncodingFormat() const = 0;
};


} // namespace imtcom


