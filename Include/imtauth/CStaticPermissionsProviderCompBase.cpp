// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CStaticPermissionsProviderCompBase.h>


namespace imtauth
{


// public methods

// reimplemented (IFeatureInfoProvider)

imtlic::IFeatureInfoSharedPtr CStaticPermissionsProviderCompBase::GetFeatureInfo(const QByteArray& featureId) const
{
	imtlic::IFeatureInfoSharedPtr direct = m_featureCollection.GetFeatureInfo(featureId);
	if (direct.IsValid()){
		return direct;
	}

	return m_featureCollection.FindFeatureById(featureId);
}


const imtbase::ICollectionInfo& CStaticPermissionsProviderCompBase::GetFeatureList() const
{
	return m_featureCollection.GetFeatureList();
}


// protected methods

imtlic::CFeatureInfo* CStaticPermissionsProviderCompBase::CreatePermission(
			const QByteArray& featureId,
			const char* featureName,
			const char* featureDescription,
			const QByteArrayList& requirements)
{
	imtlic::CFeatureInfo* featureInfoPtr = new imtlic::CFeatureInfo;
	featureInfoPtr->SetFeatureId(featureId);
	featureInfoPtr->SetFeatureName(featureName);
	featureInfoPtr->SetFeatureDescription(featureDescription);
	featureInfoPtr->SetOptional(false);
	featureInfoPtr->SetIsPermission(true);
	featureInfoPtr->SetRequirements(requirements);

	return featureInfoPtr;
}


imtlic::CFeatureInfo* CStaticPermissionsProviderCompBase::AddPermission(
			imtlic::CFeatureInfo& parent,
			const QByteArray& featureId,
			const char* featureName,
			const char* featureDescription,
			const QByteArrayList& requirements)
{
	imtlic::CFeatureInfo* featureInfoPtr = CreatePermission(featureId, featureName, featureDescription, requirements);
	parent.InsertSubFeature(imtlic::IFeatureInfo::FeatureInfoPtr(featureInfoPtr));

	return featureInfoPtr;
}


// reimplemented (icomp::CComponentBase)

void CStaticPermissionsProviderCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	CreateFeatures();
}


} // namespace imtauth


