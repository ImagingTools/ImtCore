// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/CFeatureContainer.h>


namespace imtauth
{


class COrganizationsPermissionsProviderComp:
			public icomp::CComponentBase,
			virtual public imtlic::IFeatureInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(COrganizationsPermissionsProviderComp)
		I_REGISTER_INTERFACE(imtlic::IFeatureInfoProvider);
	I_END_COMPONENT

	// reimplemented (IFeatureInfoProvider)
	virtual imtlic::IFeatureInfoSharedPtr GetFeatureInfo(const QByteArray& featureId) const override;
	virtual const imtbase::ICollectionInfo& GetFeatureList() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void CreateOrganizationFeatures();

private:
	imtlic::CFeatureContainer m_featureCollection;
};


} // namespace imtauth
