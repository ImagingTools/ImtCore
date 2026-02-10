// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


class QByteArray;
class QString;


namespace imtlic
{


class IFeatureInfoProvider;


/**
	Common interface for providing of dependencies for a feature.
*/
class IFeatureDependenciesProvider: virtual public istd::IChangeable
{
public:
	/**
		Get list of features the given feature depends on.
	*/
	virtual QByteArrayList GetFeatureDependencies(const QByteArray& featureId) const = 0;

	/**
		Get container where the given dependency is hosted.
	*/
	virtual const IFeatureInfoProvider* GetDependencyContainer(const QByteArray& dependencyId) const = 0;
};


} // namespace imtlic


