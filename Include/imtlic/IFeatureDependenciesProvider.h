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
	Interface for providing feature dependency information.
	
	This interface is used to resolve feature dependencies - determining
	which other features must be available for a given feature to function.
	It also helps locate where dependency features are defined.
	
	\sa IFeatureInfo, IFeatureInfoProvider
	\ingroup LicenseManagement
*/
class IFeatureDependenciesProvider: virtual public istd::IChangeable
{
public:
	/**
		Get the list of features that a given feature depends on.
		These dependencies must be satisfied for the feature to function properly.
		\param featureId ID of the feature to query
		\return List of feature IDs that this feature depends on
	*/
	virtual QByteArrayList GetFeatureDependencies(const QByteArray& featureId) const = 0;

	/**
		Get the container (provider) that hosts a specific dependency feature.
		This helps locate where a dependency feature is defined.
		\param dependencyId ID of the dependency feature
		\return Pointer to the feature info provider containing the dependency, or nullptr if not found
	*/
	virtual const IFeatureInfoProvider* GetDependencyContainer(const QByteArray& dependencyId) const = 0;
};


} // namespace imtlic


