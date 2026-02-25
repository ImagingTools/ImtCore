// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CFeatureDependenciesProviderComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CFeatureDependenciesProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QString("Unable to get a feature dependencies. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CFeatureDependenciesProviderComp");

		return nullptr;
	}

	QByteArray features = gqlInputParamPtr->GetParamArgumentValue("FeatureIds").toByteArray();
	if (features.isEmpty()){
		errorMessage = QString("Unable to get dependencies for feature with empty Feature-ID.");
		SendErrorMessage(0, errorMessage, "CFeatureDependenciesProviderComp");

		return nullptr;
	}

	QByteArrayList featureIds = features.split(';');

	QByteArrayList retVal;

	for (const QByteArray& featureId : featureIds){
		retVal += GetFeatureDependencies(featureId);
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	dataModelPtr->SetData("FeatureIds", features);
	dataModelPtr->SetData("FeaturesDependencies", retVal.join(';'));

	return rootModelPtr.PopPtr();
}


// reimplemented (imtlic::IFeatureDependenciesProvider)

QByteArrayList CFeatureDependenciesProviderComp::GetFeatureDependencies(const QByteArray& featureId) const
{
	if (!m_featureCollectionCompPtr.IsValid()){
		SendErrorMessage(0, QString("Unable to get a feature dependencies. Internal error."), "CFeatureDependenciesProviderComp");

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


