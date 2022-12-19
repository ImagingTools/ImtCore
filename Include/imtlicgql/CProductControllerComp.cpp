#include <imtlicgql/CProductControllerComp.h>

// ImtCore includes
#include <imtlic/CLicenseInfo.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/IFeatureInfo.h>
#include <imtbase/ICollectionInfo.h>


namespace imtlicgql
{


imtbase::CHierarchicalItemModelPtr CProductControllerComp::ListObjects(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			const imtgql::IGqlContext* gqlContext,
			QString& errorMessage) const
{
	imtbase::CHierarchicalItemModelPtr rootModel(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error");
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
		if (m_objectCollectionCompPtr->GetObjectData(productId, dataPtr)){
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
			errorMessage = QObject::tr("Can not create product: %1").arg(QString(objectId));
			return nullptr;
		}

		istd::TDelPtr<imtlic::IProductLicensingInfo> productPtr = m_productFactCompPtr.CreateInstance();
		if (!productPtr.IsValid()){
			errorMessage = QT_TR_NOOP("Unable to get an product pointer");
			return nullptr;
		}

		imtbase::IObjectCollection* licenseCollectionPtr = dynamic_cast<imtbase::IObjectCollection*>(productPtr.GetPtr());
		if (licenseCollectionPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an license collection");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		if (itemModel.ContainsKey("Id")){
			objectId = itemModel.GetData("Id").toByteArray();

			if (objectId.isEmpty()){
				errorMessage = QT_TR_NOOP("Product-ID cannot be empty!");
				return nullptr;
			}

			productPtr->SetProductId(objectId);
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			productPtr->SetName(name);
		}

		imtbase::CTreeItemModel *licenses = nullptr;
		if (itemModel.ContainsKey("Items")){
			licenses = itemModel.GetTreeItemModel("Items");
		}

		imtbase::CTreeItemModel *featuresModel = nullptr;
		if (itemModel.ContainsKey("Features")){
			featuresModel = itemModel.GetTreeItemModel("Features");
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

				if (licenseId.isEmpty()){
					errorMessage = QT_TR_NOOP(licenseName + " has an empty License-ID!");
					return nullptr;
				}

				istd::TDelPtr<imtlic::CLicenseInfo> licenseInfoPtr = new imtlic::CLicenseInfo;
				if (licenseInfoPtr == nullptr){
					continue;
				}

				licenseInfoPtr->SetLicenseId(licenseId);
				licenseInfoPtr->SetLicenseName(licenseName);

				imtlic::ILicenseInfo::FeatureInfos featureInfos;
				if (featuresModel != nullptr){
					if (featuresModel->ContainsKey(licenseId)){
						imtbase::CTreeItemModel *featureModelPtr = featuresModel->GetTreeItemModel(licenseId);
						if (featureModelPtr != nullptr){
							for (int i = 0; i < featureModelPtr->GetItemsCount(); i++){
								imtlic::ILicenseInfo::FeatureInfo featureInfo;

								QByteArray featureId = featureModelPtr->GetData("Id", i).toByteArray();
								QString featureName = featureModelPtr->GetData("Name", i).toString();

								featureInfo.id = featureId;
								featureInfo.name = featureName;

								featureInfos.push_back(featureInfo);
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

	errorMessage = QObject::tr("Can not create product: %1").arg(QString(objectId));

	return nullptr;
}


imtbase::CHierarchicalItemModelPtr CProductControllerComp::GetObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			const imtgql::IGqlContext* gqlContext,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return imtbase::CHierarchicalItemModelPtr();
	}

	imtbase::CHierarchicalItemModelPtr rootModel(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();;

	QByteArray productId = GetObjectIdFromInputParams(inputParams);

	dataModel->SetData("Id", productId);
	dataModel->SetData("Name", "");

	imtbase::IObjectCollection::DataPtr dataPtr;

	if (m_headersProviderCompPtr.IsValid()){
		imtbase::CTreeItemModel* headersModel = m_headersProviderCompPtr->GetRepresentation(inputParams, QByteArrayList(), gqlContext);
		imtbase::CTreeItemModel* headers = headersModel->GetTreeItemModel("Headers");
		dataModel->SetExternTreeModel("Headers", headers);
	}

	imtbase::CTreeItemModel* licensesModelPtr = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* featuresModelPtr = dataModel->AddTreeModel("Features");

	if (m_objectCollectionCompPtr->GetObjectData(productId, dataPtr)){
		const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr.GetPtr());
		if (productPtr != nullptr){

			QString name = m_objectCollectionCompPtr->GetElementInfo(productId, imtbase::ICollectionInfo::EIT_NAME).toString();
			dataModel->SetData("Name", name);

			const imtbase::ICollectionInfo& licenseList = productPtr->GetLicenseList();
			const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();

			for ( const QByteArray& licenseId : licenseCollectionIds){
				const imtlic::ILicenseInfo* licenseInfoPtr = productPtr->GetLicenseInfo(licenseId);
				if (licenseInfoPtr == nullptr){
					continue;
				}

				int index = licensesModelPtr->InsertNewItem();

				licensesModelPtr->SetData("Id", licenseId, index);

				QString licenseName = licenseInfoPtr->GetLicenseName();
				licensesModelPtr->SetData("Name", licenseName, index);

				QString description = licenseList.GetElementInfo(licenseId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
				licensesModelPtr->SetData("Description", description, index);

				imtlic::ILicenseInfo::FeatureInfos featureInfos = licenseInfoPtr->GetFeatureInfos();

				imtbase::CTreeItemModel* featureModelPtr = nullptr;
				if (!featureInfos.empty()){
					featureModelPtr = featuresModelPtr->AddTreeModel(licenseId);
				}

				if (featureModelPtr != nullptr){
					for (const imtlic::ILicenseInfo::FeatureInfo& featureInfo : featureInfos){
						if (m_featureInfoProviderCompPtr.IsValid()){
							istd::TDelPtr<const imtlic::IFeatureInfo> featureInfoPtr = m_featureInfoProviderCompPtr->GetFeatureInfo(featureInfo.id);
							if (featureInfoPtr != nullptr){
								int index = featureModelPtr->InsertNewItem();

								QByteArray featureId = featureInfoPtr->GetFeatureId();
								QString featureName = featureInfoPtr->GetFeatureName();
								bool isOptional = featureInfoPtr->IsOptional();

								featureModelPtr->SetData("Id", featureId, index);
								featureModelPtr->SetData("Name", featureName, index);
								featureModelPtr->SetData("Optional", isOptional, index);
							}
						}
					}
				}
			}
		}
	}

	dataModel->SetExternTreeModel("Items", licensesModelPtr);

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
		if (fieldList->at(i).GetId() == "LicensesItems"){
			gqlObject = fieldList->at(i);
			operationType = OT_USER_OPERATION + 1;
			return true;
		}

//		if (fieldList->at(i).GetId() == "Features"){
//			gqlObject = fieldList->at(i);
//			operationType = OT_USER_OPERATION + 2;
//			return true;
//		}

		if (BaseClass::GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
			return true;
		}
	}

	return false;
}


imtbase::CHierarchicalItemModelPtr CProductControllerComp::GetTreeItemModel(const QList<imtgql::CGqlObject> &inputParams, const imtgql::CGqlObject &gqlObject, QString &errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return imtbase::CHierarchicalItemModelPtr();
	}

	imtbase::CHierarchicalItemModelPtr rootModel(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (const QByteArray& collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr.GetPtr());
			if (productPtr != nullptr){
				const QByteArray productId = productPtr->GetProductId();
				const QString productName = productPtr->GetName();

				int productIndex = dataModel->InsertNewItem();

				dataModel->SetData("Id", productId, productIndex);
				dataModel->SetData("Name", productName, productIndex);

				imtbase::CTreeItemModel* licensesModel = dataModel->AddTreeModel("Licenses", productIndex);
				const imtbase::ICollectionInfo& licenseList = productPtr->GetLicenseList();
				const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();
				for ( const QByteArray& licenseId : licenseCollectionIds){
					const imtlic::ILicenseInfo* licenseInfoPtr = productPtr->GetLicenseInfo(licenseId);
					if (licenseInfoPtr != nullptr){
						const QByteArray licenseId = licenseInfoPtr->GetLicenseId();
						const QString licenseName = licenseInfoPtr->GetLicenseName();

						int licenseIndex = licensesModel->InsertNewItem();

						licensesModel->SetData("Id", licenseId, licenseIndex);
						licensesModel->SetData("Name", licenseName, licenseIndex);
					}
				}
			}
		}
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtlicgql


