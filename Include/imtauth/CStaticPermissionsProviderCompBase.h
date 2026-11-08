// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/CFeatureContainer.h>
#include <imtlic/CFeatureInfo.h>


namespace imtauth
{


/**
	Common part of the components providing a permission tree that is built in code.
	Derived components only describe their tree in CreateFeatures.
*/
class CStaticPermissionsProviderCompBase:
			public icomp::CComponentBase,
			virtual public imtlic::IFeatureInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CStaticPermissionsProviderCompBase);
		I_REGISTER_INTERFACE(imtlic::IFeatureInfoProvider);
	I_END_COMPONENT;

	// reimplemented (IFeatureInfoProvider)
	virtual imtlic::IFeatureInfoSharedPtr GetFeatureInfo(const QByteArray& featureId) const override;
	virtual const imtbase::ICollectionInfo& GetFeatureList() const override;

protected:
	static imtlic::CFeatureInfo* CreatePermission(
				const QByteArray& featureId,
				const char* featureName,
				const char* featureDescription,
				const QByteArrayList& requirements = QByteArrayList());
	static imtlic::CFeatureInfo* AddPermission(
				imtlic::CFeatureInfo& parent,
				const QByteArray& featureId,
				const char* featureName,
				const char* featureDescription,
				const QByteArrayList& requirements = QByteArrayList());

	virtual void CreateFeatures() = 0;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	imtlic::CFeatureContainer m_featureCollection;
};


} // namespace imtauth


