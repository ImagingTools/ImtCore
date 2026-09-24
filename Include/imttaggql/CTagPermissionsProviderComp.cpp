// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttaggql/CTagPermissionsProviderComp.h>


// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imttaggql/imttaggql.h>


namespace imttaggql
{


// public methods

// reimplemented (imtlic::IFeatureInfoProvider)

imtlic::IFeatureInfoSharedPtr CTagPermissionsProviderComp::GetFeatureInfo(const QByteArray& featureId) const
{
	return m_featureCollection.GetFeatureInfo(featureId);
}


const imtbase::ICollectionInfo& CTagPermissionsProviderComp::GetFeatureList() const
{
	return m_featureCollection.GetFeatureList();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CTagPermissionsProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	InsertPermission(s_viewTagsPermission, QT_TRANSLATE_NOOP("Feature", "View Tags"), QT_TRANSLATE_NOOP("Feature", "Allows seeing the tag catalog and the tags of objects."));
	InsertPermission(s_assignTagsPermission, QT_TRANSLATE_NOOP("Feature", "Assign Tags"), QT_TRANSLATE_NOOP("Feature", "Allows adding tags to objects and removing them."));
	InsertPermission(s_manageTagsPermission, QT_TRANSLATE_NOOP("Feature", "Manage Tags"), QT_TRANSLATE_NOOP("Feature", "Allows creating, changing and deleting the tags of the organization."));
}


// private methods

void CTagPermissionsProviderComp::InsertPermission(const QByteArray& featureId, const QString& name, const QString& description)
{
	istd::TDelPtr<imtlic::CFeatureInfo> featureInfoPtr;
	featureInfoPtr.SetPtr(new imtlic::CFeatureInfo);
	featureInfoPtr->SetFeatureId(featureId);
	featureInfoPtr->SetFeatureName(name);
	featureInfoPtr->SetFeatureDescription(description);
	featureInfoPtr->SetOptional(false);
	featureInfoPtr->SetIsPermission(true);

	m_featureCollection.InsertNewObject("FeatureInfo", "", "", featureInfoPtr.GetPtr());
}


} // namespace imttaggql


