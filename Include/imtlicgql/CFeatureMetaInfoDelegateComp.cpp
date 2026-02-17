// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CFeatureMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlicgql
{


// protected methods

bool CFeatureMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo, const QByteArray& typeId) const
{
	Q_UNUSED(typeId);
	
	QByteArray featureId = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_ID).toByteArray();
	representation["FeatureId"] = QString(featureId);
	
	QString featureName = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_NAME).toString();
	representation["FeatureName"] = QString(featureName);
	
	QString description = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_DESCRIPTION).toString();
	representation["Description"] = QString(description);
	
	bool isOptional = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_IS_OPTIONAL).toBool();
	representation["IsOptional"] = isOptional;

	bool isPermission = metaInfo.GetMetaInfo(imtlic::IFeatureInfo::MIT_IS_PERMISSION).toBool();
	representation["IsPermission"] = isPermission;
	
	return true;
}


bool CFeatureMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation, const QByteArray& typeId) const
{
	Q_UNUSED(typeId);
	
	if (representation.contains("FeatureId")){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_ID, representation.value("FeatureId"));
	}
	
	if (representation.contains("FeatureName")){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_NAME, representation.value("FeatureName"));
	}
	
	if (representation.contains("Description")){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_FEATURE_DESCRIPTION, representation.value("Description"));
	}
	
	if (representation.contains("IsOptional")){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_IS_OPTIONAL, representation.value("IsOptional"));
	}
	
	if (representation.contains("IsPermission")){
		metaInfo.SetMetaInfo(imtlic::IFeatureInfo::MIT_IS_PERMISSION, representation.value("IsPermission"));
	}
	
	return true;
}


} // namespace imtlicgql


