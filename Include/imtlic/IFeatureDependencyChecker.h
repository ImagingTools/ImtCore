#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <istd/IChangeable.h>


class QByteArray;


namespace imtlic
{


/**
	Common interface for checking of dependencies for a feature.
*/
class IFeatureDependencyChecker: virtual public istd::IChangeable
{
public:
	typedef QMap<QByteArray, QByteArrayList> FeatureDependencyMap;

	/*
		Get dependency map for all features in package collection
	*/
	virtual const FeatureDependencyMap* GetDependencyMap() const = 0;

	/*
		Check dependency from one feature to another with custom dependency map
		Package collection dependency map will be used if dependencyMapPtr == nullptr 
	*/
	virtual bool HasDependency(const QByteArray& fromFeatureId, const QByteArray& toFeatureId, const FeatureDependencyMap* dependencyMapPtr = nullptr) const = 0;
};


} // namespace imtlic


