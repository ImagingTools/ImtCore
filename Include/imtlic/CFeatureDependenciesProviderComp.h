#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureDependenciesProvider.h>


namespace imtlic
{


/**
	Component for dependencies providing of features.
*/
class CFeatureDependenciesProviderComp:
			public icomp::CComponentBase,
			virtual public IFeatureDependenciesProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureDependenciesProviderComp)
		I_REGISTER_INTERFACE(IFeatureDependenciesProvider);
		I_ASSIGN(m_packageCollectionCompPtr, "PackageCollection", "Package collection", true, "PackageCollection");
	I_END_COMPONENT

	// reimplemented (IFeatureDependenciesProvider)
	virtual QByteArrayList GetFeatureDependencies(const QByteArray& featureId) const override;
	virtual const IFeatureInfoProvider* GetDependencyContainer(const QByteArray& dependencyId) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_packageCollectionCompPtr);
};


} // namespace imtlic


