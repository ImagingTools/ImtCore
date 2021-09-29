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
};


} // namespace imtlic


