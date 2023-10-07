#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/IFeatureInfoProvider.h>


namespace imtlic
{


/**
	Component for providing of features.
*/
class CFeatureInfoProviderComp:
			public icomp::CComponentBase,
			virtual public IFeatureInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureInfoProviderComp)
		I_REGISTER_INTERFACE(IFeatureInfoProvider);
		I_ASSIGN(m_packageCollectionCompPtr, "PackageCollection", "Package collection", true, "PackageCollection");
	I_END_COMPONENT

	// reimplemented (IFeatureInfoProvider)
	virtual const IFeatureInfo* GetFeatureInfo(const QByteArray& featureId) const override;
	virtual const imtbase::ICollectionInfo& GetFeatureList() const override;

protected:
	I_REF(imtbase::IObjectCollection, m_packageCollectionCompPtr);
};


} // namespace imtlic


