#include <imtlicgql/CPackageControllerComp.h>


// ACF includes
#include <istd/TChangeDelegator.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>


namespace imtlicgql
{


// pretected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

istd::IChangeable* CPackageControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray& objectId,
		QString& name,
		QString& description,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);
		return nullptr;
	}

	if (!m_featureInfoProviderCompPtr.IsValid()){
		Q_ASSERT(false);
		return nullptr;
	}

	if (inputParams.isEmpty()){
		errorMessage = QObject::tr("Can not create package: %1").arg(QString(objectId));
		Q_ASSERT(false);
		return nullptr;
	}

//	objectId = GetObjectIdFromInputParams(inputParams);

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<imtlic::CFeaturePackage> featurePackagePtr = new imtlic::CFeaturePackage;
		if (featurePackagePtr == nullptr){
			SendErrorMessage(0, "Unable to get a feature package", "Package Controller");

			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;

		if (!itemModel.CreateFromJson(itemData)){
			return nullptr;
		}

		if (itemModel.ContainsKey("Id")){
			objectId = itemModel.GetData("Id").toByteArray();
		}

		if (objectId.isEmpty()){
			errorMessage = QT_TR_NOOP("Package-ID can not be empty!");
//			Q_ASSERT(false);
			return nullptr;
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
		}

		QString description;
		if (itemModel.ContainsKey("Description")){
			description = itemModel.GetData("Description").toString();
		}

		featurePackagePtr->SetPackageId(objectId);
		featurePackagePtr->SetElementDescription(objectId, description);

		imtbase::CTreeItemModel* featuresModelPtr = nullptr;

		if (itemModel.ContainsKey("Items")){
			featuresModelPtr = itemModel.GetTreeItemModel("Items");
		}

		imtbase::CTreeItemModel* dependenciesModelPtr = nullptr;

		if (itemModel.ContainsKey("DependenciesModel")){
			dependenciesModelPtr = itemModel.GetTreeItemModel("DependenciesModel");
		}

		if (featuresModelPtr != nullptr){
			for (int i = 0; i < featuresModelPtr->GetItemsCount(); i++){
				QString featureName;
				if (featuresModelPtr->ContainsKey("Name", i)){
					featureName = featuresModelPtr->GetData("Name", i).toString();
				}

				QByteArray featureId;
				if (featuresModelPtr->ContainsKey("Id", i)){
					featureId = featuresModelPtr->GetData("Id", i).toByteArray();
				}

//				const imtlic::IFeatureInfo* findFeatureInfoPtr = m_featureInfoProviderCompPtr->GetFeatureInfo(featureId);
//				if (findFeatureInfoPtr != nullptr){
//					errorMessage = QT_TR_NOOP(QString("Please change the Feature-ID for %1. (Feature-ID already exists)").arg(featureName));
//					return nullptr;
//				}

				if (featureId.isEmpty()){
					errorMessage = QT_TR_NOOP(QString("%1 has an empty Feature-ID").arg(featureName));
					return nullptr;
				}

				QString featureDescription;
				if (featuresModelPtr->ContainsKey("Description", i)){
					featureDescription = featuresModelPtr->GetData("Description", i).toString();
				}

				bool isOptional = false;
				if (featuresModelPtr->ContainsKey("Optional", i)){
					isOptional = featuresModelPtr->GetData("Optional", i).toBool();
				}

				imtlic::CFeatureInfo* featureInfoPtr = new imtlic::CFeatureInfo;
				featureInfoPtr->SetFeatureId(featureId);
				featureInfoPtr->SetFeatureName(featureName);
				featureInfoPtr->SetOptional(isOptional);

				if (featuresModelPtr->ContainsKey("ChildModel", i)){
					imtbase::CTreeItemModel* subFeaturesModelPtr = featuresModelPtr->GetTreeItemModel("ChildModel", i);
					bool result = InsertSubFeaturesToDataFromModel(featurePackagePtr.GetPtr(), featureInfoPtr, dependenciesModelPtr, subFeaturesModelPtr, errorMessage);
					if (!result){
						return nullptr;
					}
				}

				featurePackagePtr->InsertNewObject("FeatureInfo", featureName, featureDescription, featureInfoPtr);

				if (dependenciesModelPtr != nullptr){
					if (dependenciesModelPtr->ContainsKey(featureId)){
						QByteArray dependencies = dependenciesModelPtr->GetData(featureId).toByteArray();

						if (!dependencies.isEmpty()){
							QByteArrayList dependenciesList = dependencies.split(';');
							featurePackagePtr->SetFeatureDependencies(featureId, dependenciesList);
						}
					}
				}
			}
		}

		return featurePackagePtr.PopPtr();
	}
	errorMessage = QObject::tr("Can not create package: %1").arg(QString(objectId));

	return nullptr;
}


