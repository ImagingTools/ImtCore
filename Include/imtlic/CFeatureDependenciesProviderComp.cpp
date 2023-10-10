#include <imtlic/CFeatureDependenciesProviderComp.h>


// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtlic
{


QByteArrayList CFeatureDependenciesProviderComp::GetFeatureDependencies(const QByteArray& featureId) const
{
	if (!m_packageCollectionCompPtr.IsValid()){
		return QByteArrayList();
	}

	return QByteArrayList();
}


const IFeatureInfoProvider* CFeatureDependenciesProviderComp::GetDependencyContainer(const QByteArray& /*dependencyId*/) const
{
	return nullptr;
}


} // namespace imtlic


