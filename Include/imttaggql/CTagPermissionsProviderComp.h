// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/CFeatureContainer.h>
#include <imtlic/IFeatureInfoProvider.h>


namespace imttaggql
{


/**
	Declares the tag permissions (ViewTags, AssignTags, ManageTags) for the roles of a tenant.
	Changing system tags needs the superuser and is not a grantable permission.
*/
class CTagPermissionsProviderComp:
			public icomp::CComponentBase,
			virtual public imtlic::IFeatureInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTagPermissionsProviderComp)
		I_REGISTER_INTERFACE(imtlic::IFeatureInfoProvider);
	I_END_COMPONENT

	// reimplemented (imtlic::IFeatureInfoProvider)
	virtual imtlic::IFeatureInfoSharedPtr GetFeatureInfo(const QByteArray& featureId) const override;
	virtual const imtbase::ICollectionInfo& GetFeatureList() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void InsertPermission(const QByteArray& featureId, const QString& name, const QString& description);

	imtlic::CFeatureContainer m_featureCollection;
};


} // namespace imttaggql


