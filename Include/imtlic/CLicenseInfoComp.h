#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/CLicenseInfo.h>


namespace imtlic
{


/**
	Component implementation of ILicenseInfo interface with reference to the list of all defined products.
	\ingroup LicenseManagement
*/
class CLicenseInfoComp:
			public icomp::CComponentBase,
			public CLicenseInfo
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CLicenseInfo BaseClass2;

	I_BEGIN_COMPONENT(CLicenseInfoComp)
		I_REGISTER_INTERFACE(ILicenseInfo);
		I_REGISTER_INTERFACE(iser::IObject);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_featurePackageCollectionCompPtr, "FeaturePackageCollection", "Feature package collection", true, "");
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_REF(imtbase::IObjectCollection, m_featurePackageCollectionCompPtr);
};


} // namespace imtlic


