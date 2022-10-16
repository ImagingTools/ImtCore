#include <imtlicgql/CPackageControllerComp.h>


// ACF includes
#include <istd/TChangeDelegator.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>


namespace imtlicgql
{


istd::IChangeable* CPackageControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (inputParams.isEmpty()){
		errorMessage = QObject::tr("Can not create package: %1").arg(QString(objectId));
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<imtlic::CFeaturePackage> featurePackagePtr = new imtlic::CFeaturePackage;

		if (featurePackagePtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an feature package");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

		if (itemModel.ContainsKey("Id")){
			objectId = itemModel.GetData("Id").toByteArray();
		}

		if (objectId.isEmpty()){
			errorMessage = QT_TR_NOOP("Package-ID can not be empty!");
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

		if (featuresModelPtr != nullptr){
			for (int i = 0; i < featuresModelPtr->GetItemsCount(); i++){
				QByteArray featureId;
				if (featuresModelPtr->ContainsKey("Id", i)){
					featureId = featuresModelPtr->GetData("Id", i).toByteArray();
				}

				QString featureName;
				if (featuresModelPtr->ContainsKey("Name", i)){
					featureName = featuresModelPtr->GetData("Name", i).toString();
				}

				QString featureDescription;
				if (featuresModelPtr->ContainsKey("Description", i)){
					featureDescription = featuresModelPtr->GetData("Description", i).toString();
				}

				bool isOptional = false;
				if (featuresModelPtr->ContainsKey("Optional", i)){
					isOptional = featuresModelPtr->GetData("Optional", i).toBool();
				}

				if (featureId.isEmpty()){
					errorMessage = QT_TR_NOOP(featureName + " " + "has an empty Feature-ID!");

					return nullptr;
				}

				imtlic::CFeatureInfo* featureInfoPtr = new imtlic::CFeatureInfo;
				featureInfoPtr->SetFeatureId(featureId);
				featureInfoPtr->SetFeatureName(featureName);
				featureInfoPtr->SetOptional(isOptional);

				if (featuresModelPtr->ContainsKey("ChildModel", i)){
					imtbase::CTreeItemModel* subFeaturesModelPtr = featuresModelPtr->GetTreeItemModel("ChildModel", i);
					InsertSubFeaturesToDataFromModel(featurePackagePtr.GetPtr(), featureInfoPtr, subFeaturesModelPtr);
				}

				featurePackagePtr->InsertNewObject("FeatureInfo", featureName, featureDescription, featureInfoPtr);

				if (featuresModelPtr->ContainsKey("DependenciesModel", i)){
					imtbase::CTreeItemModel* dependenciesModel = featuresModelPtr->GetTreeItemModel("DependenciesModel", i);

					QByteArrayList featureDependencies;
					for (int i = 0; i < dependenciesModel->GetItemsCount(); i++){
						QByteArray dependencyFeatureId = dependenciesModel->GetData("Id", i).toByteArray();

						featureDependencies << dependencyFeatureId;
					}

					if (!featureDependencies.isEmpty()){
						featurePackagePtr->SetFeatureDependencies(featureId, featureDependencies);
					}
				}
			}
		}

		return featurePackagePtr.PopPtr();
	}
	errorMessage = QObject::tr("Can not create package: %1").arg(QString(objectId));

	return nullptr;
}


void CPackageControllerComp::InsertSubFeaturesToDataFromModel(
		imtlic::CFeaturePackage* packagePtr,
		imtlic::IFeatureInfo* parentFeaturePtr,
		const imtbase::CTreeItemModel* subFeaturesModelPtr) const
{
	for (int i = 0; i < subFeaturesModelPtr->GetItemsCount(); i++){
		QByteArray featureId;
		if (subFeaturesModelPtr->ContainsKey("Id", i)){
			featureId = subFeaturesModelPtr->GetData("Id", i).toByteArray();
		}

		QString featureName;
		if (subFeaturesModelPtr->ContainsKey("Name", i)){
			featureName = subFeaturesModelPtr->GetData("Name", i).toString();
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
		featureInfoPtr->SetParentFeature(parentFeaturePtr);

		packagePtr->InsertNewObject("FeatureInfo", featureName, description, featureInfoPtr);

		if (subFeaturesModelPtr->ContainsKey("DependenciesModel", i)){
			imtbase::CTreeItemModel* dependenciesModel = subFeaturesModelPtr->GetTreeItemModel("DependenciesModel", i);

			QByteArrayList featureDependencies;
			for (int i = 0; i < dependenciesModel->GetItemsCount(); i++){
				QByteArray dependencyFeatureId = dependenciesModel->GetData("Id", i).toByteArray();

				featureDependencies << dependencyFeatureId;
			}

			if (!featureDependencies.isEmpty()){
				packagePtr->SetFeatureDependencies(featureId, featureDependencies);
			}
		}

		QList<const imtlic::IFeatureInfo*> subFeatures = parentFeaturePtr->GetSubFeatures();

		if (subFeaturesModelPtr->ContainsKey("ChildModel", i)){
			imtbase::CTreeItemModel *subModelPtr = subFeaturesModelPtr->GetTreeItemModel("ChildModel", i);

			InsertSubFeaturesToDataFromModel(packagePtr, featureInfoPtr, subModelPtr);
		}
	}
}


void CPackageControllerComp::InsertSubFeaturesToModelFromData(
		const imtlic::CFeaturePackage* packagePtr,
		const imtlic::IFeatureInfo *featurePtr,
		imtbase::CTreeItemModel *featuresModel) const
{
	QList<const imtlic::IFeatureInfo*> subFeatures = featurePtr->GetSubFeatures();

	for (const imtlic::IFeatureInfo* subFeatureInfoPtr : subFeatures){
		int index = featuresModel->InsertNewItem();

		QByteArray featureId = subFeatureInfoPtr->GetFeatureId();
		QString featureName = subFeatureInfoPtr->GetFeatureName();

		QByteArray featureCollectionId = packagePtr->GetFeatureCollectionId(featureId);
		QString featureDescription = packagePtr->GetFeatureList().GetElementInfo(featureCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

		bool isOptional = subFeatureInfoPtr->IsOptional();

		featuresModel->SetData("Id", featureId, index);
		featuresModel->SetData("Name", featureName, index);
		featuresModel->SetData("Optional", isOptional, index);
		featuresModel->SetData("Description", featureDescription, index);

		featuresModel->SetData("Active", true, index);
		featuresModel->SetData("Visible", true, index);
		featuresModel->SetData("Opened", false, index);
		featuresModel->SetData("Selected", false, index);
		featuresModel->SetData("State", Qt::Unchecked, index);

//		if (isOptional){
//			featuresModel->SetData("State", Qt::Unchecked, index);
//		}
//		else{
//			featuresModel->SetData("State", Qt::Checked, index);
//		}

		imtbase::CTreeItemModel *parentModel = featuresModel->GetParent();
		int level = parentModel->GetData("Level").toInt();
		featuresModel->SetData("Level", level + 1, index);

		imtbase::CTreeItemModel* dependenciesModel = featuresModel->AddTreeModel("DependenciesModel", index);
		QByteArrayList featureDependencies = packagePtr->GetFeatureDependencies(featureId);

		for (const QByteArray& dependencyFeatureId : featureDependencies){
			int i = dependenciesModel->InsertNewItem();

			dependenciesModel->SetData("Id", dependencyFeatureId, i);

			const imtlic::IFeatureInfo* dependencyFeatureInfoPtr = packagePtr->FindFeatureById(dependencyFeatureId);
			if (dependencyFeatureInfoPtr != nullptr){
				const imtlic::IFeatureInfo* dependencyFeatureInfoPtr = packagePtr->FindFeatureById(dependencyFeatureId);
				if (dependencyFeatureInfoPtr != nullptr){
					const imtlic::IFeatureInfo* parentDependencyFeatureInfoPtr = dependencyFeatureInfoPtr->GetParentFeature();
					if (parentDependencyFeatureInfoPtr != nullptr){
						QByteArray parentFeatureId = parentDependencyFeatureInfoPtr->GetFeatureId();
						dependenciesModel->SetData("ParentId", parentFeatureId, i);
					}
				}
			}
		}

		imtbase::CTreeItemModel *subFeaturesModel = featuresModel->AddTreeModel("ChildModel", index);

		InsertSubFeaturesToModelFromData(packagePtr, subFeatureInfoPtr, subFeaturesModel);
	}
}


imtbase::CTreeItemModel* CPackageControllerComp::GetObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();;

	QByteArray packageId = GetObjectIdFromInputParams(inputParams);

	imtbase::IObjectCollection::DataPtr dataPtr;

	if (m_headersProviderCompPtr.IsValid()){
		imtbase::CTreeItemModel* headersModel = m_headersProviderCompPtr->GetTreeItemModel(inputParams, QByteArrayList());
		imtbase::CTreeItemModel* headers = headersModel->GetTreeItemModel("Headers");
		dataModel->SetExternTreeModel("Headers", headers);
	}

	imtbase::CTreeItemModel* featuresModel = dataModel->AddTreeModel("Items");

	if (m_objectCollectionCompPtr->GetObjectData(packageId, dataPtr)){
		const imtlic::CFeaturePackage* packagePtr  = dynamic_cast<const imtlic::CFeaturePackage*>(dataPtr.GetPtr());
		if (packagePtr != nullptr){
			dataModel->SetData("Id", packagePtr->GetPackageId());

			QString name = m_objectCollectionCompPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_NAME).toString();
			dataModel->SetData("Name", name);

			QString description = m_objectCollectionCompPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
			dataModel->SetData("Description", description);

			QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();

			for (const QByteArray& featureCollectionId : featureCollectionIds){

				const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);

				if (featureInfoPtr != nullptr){
					const imtlic::IFeatureInfo* parentFeatureInfoPtr = featureInfoPtr->GetParentFeature();
					if (parentFeatureInfoPtr == nullptr){
						int index = featuresModel->InsertNewItem();

						QByteArray featureId = featureInfoPtr->GetFeatureId();
						QString featureName = featureInfoPtr->GetFeatureName();
						bool isOptional = featureInfoPtr->IsOptional();

						QString featureDescription = packagePtr->GetFeatureList().GetElementInfo(featureCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

						featuresModel->SetData("Id", featureId, index);
						featuresModel->SetData("Name", featureName, index);
						featuresModel->SetData("Optional", isOptional, index);
						featuresModel->SetData("Description", featureDescription, index);

						featuresModel->SetData("Active", true, index);
						featuresModel->SetData("Visible", true, index);
						featuresModel->SetData("Opened", false, index);
						featuresModel->SetData("Selected", false, index);
						featuresModel->SetData("Level", 1, index);

						featuresModel->SetData("State", Qt::Unchecked, index);

//						if (isOptional){
//							featuresModel->SetData("State", Qt::Unchecked, index);
//						}
//						else{
//							featuresModel->SetData("State", Qt::Checked, index);
//						}

						imtbase::CTreeItemModel* dependenciesModel = featuresModel->AddTreeModel("DependenciesModel", index);

						QByteArrayList featureDependencies = packagePtr->GetFeatureDependencies(featureId);

						for (const QByteArray& dependencyFeatureId : featureDependencies){
							int index = dependenciesModel->InsertNewItem();

							dependenciesModel->SetData("Id", dependencyFeatureId, index);

							const imtlic::IFeatureInfo* dependencyFeatureInfoPtr = packagePtr->FindFeatureById(dependencyFeatureId);
							if (dependencyFeatureInfoPtr != nullptr){
								const imtlic::IFeatureInfo* dependencyFeatureInfoPtr = packagePtr->FindFeatureById(dependencyFeatureId);
								if (dependencyFeatureInfoPtr != nullptr){
									const imtlic::IFeatureInfo* parentDependencyFeatureInfoPtr = dependencyFeatureInfoPtr->GetParentFeature();
									if (parentDependencyFeatureInfoPtr != nullptr){
										QByteArray parentFeatureId = parentDependencyFeatureInfoPtr->GetFeatureId();
										dependenciesModel->SetData("ParentId", parentFeatureId, index);
									}
								}
							}
						}

						imtbase::CTreeItemModel* subFeaturesModel = featuresModel->AddTreeModel("ChildModel", index);

						InsertSubFeaturesToModelFromData(packagePtr, featureInfoPtr, subFeaturesModel);
					}
				}
			}
		}
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CPackageControllerComp::GetDependencies(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else {
		imtbase::CTreeItemModel* dataModel = rootModel->AddTreeModel("data");

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		for (const QByteArray& collectionId : collectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (!m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				continue;
			}
			const imtlic::CFeaturePackage* packagePtr  = dynamic_cast<const imtlic::CFeaturePackage*>(dataPtr.GetPtr());
			if (packagePtr != nullptr){
				imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);

					if (featureInfoPtr != nullptr){
						QByteArray featureId = featureInfoPtr->GetFeatureId();
						QByteArrayList featureDependencies = packagePtr->GetFeatureDependencies(featureId);

						if (!featureDependencies.isEmpty()){
							imtbase::CTreeItemModel* dependenciesModel = dataModel->AddTreeModel(featureId);

							for (const QByteArray& dependencyId : featureDependencies){
								int index = dependenciesModel->InsertNewItem();

								dependenciesModel->SetData("Id", dependencyId, index);

								const imtlic::IFeatureInfo* dependencyFeatureInfoPtr = packagePtr->FindFeatureById(dependencyId);
								if (dependencyFeatureInfoPtr != nullptr){
									const imtlic::IFeatureInfo* parentDependencyFeatureInfoPtr = dependencyFeatureInfoPtr->GetParentFeature();
									if (parentDependencyFeatureInfoPtr != nullptr){
										QByteArray parentFeatureId = parentDependencyFeatureInfoPtr->GetFeatureId();
										dependenciesModel->SetData("ParentId", parentFeatureId, index);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return rootModel;
}


imtbase::CTreeItemModel* CPackageControllerComp::GetTreeItemModel(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* treeItemModel = nullptr;
	imtbase::CTreeItemModel* dataModel = nullptr;

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		treeItemModel = new imtbase::CTreeItemModel();
		treeItemModel->SetIsArray(true);
		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		int index;

		for (const QByteArray& collectionId : collectionIds){
			index = treeItemModel->InsertNewItem();

			treeItemModel->SetData("Id", collectionId, index);
			treeItemModel->SetData("Name", collectionId, index);
			treeItemModel->SetData("State", Qt::Unchecked, index);
			treeItemModel->SetData("Level", 0, index);
			treeItemModel->SetData("Visible", true, index);
			treeItemModel->SetData("Active", true, index);
			treeItemModel->SetData("Opened", true, index);
			treeItemModel->SetData("Selected", false, index);

			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				const imtlic::CFeaturePackage* packagePtr  = dynamic_cast<const imtlic::CFeaturePackage*>(dataPtr.GetPtr());
				QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();

				imtbase::CTreeItemModel* childItemModel = treeItemModel->AddTreeModel("ChildModel", index);

				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
					if (featureInfoPtr != nullptr){
						const imtlic::IFeatureInfo* parentFeatureInfoPtr = featureInfoPtr->GetParentFeature();
						if (parentFeatureInfoPtr == nullptr){
							QString featureId = featureInfoPtr->GetFeatureId();
							QString featureName = featureInfoPtr->GetFeatureName();
							bool isOptional = featureInfoPtr->IsOptional();

							int childItemIndex = childItemModel->InsertNewItem();

							childItemModel->SetData("Id", featureId, childItemIndex);
							childItemModel->SetData("Name", featureName, childItemIndex);
							childItemModel->SetData("Level", 1, childItemIndex);
							childItemModel->SetData("Visible", true, childItemIndex);
							childItemModel->SetData("Active", true, childItemIndex);
							childItemModel->SetData("Opened", false, childItemIndex);
							childItemModel->SetData("Selected", false, childItemIndex);
							childItemModel->SetData("Optional", isOptional, childItemIndex);
							childItemModel->SetData("State", Qt::Unchecked, childItemIndex);
//							if (isOptional){
//								childItemModel->SetData("State", Qt::Unchecked, childItemIndex);
//							}
//							else{
//								childItemModel->SetData("State", Qt::Checked, childItemIndex);
//							}

							imtbase::CTreeItemModel* subFeaturesModel = childItemModel->AddTreeModel("ChildModel", childItemIndex);

							InsertSubFeaturesToModelFromData(packagePtr, featureInfoPtr, subFeaturesModel);
						}
					}
				}
			}
		}
		dataModel->SetExternTreeModel("TreeModel", treeItemModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
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
