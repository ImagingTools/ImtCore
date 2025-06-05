#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtlic
{

/**
	Provider of encrypted license keys for a product instance.
	\ingroup LicenseManagement
*/
class IKeyRing: virtual public istd::IChangeable
{
public:
	
	virtual QByteArray GetKey(const QByteArray& keyId) const = 0;
	virtual bool InsertKey(const QByteArray& keyId, const QByteArray& key) = 0;
	
	/**
		Get the encrypted license key for a given or default license-ID.
	*/
	virtual QByteArray SelectKeyFor(const QByteArray& licenseId = QByteArray()) = 0;
	virtual QByteArray SelectKeyFor(const QString& licenseFile) = 0;
};


} // namespace imtlic


