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

		QByteArrayList featureIds;
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

istd::IChangeable* CProductControllerComp::CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray& objectId,
																	 QString& name, QString& description, QString &errorMessage) const
{
	if (inputParams.isEmpty()){
		return nullptr;
	}
	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){

		istd::TDelPtr<imtlic::CLicenseInstance> licenseInstancePtr = new imtlic::CLicenseInstance;
		imtbase::IObjectCollection* licenseCollectionPtr = dynamic_cast<imtbase::IObjectCollection*>(licenseInstancePtr.GetPtr());
		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

		objectId = itemModel.GetData("Id").toByteArray();
		name = itemModel.GetData("Name").toString();

		licenseInstancePtr->SetLicenseId(objectId);

		imtbase::CTreeItemModel *licenses = itemModel.GetTreeItemModel("licenses");

		for (int i = 0; i < licenses->GetItemsCount(); i++){
			QByteArray licenseId = licenses->GetData("Id", i).toByteArray();
			QString licenseName = licenses->GetData("Name", i).toString();
			QString licenseDescription = licenses->GetData("Description", i).toString();

			istd::TDelPtr<imtlic::CLicenseInfo> licenseInfoPtr = new imtlic::CLicenseInfo;
			licenseInfoPtr->SetLicenseId(licenseId);
			licenseInfoPtr->SetLicenseName(licenseName);
			licenseCollectionPtr->InsertNewObject("ProductInfo", licenseName, licenseDescription, licenseInfoPtr.GetPtr());
		}

		return licenseInstancePtr.PopPtr();
	}
	return nullptr;
}

imtbase::CTreeItemModel* CProductControllerComp::GetTreeItemModel(
		const QList<imtgql::CGqlObject>& inputParams,
		const imtgql::CGqlObject& gqlObject,
		QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* treeItemModel = nullptr;
	imtbase::CTreeItemModel* dataModel = nullptr;
	bool isSetResponce = false;
	QByteArrayList fields;

	if (!m_viewDelegateCompPtr.IsValid()) {
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()) {
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else {
		dataModel = new imtbase::CTreeItemModel();
		treeItemModel = new imtbase::CTreeItemModel();
		treeItemModel->SetIsArray(true);
		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		int index;
		for (const QByteArray& collectionId : collectionIds) {
			index = treeItemModel->InsertNewItem();

			treeItemModel->SetData("Name", collectionId, index);
			treeItemModel->SetData("stateChecked", 0, index);
			treeItemModel->SetData("level", 0, index);

			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)) {
				const imtlic::IFeaturePackage* packagePtr  = dynamic_cast<const imtlic::IFeaturePackage*>(dataPtr.GetPtr());
				QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();

				imtbase::CTreeItemModel* childItemModel = treeItemModel->AddTreeModel("childItemModel", index);

				for (const QByteArray& featureCollectionId : featureCollectionIds) {
					QString featureId = packagePtr->GetFeatureInfo(featureCollectionId)->GetFeatureId();
					QString featureName = packagePtr->GetFeatureInfo(featureCollectionId)->GetFeatureName();

					int childItemIndex = childItemModel->InsertNewItem();

					childItemModel->SetData("Name", featureName, childItemIndex);
					childItemModel->SetData("stateChecked", 0, childItemIndex);
					childItemModel->SetData("level", 1, childItemIndex);
				}
			}
		}
		dataModel->SetExternTreeModel("TreeModel", treeItemModel);
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


