// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRoleRemoteCollectionControllerComp.h>


// ACF includes
#include <iqt/iqt.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtauthgql
{


// protected methods

QString CRoleRemoteCollectionControllerComp::GetPermissionName(const QByteArray& permissionId) const
{
	if (!m_productInfoCompPtr.IsValid()){
		return QString();
	}

	imtbase::IObjectCollection* featureCollectionPtr = m_productInfoCompPtr->GetFeatures();
	if (featureCollectionPtr == nullptr){
		return QString();
	}

	imtbase::ICollectionInfo::Ids elementIds = featureCollectionPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (featureCollectionPtr->GetObjectData(elementId, dataPtr)){
			const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
			if (featureInfoPtr != nullptr){
				for (imtbase::ICollectionInfo::Id& subFeatureId : featureInfoPtr->GetSubFeatureIds()){
					if (permissionId == subFeatureId){
						imtlic::IFeatureInfoSharedPtr subFeatureInfoPtr = featureInfoPtr->GetSubFeature(subFeatureId);
						if (subFeatureInfoPtr.IsValid()){
							return subFeatureInfoPtr->GetFeatureName();
						}
					}
				}
			}
		}
	}

	return permissionId;
}


// reimplemented (imtclientgql::CRemoteObjectCollectionControllerComp)

sdl::V1_0::imtbase::CGetElementMetaInfoPayload CRoleRemoteCollectionControllerComp::OnGetElementMetaInfo(
			const sdl::V1_0::imtbase::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetElementMetaInfoPayload payload = BaseClass::OnGetElementMetaInfo(getElementMetaInfoRequest, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return payload;
	}

	if (!payload.HasValue()){
		Q_ASSERT(false);
		return payload;
	}

	if (payload.elementMetaInfo.HasValue() && payload.elementMetaInfo->infoParams.HasValue()){
		imtsdl::TElementList<sdl::V1_0::imtbase::CParameter> infoParams = *payload.elementMetaInfo->infoParams;
		QList<sdl::V1_0::imtbase::CParameter> paramList = infoParams.ToList();
		for (sdl::V1_0::imtbase::CParameter& parameter : paramList){
			if (parameter.id == QByteArrayLiteral("Permissions")){
				QString newData;
				QString data = *parameter.data;
				QByteArrayList permissionIds = (*parameter.data).toUtf8().split('\n');
				for (const QByteArray& permissionId : permissionIds){
					QString permissionName = GetPermissionName(permissionId);
					newData += permissionName + "\n";
				}

				parameter.data = newData;
			}
		}
	}

	return payload;
}


} // namespace imtauthgql


