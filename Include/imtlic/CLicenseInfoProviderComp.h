#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/ILicenseInfoProvider.h>


namespace imtlic
{


/**
	Component for providing of licenses.
*/
class CLicenseInfoProviderComp:
			public icomp::CComponentBase,
			virtual public ILicenseInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLicenseInfoProviderComp)
		I_REGISTER_INTERFACE(ILicenseInfoProvider);
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
	I_END_COMPONENT

	// reimplemented (ILicenseInfoProvider)
	virtual const imtbase::ICollectionInfo& GetLicenseList() const override;
	virtual const imtlic::ILicenseInfo* GetLicenseInfo(const QByteArray& licenseId) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
};


} // namespace imtlic


