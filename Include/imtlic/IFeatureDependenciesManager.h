// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <imtlic/IFeatureDependenciesProvider.h>


namespace imtlic
{


/**
	Common interface for managing of dependencies for a feature.
*/
class IFeatureDependenciesManager: virtual public IFeatureDependenciesProvider
{
public:
	/**
		Set list of features the given feature depends on.
	*/
	virtual void SetFeatureDependencies(const QByteArray& featureId, const QByteArrayList& dependentIds) = 0;
};


} // namespace imtlic


