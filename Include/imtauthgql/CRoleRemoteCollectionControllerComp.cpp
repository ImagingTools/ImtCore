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

sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload CRoleRemoteCollectionControllerComp::OnGetElementMetaInfo(
			const sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload payload = BaseClass::OnGetElementMetaInfo(getElementMetaInfoRequest, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return payload;
	}

	if (!payload.Version_1_0.HasValue()){
		Q_ASSERT(false);
		return payload;
	}

	if (payload.Version_1_0->elementMetaInfo.HasValue() && payload.Version_1_0->elementMetaInfo->infoParams.HasValue()){
		imtsdl::TElementList<sdl::imtbase::ImtBaseTypes::CParameter::V1_0> infoParams = *payload.Version_1_0->elementMetaInfo->infoParams;
		QList<sdl::imtbase::ImtBaseTypes::CParameter::V1_0> paramList = infoParams.ToList();
		for (sdl::imtbase::ImtBaseTypes::CParameter::V1_0& parameter : paramList){
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


