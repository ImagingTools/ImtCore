#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlic
{


/**
	Component implementation of ILicenseInfo interface with reference to the list of all defined products.
	\ingroup LicenseManagement
*/
class CFeatureInfoComp:
			public icomp::CComponentBase,
			public CFeatureInfo
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CFeatureInfo BaseClass2;

	I_BEGIN_COMPONENT(CFeatureInfoComp)
		I_REGISTER_INTERFACE(IFeatureInfo);
		I_REGISTER_INTERFACE(iser::IObject);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
};


} // namespace imtlic


