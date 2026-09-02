// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CFeatureDependenciesProviderComp.h>


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

QJsonObject CFeatureDependenciesProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QStringLiteral("Unable to get a feature dependencies. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CFeatureDependenciesProviderComp");

		return QJsonObject();
	}

	QByteArray features = gqlInputParamPtr->GetParamArgumentValue("FeatureIds").toByteArray();
	if (features.isEmpty()){
		errorMessage = QStringLiteral("Unable to get dependencies for feature with empty Feature-ID.");
		SendErrorMessage(0, errorMessage, "CFeatureDependenciesProviderComp");

		return QJsonObject();
	}

	QByteArrayList featureIds = features.split(';');

	QByteArrayList retVal;

	for (const QByteArray& featureId : featureIds){
		retVal += GetFeatureDependencies(featureId);
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	dataObj.insert(QStringLiteral("FeatureIds"), QJsonValue::fromVariant(features));
	dataObj.insert(QStringLiteral("FeaturesDependencies"), QJsonValue::fromVariant(retVal.join(';')));

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


// reimplemented (imtlic::IFeatureDependenciesProvider)

QByteArrayList CFeatureDependenciesProviderComp::GetFeatureDependencies(const QByteArray& featureId) const
{
	if (!m_featureCollectionCompPtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("Unable to get a feature dependencies. Internal error."), "CFeatureDependenciesProviderComp");

		return QByteArrayList();
	}

	QByteArrayList retVal;

	iprm::CIdParam idParam;
	idParam.SetId(featureId);

	iprm::CParamsSet paramsSet1;
	paramsSet1.SetEditableParameter("FeatureId", &idParam);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

	imtbase::ICollectionInfo::Ids collectionIds = m_featureCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		imtbase::ICollectionInfo::Id featureUuid = collectionIds[0];

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_featureCollectionCompPtr->GetObjectData(featureUuid, dataPtr)){
			const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
			if (featureInfoPtr != nullptr){
				QByteArrayList featureDependencies;
				if (featureInfoPtr->GetFeatureId() != featureId){
					imtlic::IFeatureInfoSharedPtr subfeatureInfoPtr = featureInfoPtr->GetSubFeature(featureId);
					if (subfeatureInfoPtr.IsValid()){
						featureDependencies = subfeatureInfoPtr->GetDependencies();
					}
				}
				else{
					featureDependencies = featureInfoPtr->GetDependencies();
				}

				retVal += featureDependencies;

				for (const QByteArray& dependencyId : std::as_const(featureDependencies)){
					retVal += GetFeatureDependencies(dependencyId);
				}
			}
		}
	}

	return retVal;
}


const imtlic::IFeatureInfoProvider* CFeatureDependenciesProviderComp::GetDependencyContainer(const QByteArray& /*dependencyId*/) const
{
	return nullptr;
}


} // namespace imtlicgql


