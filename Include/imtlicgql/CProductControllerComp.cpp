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
					QByteArray key = objectId + '.' + licenseId;
					if (dependencies->ContainsKey(key)){
						QStringList valuesList = dependencies->GetData(key).toString().split(';');
						imtlic::ILicenseInfo::FeatureInfo featureInfo;
						for (const QString& value : valuesList){
							featureInfo.id = value.toUtf8();
							featureInfos.push_back(featureInfo);
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

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else {
		dataModel = new imtbase::CTreeItemModel();
		dependenciesModel = new imtbase::CTreeItemModel();

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
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
						QByteArray key = productPtr->GetProductId() + "." + licenseId;
						QString value;
						for (int i = 0; i < featureInfos.size(); i++){
							if (i != 0){
								value += ';';
							}
							value += featureInfos[i].id;
						}
						dependenciesModel->SetData(key, value);
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


