// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtlic
{


/**
	Interface for license validation status information.
	
	This interface provides information about the current status of a license,
	including validity, expiration, and goodwill period information.
	
	\ingroup LicenseManagement
*/
class ILicenseStatus: virtual public istd::IChangeable
{
public:
	enum LicenseStatusFlags
	{
		/**
			License is valid and can be used.
		*/
		LSF_LICENSE_VALID = 1,

		/**
			License is invalid and cannot be used.
		*/
		LSF_LICENSE_INVALID = 2,

		/**
			No license information is available.
		*/
		LSF_NO_LICENSE = 4,

		/**
			License has expired and is no longer valid.
		*/
		LSF_LICENSE_EXPIRED = 8,

		/**
			License is in goodwill mode (grace period after expiration).
		*/
		LSF_GOODWILL = 16
	};

	/**
		Get the current status flags of the license.
		Flags can be combined using bitwise OR.
		\return Combination of LicenseStatusFlags values
	*/
	virtual int GetLicenseStatusFlags() const = 0;

	/**
		Set the status flags of the license.
		\param licenseStatusFlags Combination of LicenseStatusFlags values
	*/
	virtual void SetLicenseStatusFlags(int licenseStatusFlags) = 0;

	/**
		Get the file system location of the license file.
		\return Path to the license file
	*/
	virtual QString GetLicenseLocation() const = 0;

	/**
		Set the file system location of the license file.
		\param licenseLocation Path to the license file
	*/
	virtual void SetLicenseLocation(const QString& licenseLocation) = 0;

	/**
		Get the number of remaining days in the goodwill period.
		Goodwill period is a grace period after license expiration.
		\return Number of days remaining in goodwill period
	*/
	virtual int GetGoodwillRemainingDays() const = 0;

	/**
		Set the number of remaining days in the goodwill period.
		\param days Number of days remaining
	*/
	virtual void SetGoodwillRemainingDays(int days) = 0;
};


} // namespace imtlic