bool CPackageControllerComp::InsertSubFeaturesToDataFromModel(
		imtlic::CFeaturePackage* packagePtr,
		imtlic::IFeatureInfo* parentFeaturePtr,
		const imtbase::CTreeItemModel* dependenciesModelPtr,
		const imtbase::CTreeItemModel* subFeaturesModelPtr,
		QString& errorMessage) const
{
	for (int i = 0; i < subFeaturesModelPtr->GetItemsCount(); i++){
		QString featureName;
		if (subFeaturesModelPtr->ContainsKey("Name", i)){
			featureName = subFeaturesModelPtr->GetData("Name", i).toString();
		}

		QByteArray featureId;
		if (subFeaturesModelPtr->ContainsKey("Id", i)){
			featureId = subFeaturesModelPtr->GetData("Id", i).toByteArray();
		}

//		const imtlic::IFeatureInfo* findFeatureInfoPtr = m_featureInfoProviderCompPtr->GetFeatureInfo(featureId);
//		if (findFeatureInfoPtr != nullptr){
//			errorMessage = QT_TR_NOOP(QString("Please change the Feature-ID for %1. (Feature-ID already exists)").arg(featureName));
//			return false;
//		}

		if (featureId.isEmpty()){
			errorMessage = QT_TR_NOOP(QString("%1 has an empty Feature-ID").arg(featureName));
			return false;
		}

		QString description;
		if (subFeaturesModelPtr->ContainsKey("Description", i)){
			description = subFeaturesModelPtr->GetData("Description", i).toString();
		}

		bool isOptional = false;
		if (subFeaturesModelPtr->ContainsKey("Optional", i)){
			isOptional = subFeaturesModelPtr->GetData("Optional", i).toBool();
		}

		istd::TChangeDelegator<imtlic::CFeatureInfo>* featureInfoPtr = new istd::TChangeDelegator<imtlic::CFeatureInfo>;

		featureInfoPtr->SetSlavePtr(parentFeaturePtr);

		featureInfoPtr->SetFeatureId(featureId);
		featureInfoPtr->SetFeatureName(featureName);
		featureInfoPtr->SetOptional(isOptional);

		parentFeaturePtr->InsertSubFeature(featureInfoPtr);

		if (dependenciesModelPtr != nullptr){
			if (dependenciesModelPtr->ContainsKey(featureId)){
				QByteArray dependencies = dependenciesModelPtr->GetData(featureId).toByteArray();

				if (!dependencies.isEmpty()){
					QByteArrayList dependenciesList = dependencies.split(';');
					packagePtr->SetFeatureDependencies(featureId, dependenciesList);
				}
			}
		}

		if (subFeaturesModelPtr->ContainsKey("ChildModel", i)){
			imtbase::CTreeItemModel *subModelPtr = subFeaturesModelPtr->GetTreeItemModel("ChildModel", i);

			bool result = InsertSubFeaturesToDataFromModel(packagePtr, featureInfoPtr, dependenciesModelPtr, subModelPtr, errorMessage);
			if (!result){
				return false;
			}
		}
	}

	return true;
}


