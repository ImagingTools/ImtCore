#include <imtlicgql/CPackageControllerComp.h>


// ImtCore includes
#include <imtlic/CFeaturePackageCollectionUtility.h>
#include <imtlic/CFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <imtlic/CFeaturePackage.h>



namespace imtlicgql
{


imtbase::CTreeItemModel* CPackageControllerComp::ListObjects(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();
		QByteArray packageId = GetObjectIdFromInputParams(inputParams);

		QByteArrayList featureIds;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (packageId != "" && m_objectCollectionCompPtr->GetObjectData(packageId, dataPtr)){
			const imtlic::IFeaturePackage* packagePtr  = dynamic_cast<const imtlic::IFeaturePackage*>(dataPtr.GetPtr());
			if (packagePtr != nullptr){
				QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					int itemIndex = itemsModel->InsertNewItem();
					QString featureId = packagePtr->GetFeatureInfo(featureCollectionId)->GetFeatureId();
					itemsModel->SetData("Id", featureId, itemIndex);
					QString featureName = packagePtr->GetFeatureInfo(featureCollectionId)->GetFeatureName();
					itemsModel->SetData("Name", featureName, itemIndex);
					QString featureDescription = packagePtr->GetFeatureList().GetElementInfo(featureCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
					itemsModel->SetData("Description", featureDescription, itemIndex);
				}
			}
		}

		itemsModel->SetIsArray(true);

		dataModel->SetExternTreeModel("items", itemsModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


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
		featurePackagePtr->SetObjectDescription(objectId, description);


		imtbase::CTreeItemModel* featuresModelPtr = nullptr;

		if (itemModel.ContainsKey("features")){
			featuresModelPtr = itemModel.GetTreeItemModel("features");
		}

		imtbase::CTreeItemModel* dependenciesModelPtr = nullptr;
		if (itemModel.ContainsKey("dependencies")){
			dependenciesModelPtr = itemModel.GetTreeItemModel("dependencies");
		}

		if (featuresModelPtr != nullptr){
			for (int i = 0; i < featuresModelPtr->GetItemsCount(); i++){
				QByteArray featureId = featuresModelPtr->GetData("Id", i).toByteArray();
				QString featureName = featuresModelPtr->GetData("Name", i).toString();
				QString featureDescription = featuresModelPtr->GetData("Description", i).toString();

				istd::TDelPtr<imtlic::CFeatureInfo> featureInfoPtr = new imtlic::CFeatureInfo;
				featureInfoPtr->SetFeatureId(featureId);
				featureInfoPtr->SetFeatureName(featureName);

				featurePackagePtr->InsertNewObject("FeatureInfo", featureName, featureDescription, featureInfoPtr.GetPtr());

				if (dependenciesModelPtr != nullptr){
					QStringList featureDependenciesKeys = dependenciesModelPtr->GetKeys();
					for (const QString& key : featureDependenciesKeys){
						//QByteArray depFeatureId = key.split('.')[1].toUtf8();
						if (featureId == key){
							QByteArrayList values = dependenciesModelPtr->GetData(key.toUtf8()).toByteArray().split(';');
							featurePackagePtr->SetFeatureDependencies(key.toUtf8(), values);
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
//						QByteArrayList dependsIds = dependenciesProvider->GetFeatureDependencies(collectionId + "." + featureId);
						QByteArrayList dependsIds = dependenciesProvider->GetFeatureDependencies(featureId);
						if (dependsIds.size() > 0){
//							QByteArray key = collectionId + "." + featureId;
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
			treeItemModel->SetData("stateChecked", 0, index);
			treeItemModel->SetData("level", 0, index);
			treeItemModel->SetData("visible", 1, index);
			treeItemModel->SetData("isActive", 1, index);
			treeItemModel->SetData("isOpened", 1, index);

			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				const imtlic::IFeaturePackage* packagePtr  = dynamic_cast<const imtlic::IFeaturePackage*>(dataPtr.GetPtr());
				QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();

				imtbase::CTreeItemModel* childItemModel = treeItemModel->AddTreeModel("childItemModel", index);

				for (const QByteArray& featureCollectionId : featureCollectionIds){
					QString featureId = packagePtr->GetFeatureInfo(featureCollectionId)->GetFeatureId();
					QString featureName = packagePtr->GetFeatureInfo(featureCollectionId)->GetFeatureName();

					int childItemIndex = childItemModel->InsertNewItem();

					childItemModel->SetData("Id", featureId, childItemIndex);
					childItemModel->SetData("Name", featureName, childItemIndex);
					childItemModel->SetData("stateChecked", 0, childItemIndex);
					childItemModel->SetData("level", 1, childItemIndex);
					childItemModel->SetData("visible", 1, childItemIndex);
					childItemModel->SetData("isActive", 1, childItemIndex);
					childItemModel->SetData("packageId", collectionId, childItemIndex);
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


