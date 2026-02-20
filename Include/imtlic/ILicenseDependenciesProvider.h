// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtlic
{


/**
	Interface for providing license dependency information.
	
	This interface is used to resolve license dependencies - determining
	which other licenses must be activated before a given license can be used.
	This is important for hierarchical licensing models where advanced licenses
	require basic licenses as prerequisites.
	
	\sa ILicenseDefinition
	\ingroup LicenseManagement
*/
class ILicenseDependenciesProvider: virtual public istd::IChangeable
{
public:
	/**
		Get the list of licenses that a given license depends on.
		These prerequisite licenses must be activated before the target license can be used.
		\param licenseId ID of the license to query
		\return List of license IDs that must be active as prerequisites
	*/
	virtual QByteArrayList GetLicenseDependencies(const QByteArray& licenseId) const = 0;
};


} // namespace imtlic


