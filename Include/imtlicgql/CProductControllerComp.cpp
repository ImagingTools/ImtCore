#include <imtlicgql/CProductControllerComp.h>

// ImtCore includes
#include <imtlic/CFeaturePackageCollectionUtility.h>
//#include <imtlic/CFeatureInfo.h>
//#include <imtlic/IFeaturePackage.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>

#include <imtlic/CLicenseInstance.h>
#include <imtlic/CLicenseInfo.h>

#include <imtlic/IProductLicensingInfo.h>

#include <imtlic/ILicenseInstance.h>
#include <imtlic/CFeaturePackage.h>

#include <imtbase/ICollectionInfo.h>



namespace imtlicgql
{


imtbase::CTreeItemModel* CProductControllerComp::ListObjects(
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
		QByteArray productId = GetObjectIdFromInputParams(inputParams);

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (productId != "" && m_objectCollectionCompPtr->GetObjectData(productId, dataPtr)){
			imtbase::IObjectCollection* licensePtr = dynamic_cast<imtbase::IObjectCollection*>(dataPtr.GetPtr());

			if (licensePtr != nullptr){
				QByteArrayList licenseCollectionIds = licensePtr->GetElementIds().toList();
				for (const QByteArray& licenseCollectionId : licenseCollectionIds){
					int itemIndex = itemsModel->InsertNewItem();
					QString licenseName = licensePtr->GetElementInfo(licenseCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString();
					QString licenseDescription = licensePtr->GetElementInfo(licenseCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
					itemsModel->SetData("Id", licenseCollectionId, itemIndex);
					itemsModel->SetData("Name", licenseName, itemIndex);
					itemsModel->SetData("Description", licenseDescription, itemIndex);
				}
			}
		}

		itemsModel->SetIsArray(true);
		dataModel->SetExternTreeModel("items", itemsModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


istd::IChangeable* CProductControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray& objectId,
		QString& name,
		QString& description,
		QString &errorMessage) const
{
	if (inputParams.isEmpty()){
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		if (!m_productFactCompPtr.IsValid()){
			return nullptr;
		}

		istd::TDelPtr<imtlic::IProductLicensingInfo> productPtr = m_productFactCompPtr.CreateInstance();
		if (!productPtr.IsValid()){
			return nullptr;
		}

		imtbase::IObjectCollection* licenseCollectionPtr = dynamic_cast<imtbase::IObjectCollection*>(productPtr.GetPtr());
		if (licenseCollectionPtr == nullptr){
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

		if (itemModel.ContainsKey("Id")){
			objectId = itemModel.GetData("Id").toByteArray();
			productPtr->SetProductId(objectId);
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			productPtr->SetName(name);
		}

		imtbase::CTreeItemModel *licenses = nullptr;

		if (itemModel.ContainsKey("licenses")){
			licenses = itemModel.GetTreeItemModel("licenses");
		}

		imtbase::CTreeItemModel *dependencies = nullptr;

		if (itemModel.ContainsKey("dependencies")){
			dependencies = itemModel.GetTreeItemModel("dependencies");
		}

		if (licenses != nullptr){
			for (int i = 0; i < licenses->GetItemsCount(); i++){
				QByteArray licenseId;
				QString licenseDescription, licenseName;

				if (licenses->ContainsKey("Id")){
					licenseId = licenses->GetData("Id", i).toByteArray();
				}

				if (licenses->ContainsKey("Name")){
					licenseName = licenses->GetData("Name", i).toString();
				}

				if (licenses->ContainsKey("Description")){
					licenseDescription = licenses->GetData("Description", i).toString();
				}

				istd::TDelPtr<imtlic::CLicenseInfo> licenseInfoPtr = new imtlic::CLicenseInfo;
				if (licenseInfoPtr == nullptr){
					continue;
				}

				licenseInfoPtr->SetLicenseId(licenseId);
				licenseInfoPtr->SetLicenseName(licenseName);

				imtlic::ILicenseInfo::FeatureInfos featureInfos;
				if (dependencies != nullptr){
					for (int i = 0; i < dependencies->GetItemsCount(); i++){
						QByteArray rootLicenseId = dependencies->GetData("RootLicenseId", i).toByteArray();
						QByteArray rootProductId = dependencies->GetData("RootProductId", i).toByteArray();

						QByteArrayList featuresDependencies;
						imtbase::CTreeItemModel* packagesModel = dependencies->GetTreeItemModel("Packages", i);

						if (rootLicenseId == licenseId){
							for (int j = 0; j < packagesModel->GetItemsCount(); j++){
								QByteArray packageId = packagesModel->GetData("Id", j).toByteArray();
								imtbase::CTreeItemModel* childModel = packagesModel->GetTreeItemModel("Features", j);
								imtlic::ILicenseInfo::FeatureInfo featureInfo;

								for (int k = 0; k < childModel->GetItemsCount(); k++){
									QByteArray featureId = childModel->GetData("Id", k).toByteArray();
									featureInfo.id = packageId + "." + featureId;
									featureInfos.push_back(featureInfo);
								}
							}
						}
					}
				}
				licenseInfoPtr->SetFeatureInfos(featureInfos);
				licenseCollectionPtr->InsertNewObject(imtlic::CLicenseInfo::GetTypeId(), licenseName, licenseDescription, licenseInfoPtr.GetPtr(), licenseId);
			}
		}

		return productPtr.PopPtr();
	}

	return nullptr;
}

imtbase::CTreeItemModel* CProductControllerComp::GetDependencies(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dependenciesModel = nullptr;
	imtbase::CTreeItemModel* dataModel = nullptr;
	QByteArrayList fields;

	if (!m_viewDelegateCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else {
		dataModel = new imtbase::CTreeItemModel();
		dependenciesModel = new imtbase::CTreeItemModel();

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		int rootIndex;

		for (const QByteArray& collectionId : collectionIds){

			imtbase::IObjectCollection::DataPtr dataPtr;
			if (collectionId != "" && m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){

				const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr.GetPtr());

				const imtbase::ICollectionInfo& licenseList = productPtr->GetLicenseList();
				const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();

				for ( const QByteArray& licenseId : licenseCollectionIds){
					const imtlic::ILicenseInfo* licenseInfoPtr = productPtr->GetLicenseInfo(licenseId);

					if (licenseInfoPtr == nullptr){
						continue;
					}

					imtlic::ILicenseInfo::FeatureInfos featureInfos = licenseInfoPtr->GetFeatureInfos();

					if (featureInfos.size() > 0){

						rootIndex = dependenciesModel->InsertNewItem();
						dependenciesModel->SetData("RootProductId", collectionId, rootIndex);
						dependenciesModel->SetData("RootLicenseId", licenseId, rootIndex);

						imtbase::CTreeItemModel* packagesModel;

						if (!dependenciesModel->ContainsKey("Packages", rootIndex)){
							packagesModel = dependenciesModel->AddTreeModel("Packages", rootIndex);
						}
						else{
							packagesModel = dependenciesModel->GetTreeItemModel("Packages", rootIndex);
						}

						for (imtlic::ILicenseInfo::FeatureInfo& featureInfo : featureInfos){
							QByteArrayList data = featureInfo.id.split('.');

							int packageIndex = -1;
							if (packagesModel->GetItemsCount() > 0){
								for (int i = 0; i < packagesModel->GetItemsCount(); i++){
									QByteArray packageItemId = packagesModel->GetData("Id", i).toByteArray();

									if (packageItemId == data[0]){
										packageIndex = i;
										break;
									}
								}
							}

							if (packageIndex == -1){
								packageIndex = packagesModel->InsertNewItem();
								packagesModel->SetData("Id", data[0], packageIndex);
							}

							imtbase::CTreeItemModel* childModel;
							if (!packagesModel->ContainsKey("Features", packageIndex)){
								childModel = packagesModel->AddTreeModel("Features", packageIndex);
							}
							else{
								childModel = packagesModel->GetTreeItemModel("Features", packageIndex);
							}

							int childItemIndex = childModel->InsertNewItem();
							childModel->SetData("Id", data[1], childItemIndex);
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

bool CProductControllerComp::GetOperationFromRequest(
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


