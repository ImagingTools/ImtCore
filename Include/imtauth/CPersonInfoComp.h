#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IAddressProvider.h>
#include <imtauth/CPersonInfo.h>


namespace imtauth
{


/**
	Component implementation of IPersonInfo interface with reference to the list of all defined Addresses.
	\ingroup LicenseManagement
*/
class CPersonInfoComp:
			public icomp::CComponentBase,
			public CPersonInfo
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CPersonInfo BaseClass2;

	I_BEGIN_COMPONENT(CPersonInfoComp)
		I_REGISTER_INTERFACE(IPersonInfo);
		I_REGISTER_INTERFACE(iser::IObject);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
};


} // namespace imtauth


