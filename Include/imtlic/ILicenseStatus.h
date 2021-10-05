#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtlic
{


/**
	\ingroup LicenseManagement
*/
class ILicenseStatus: virtual public istd::IChangeable
{
public:
	enum LicenseStatusFlags
	{
		/**
			License is valid.
		*/
		LSF_LICENSE_VALID = 1,

		/**
			License is invalid.
		*/
		LSF_LICENSE_INVALID = 2,

		/**
			No license informations are available.
		*/
		LSF_NO_LICENSE = 4,

		/**
			License is expired.
		*/
		LSF_LICENSE_EXPIRED = 8
	};

	/**
		Get the status flags of the license.
	*/
	virtual int GetLicenseStatusFlags() const = 0;

	/**
		Set the status flags of the license.
	*/
	virtual void SetLicenseStatusFlags(int licenseStatusFlags) = 0;

	/**
	Get location of the license file in the system.
	*/
	virtual QString GetLicenseLocation() const = 0;

	/**
		Set location of the license file in the system.
	*/
	virtual void SetLicenseLocation(const QString& licenseLocation) = 0;
};


} // namespace imtlic


