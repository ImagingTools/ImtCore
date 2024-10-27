#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>


namespace imtlic
{


/**
	Interface for a license controller on the client side.
	\ingroup LicenseManagement
*/
class ILicenseController: virtual public istd::IPolymorphic
{
public:
	/**
		Import the license file into the system.
	*/
	virtual bool ImportLicense(const QString& licenseFilePath, ilog::IMessageConsumer* logPtr = nullptr) const = 0;

	/**
		Check if license is valid
	*/
	virtual bool CheckLicense(const QByteArray& key) const = 0;

	/**
		Check if provided license is valid
	*/
	virtual bool CheckLicense(const QByteArray& key, const QString& licensePath) const = 0;
};


} // namespace imtlic


