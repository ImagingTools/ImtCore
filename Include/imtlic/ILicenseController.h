// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>


namespace imtlic
{


/**
	Interface for a license controller on the client side.
	
	The License Controller manages license file operations on the client installation,
	including importing license files and validating license keys.
	
	\sa IProductInstanceInfo
	\ingroup LicenseManagement
*/
class ILicenseController: virtual public istd::IPolymorphic
{
public:
	/**
		Import a license file into the system.
		This activates new licenses on the client installation.
		\param licenseFilePath Path to the license file to import
		\param logPtr Optional message consumer for logging import operations
		\return true if the license was successfully imported, false otherwise
	*/
	virtual bool ImportLicense(const QString& licenseFilePath, ilog::IMessageConsumer* logPtr = nullptr) const = 0;

	/**
		Check if a license key is valid.
		\param key License key to validate
		\return true if the key is valid, false otherwise
	*/
	virtual bool CheckLicense(const QByteArray& key) const = 0;

	/**
		Check if a license key is valid for a specific license file.
		\param key License key to validate
		\param licensePath Path to the license file
		\return true if the key is valid for the license file, false otherwise
	*/
	virtual bool CheckLicense(const QByteArray& key, const QString& licensePath) const = 0;
};


} // namespace imtlic