void CPackageControllerComp::InsertSubFeaturesToModelFromData(
		const imtlic::CFeaturePackage* packagePtr,
		const imtlic::IFeatureInfo* featurePtr,
		imtbase::CTreeItemModel* dependenciesModelPtr,
		imtbase::CTreeItemModel* featuresModelPtr) const
{
	const imtlic::FeatureInfoList& subFeatures = featurePtr->GetSubFeatures();

	for (int i = 0; i < subFeatures.GetCount(); i++){
		const imtlic::IFeatureInfo* subFeatureInfoPtr = subFeatures.GetAt(i);

		int index = featuresModelPtr->InsertNewItem();

		QByteArray featureId = subFeatureInfoPtr->GetFeatureId();
		QString featureName = subFeatureInfoPtr->GetFeatureName();

		QByteArray featureCollectionId = packagePtr->GetFeatureCollectionId(featureId);
		QString featureDescription = packagePtr->GetFeatureList().GetElementInfo(featureCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

		bool isOptional = subFeatureInfoPtr->IsOptional();

		featuresModelPtr->SetData("Id", featureId, index);
		featuresModelPtr->SetData("Name", featureName, index);
		featuresModelPtr->SetData("Optional", isOptional, index);
		featuresModelPtr->SetData("Description", featureDescription, index);

		if (dependenciesModelPtr != nullptr){
			QByteArrayList featureDependenciesList = packagePtr->GetFeatureDependencies(featureId);
			if (!featureDependenciesList.isEmpty()){
				QByteArray dependencies = featureDependenciesList.join(';');
				dependenciesModelPtr->SetData(featureId, dependencies);
			}
		}

		imtbase::CTreeItemModel *subFeaturesModel = featuresModelPtr->AddTreeModel("ChildModel", index);

		InsertSubFeaturesToModelFromData(packagePtr, subFeatureInfoPtr, dependenciesModelPtr, subFeaturesModel);
	}
}


imtbase::CTreeItemModel* CPackageControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	if (m_headersProviderCompPtr.IsValid()){
		imtbase::CTreeItemModel* headersInfoModelPtr = m_headersProviderCompPtr->CreateResponse(gqlRequest, errorMessage);
		if (headersInfoModelPtr != nullptr){
			imtbase::CTreeItemModel* headersModelPtr = headersInfoModelPtr->GetTreeItemModel("Headers");
			dataModelPtr->SetExternTreeModel("Headers", headersModelPtr);
		}
	}

	dataModelPtr->SetData("Id", "");
	dataModelPtr->SetData("Name", "");
	dataModelPtr->SetData("Description", "");

	QByteArray packageId = GetObjectIdFromInputParams(*gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(packageId, dataPtr)){
		const imtlic::CFeaturePackage* packagePtr  = dynamic_cast<const imtlic::CFeaturePackage*>(dataPtr.GetPtr());
		if (packagePtr != nullptr){
			dataModelPtr->SetData("Id", packagePtr->GetPackageId());

			QString name = m_objectCollectionCompPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_NAME).toString();
			dataModelPtr->SetData("Name", name);

			QString description = m_objectCollectionCompPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
			dataModelPtr->SetData("Description", description);

			imtbase::CTreeItemModel* featuresModelPtr = dataModelPtr->AddTreeModel("Items");
			imtbase::CTreeItemModel* dependenciesModelPtr = dataModelPtr->AddTreeModel("DependenciesModel");

			QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();

			for (const QByteArray& featureCollectionId : featureCollectionIds){

				const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);

				if (featureInfoPtr != nullptr){
					int index = featuresModelPtr->InsertNewItem();

					QByteArray featureId = featureInfoPtr->GetFeatureId();
					QString featureName = featureInfoPtr->GetFeatureName();
					bool isOptional = featureInfoPtr->IsOptional();

					QString featureDescription = packagePtr->GetFeatureList().GetElementInfo(featureCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

					featuresModelPtr->SetData("Id", featureId, index);
					featuresModelPtr->SetData("Name", featureName, index);
					featuresModelPtr->SetData("Optional", isOptional, index);
					featuresModelPtr->SetData("Description", featureDescription, index);

					QByteArrayList featureDependenciesList = packagePtr->GetFeatureDependencies(featureId);

					if (dependenciesModelPtr != nullptr){
						if (!featureDependenciesList.isEmpty()){
							QByteArray dependencies = featureDependenciesList.join(';');
							dependenciesModelPtr->SetData(featureId, dependencies);
						}
					}

					imtbase::CTreeItemModel* subFeaturesModel = featuresModelPtr->AddTreeModel("ChildModel", index);

					InsertSubFeaturesToModelFromData(packagePtr, featureInfoPtr, dependenciesModelPtr, subFeaturesModel);
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CPackageControllerComp::GetDependencies(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (const QByteArray& collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			const imtlic::CFeaturePackage* packagePtr  = dynamic_cast<const imtlic::CFeaturePackage*>(dataPtr.GetPtr());
			if (packagePtr != nullptr){
				imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
					if (featureInfoPtr != nullptr){
						QByteArray featureId = featureInfoPtr->GetFeatureId();
						QByteArrayList featureDependenciesList = packagePtr->GetFeatureDependencies(featureId);

						if (!featureDependenciesList.isEmpty()){
							QByteArray featureDependencies = featureDependenciesList.join(';');
							dataModelPtr->SetData(featureId, featureDependencies);
						}

						QByteArrayList subfeaturesList = featureInfoPtr->GetSubFeatureIds();

						for (const QByteArray& subfeatureId : subfeaturesList){
							QByteArrayList subfeatureDependenciesList = packagePtr->GetFeatureDependencies(subfeatureId);

							if (!subfeatureDependenciesList.empty()){
								QByteArray subfeatureDependencies = subfeatureDependenciesList.join(';');
								dataModelPtr->SetData(subfeatureId, subfeatureDependencies);
							}
						}
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CPackageControllerComp::GetTreeItemModel(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
		imtbase::CTreeItemModel* treeItemModel = dataModelPtr->AddTreeModel("TreeModel");

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		for (const QByteArray& collectionId : collectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				const imtlic::CFeaturePackage* packagePtr  = dynamic_cast<const imtlic::CFeaturePackage*>(dataPtr.GetPtr());
				if (packagePtr != nullptr){
					QByteArray packageId = packagePtr->GetPackageId();
					QString packageName = m_objectCollectionCompPtr->GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_NAME).toString();

					int index = treeItemModel->InsertNewItem();

					treeItemModel->SetData("Id", packageId, index);
					treeItemModel->SetData("Name", packageName, index);

					QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();

					imtbase::CTreeItemModel* childItemModel = treeItemModel->AddTreeModel("ChildModel", index);

					for (const QByteArray& featureCollectionId : featureCollectionIds){
						const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
						if (featureInfoPtr != nullptr){
							QString featureId = featureInfoPtr->GetFeatureId();
							QString featureName = featureInfoPtr->GetFeatureName();
							bool isOptional = featureInfoPtr->IsOptional();

							int childItemIndex = childItemModel->InsertNewItem();

							childItemModel->SetData("Id", featureId, childItemIndex);
							childItemModel->SetData("Name", featureName, childItemIndex);
							childItemModel->SetData("Optional", isOptional, childItemIndex);

							imtbase::CTreeItemModel* subFeaturesModel = childItemModel->AddTreeModel("ChildModel", childItemIndex);

							InsertSubFeaturesToModelFromData(packagePtr, featureInfoPtr, nullptr, subFeaturesModel);
						}
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


bool CPackageControllerComp::GetOperationFromRequest(
		const imtgql::CGqlRequest& gqlRequest,
		imtgql::CGqlObject& gqlObject,
		QString& errorMessage,
		int& operationType) const
{
	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	if(fieldList == nullptr){
		return false;
	}

	int count = fieldList->count();
	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "treeItem"){
			gqlObject = fieldList->at(i);
			operationType = OT_USER_OPERATION + 1;
			return true;
		}
		if (fieldList->at(i).GetId() == "dependencies"){
			gqlObject = fieldList->at(i);
			operationType = OT_USER_OPERATION + 2;
			return true;
		}
		if (BaseClass::GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
			return true;
		}
	}

	return false;
}


} // namespace imtlicgql


