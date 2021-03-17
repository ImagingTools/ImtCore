#pragma once


// Qt includes
#include <QtCore/QMap>

namespace imtbase
{
	class IObjectCollection;
}

namespace imtlic
{
	class IFeatureInfo;
	class IFeaturePackage;
}


namespace imtlic
{


class CFeaturePackageCollectionUtility
{
public:
	typedef QMap<QByteArray, QByteArrayList> FeatureDependencyMap;

	static QByteArrayList GetPackageIds(const imtbase::IObjectCollection& packageCollection);
	static const IFeaturePackage* GetPackagePtr(const imtbase::IObjectCollection& packageCollection, const QByteArray& packageId);
	static QString GetPackageName(const imtbase::IObjectCollection& packageCollection, const QByteArray& packageId);

	static FeatureDependencyMap GetDependencies(const imtbase::IObjectCollection& packageCollection);
	static bool HasDependency(const FeatureDependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId);

	static QByteArrayList GetFeatureIds(const imtbase::IObjectCollection& packageCollection, const QByteArray& packageId);
	static QByteArrayList GetAllFeatureIds(const imtbase::IObjectCollection& packageCollection);
	static const IFeatureInfo* GetFeaturePtr(const imtbase::IObjectCollection& packageCollection, const QByteArray& featureId);
	static const IFeaturePackage* GetFeaturePackagePtr(const imtbase::IObjectCollection& packageCollection, const QByteArray& featureId);
};


} // namespace imtlic


