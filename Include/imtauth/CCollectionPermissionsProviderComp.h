#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IPermissionsProvider.h>


namespace imtauth
{


class CCollectionPermissionsProviderComp:
		public icomp::CComponentBase,
		virtual public imtauth::IPermissionsProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionPermissionsProviderComp);
		I_REGISTER_INTERFACE(imtauth::IPermissionsProvider)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection)
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
	I_END_COMPONENT;

	// reimplemented (imtauth::IPermissionsProvider)	
	virtual const imtlic::ILicenseInfo::FeatureInfos GetPermissionInfos(const QByteArray& productId) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
};


} // namespace imtauth
