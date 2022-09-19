#include <imtlicgql/CPackageControllerComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>
#include <imtlic/CFeaturePackage.h>


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

		imtbase::CTreeItemModel* dependenciesModelPtr = nullptr;
		if (itemModel.ContainsKey("Dependencies")){
			dependenciesModelPtr = itemModel.GetTreeItemModel("Dependencies");
		}

		if (featuresModelPtr != nullptr){
			for (int i = 0; i < featuresModelPtr->GetItemsCount(); i++){
				QByteArray featureId = featuresModelPtr->GetData("Id", i).toByteArray();
				QString featureName = featuresModelPtr->GetData("Name", i).toString();
				QString featureDescription = featuresModelPtr->GetData("Description", i).toString();

				if (featureId.isEmpty()){
					errorMessage = QT_TR_NOOP(featureName + " " + "has an empty Feature-ID!");

					return nullptr;
				}

				istd::TDelPtr<imtlic::CFeatureInfo> featureInfoPtr = new imtlic::CFeatureInfo;
				featureInfoPtr->SetFeatureId(featureId);
				featureInfoPtr->SetFeatureName(featureName);

				featurePackagePtr->InsertNewObject("FeatureInfo", featureName, featureDescription, featureInfoPtr.GetPtr());

//				QString name = featureInfoPtr->GetFeatureName();

				if (dependenciesModelPtr != nullptr){
					QStringList featureDependenciesKeys = dependenciesModelPtr->GetKeys();
					for (const QString& key : featureDependenciesKeys){
						if (featureId == key){
							QByteArrayList values = dependenciesModelPtr->GetData(key.toUtf8()).toByteArray().split(';');
							featurePackagePtr->SetFeatureDependencies(key.toUtf8(), values);
						}
					}

//					for (const QString& key : featureDependenciesKeys){
//						QByteArrayList features = featurePackagePtr->GetFeatureDependencies(key.toUtf8());

//						for (const QByteArray& feature : features){
////							featurePackagePtr->GetFeatureInfo()
//						}
//					}
				}
			}
		}

		return featurePackagePtr.PopPtr();
	}
	errorMessage = QObject::tr("Can not create package: %1").arg(QString(objectId));

	return nullptr;
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

	imtbase::CTreeItemModel* featuresModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dependenciesModel = dataModel->AddTreeModel("Dependencies");

	if (m_objectCollectionCompPtr->GetObjectData(packageId, dataPtr)){
		const imtlic::IFeaturePackage* packagePtr  = dynamic_cast<const imtlic::IFeaturePackage*>(dataPtr.GetPtr());
		if (packagePtr != nullptr){
			const imtlic::IFeatureDependenciesProvider* dependenciesProvider = packagePtr->GetDependenciesInfoProvider();

			dataModel->SetData("Id", packagePtr->GetPackageId());

			QString name = m_objectCollectionCompPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_NAME).toString();
			dataModel->SetData("Name", name);

			QString description = m_objectCollectionCompPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
			dataModel->SetData("Description", description);

			QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();

			for (const QByteArray& featureCollectionId : featureCollectionIds){

				const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);

				int index = featuresModel->InsertNewItem();

				QByteArray featureId = featureInfoPtr->GetFeatureId();
				QString featureName = featureInfoPtr->GetFeatureName();
				QString featureDescription = packagePtr->GetFeatureList().GetElementInfo(featureCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

				featuresModel->SetData("Id", featureId, index);
				featuresModel->SetData("Name", featureName, index);
				featuresModel->SetData("Description", featureDescription, index);

				QByteArrayList featureDependencies = dependenciesProvider->GetFeatureDependencies(featureId);

				if (featureDependencies.size() > 0){
					QByteArray key = featureId;
					QByteArray value = featureDependencies.join(";");
					dependenciesModel->SetData(key, value);
				}
			}
		}
	}

	dataModel->SetExternTreeModel("Items", featuresModel);

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CPackageControllerComp::GetDependencies(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dependenciesModel = nullptr;
	imtbase::CTreeItemModel* dataModel = nullptr;

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else {
		dataModel = new imtbase::CTreeItemModel();
		dependenciesModel = new imtbase::CTreeItemModel();
		dependenciesModel->SetIsArray(true);
		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		for (const QByteArray& collectionId : collectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (!m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				continue;
			}

			const imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<const imtlic::IFeatureInfoProvider*>(dataPtr.GetPtr());
			if (packagePtr != nullptr){
				const imtlic::IFeatureDependenciesProvider* dependenciesProvider = packagePtr->GetDependenciesInfoProvider();
				imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
					if (featureInfoPtr != nullptr){
						QByteArray featureId = featureInfoPtr->GetFeatureId();
						QByteArrayList dependsIds = dependenciesProvider->GetFeatureDependencies(featureId);
						if (dependsIds.size() > 0){
							QByteArray key = featureId;
							QString value = dependsIds.join(';');
							dependenciesModel->SetData(key, value);
						}
					}
				}
			}
		}
		dataModel->SetExternTreeModel("TreeModel", dependenciesModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

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

			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				const imtlic::IFeaturePackage* packagePtr  = dynamic_cast<const imtlic::IFeaturePackage*>(dataPtr.GetPtr());
				QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();

				imtbase::CTreeItemModel* childItemModel = treeItemModel->AddTreeModel("ChildModel", index);

				for (const QByteArray& featureCollectionId : featureCollectionIds){
					QString featureId = packagePtr->GetFeatureInfo(featureCollectionId)->GetFeatureId();
					QString featureName = packagePtr->GetFeatureInfo(featureCollectionId)->GetFeatureName();

					int childItemIndex = childItemModel->InsertNewItem();

					childItemModel->SetData("Id", featureId, childItemIndex);
					childItemModel->SetData("Name", featureName, childItemIndex);
					childItemModel->SetData("State", Qt::Unchecked, childItemIndex);
					childItemModel->SetData("Level", 1, childItemIndex);
					childItemModel->SetData("Visible", true, childItemIndex);
					childItemModel->SetData("Active", true, childItemIndex);
					childItemModel->SetData("Opened", false, childItemIndex);
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
