#include <imtlicgql/CProductCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <istd/TDelPtr.h>
#include <iprm/CTextParam.h>
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>
#include <iprm/CParamsSet.h>
#include <iser/CCompactXmlMemReadArchive.h>
#include <iser/CCompactXmlMemWriteArchive.h>

// ImtCore includes
#include <imtlic/CLicenseDefinition.h>
#include <imtlic/CProductInfo.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/CFeatureContainer.h>


namespace imtlicgql
{


// protected methods

// reimplemented (sdl::imtlic::Products::V1_0::CProductCollectionControllerCompBase)

bool CProductCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtlic::Products::V1_0::CProductsListGqlRequest& productsListRequest,
			sdl::imtlic::Products::V1_0::CProductItem& representationObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to create representation from object. Error: Attribute 'm_objectCollectionCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return false;
	}

	QByteArray objectId = objectCollectionIterator.GetObjectId();

	imtlic::CIdentifiableProductInfo* productInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		productInfoPtr = dynamic_cast<imtlic::CIdentifiableProductInfo*>(dataPtr.GetPtr());
	}

	if (productInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return false;
	}

	idoc::MetaInfoPtr metaInfo = objectCollectionIterator.GetDataMetaInfo();

	sdl::imtlic::Products::V1_0::ProductsListRequestInfo requestInfo = productsListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.SetId(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.SetTypeId(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.SetProductId(productInfoPtr->GetProductId());
	}

	if (requestInfo.items.isNameRequested){
		representationObject.SetName(productInfoPtr->GetName());
	}

	if (requestInfo.items.isProductNameRequested){
		representationObject.SetProductName(productInfoPtr->GetName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.SetDescription(productInfoPtr->GetProductDescription());
	}

	if (requestInfo.items.isCategoryIdRequested){
		representationObject.SetCategoryId(productInfoPtr->GetCategoryId());
	}

	if (requestInfo.items.isFeaturesRequested){
		representationObject.SetFeatures(productInfoPtr->GetFeatures()->GetElementIds().join(';'));
	}

	if (requestInfo.items.isLicensesRequested){
		QList<sdl::imtlic::Products::V1_0::CLicenseData> licenseDataList;

		if (m_licenseCollectionCompPtr.IsValid()){
			iprm::CIdParam idParam;
			idParam.SetId(objectId);

			iprm::CParamsSet paramsSet1;
			paramsSet1.SetEditableParameter("ProductId", &idParam);

			iprm::CParamsSet filterParam;
			filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

			imtbase::ICollectionInfo::Ids licenseCollectionIds = m_licenseCollectionCompPtr->GetElementIds(0, -1, &filterParam);

			for (const imtbase::ICollectionInfo::Id& licenseCollectionId : licenseCollectionIds){
				imtbase::IObjectCollection::DataPtr licenseDataPtr;
				if (m_licenseCollectionCompPtr->GetObjectData(licenseCollectionId, licenseDataPtr)){
					const imtlic::CLicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::CLicenseDefinition*>(licenseDataPtr.GetPtr());
					if (licenseInfoPtr != nullptr){
						sdl::imtlic::Products::V1_0::CLicenseData licenseData;

						licenseData.SetId(licenseCollectionId);
						licenseData.SetLicenseId(licenseInfoPtr->GetLicenseId());
						licenseData.SetLicenseName(licenseInfoPtr->GetLicenseName());

						licenseDataList << licenseData;
					}
				}
			}
		}

		representationObject.SetLicenses(licenseDataList);
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetAdded(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetLastModified(lastModified);
	}

	return true;
}


istd::IChangeable* CProductCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtlic::Products::V1_0::CProductData& productDataRepresentation,
			QByteArray& newObjectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (!m_productInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_productInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtlic::IProductInfo> productInstancePtr = m_productInfoFactCompPtr.CreateInstance();
	if (!productInstancePtr.IsValid()){
		errorMessage = QString("Unable to create product instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return nullptr;
	}

	imtlic::CIdentifiableProductInfo* productInfoPtr = dynamic_cast<imtlic::CIdentifiableProductInfo*>(productInstancePtr.GetPtr());
	if (productInfoPtr == nullptr){
		errorMessage = QString("Unable to cast product instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return nullptr;
	}

	newObjectId = productDataRepresentation.GetId();
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	productInfoPtr->SetObjectUuid(newObjectId);

	QByteArray productId = productDataRepresentation.GetProductId();

	if (productId.isEmpty()){
		errorMessage = QT_TR_NOOP("Product name cannot be empty");
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	iprm::CIdParam idParam;
	idParam.SetId(productId);

	iprm::CParamsSet paramsSet1;
	paramsSet1.SetEditableParameter("ProductId", &idParam);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		QByteArray id = collectionIds[0];
		if (newObjectId != id){
			errorMessage = QT_TR_NOOP(QString("Product '%1' already exists")).arg(qPrintable(productId));

			return nullptr;
		}
	}

	productInfoPtr->SetProductId(productId);

	name = productDataRepresentation.GetProductName();
	if (name.isEmpty()){
		errorMessage = QT_TR_NOOP("Unable to create product with an empty product name");
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	iprm::CIdParam nameParam;
	nameParam.SetId(name.toUtf8());

	iprm::CParamsSet paramsSet2;
	paramsSet2.SetEditableParameter("ProductName", &nameParam);

	iprm::CParamsSet filterParam2;
	filterParam2.SetEditableParameter("ObjectFilter", &paramsSet2);

	imtbase::ICollectionInfo::Ids collectionIds2 = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam2);
	if (!collectionIds2.isEmpty()){
		QByteArray id = collectionIds2[0];
		if (newObjectId != id){
			errorMessage = QT_TR_NOOP(QString("Product name '%1' already exists, please rename")).arg(qPrintable(name));

			return nullptr;
		}
	}

	productInfoPtr->SetName(name);

	description = productDataRepresentation.GetDescription();
	productInfoPtr->SetProductDescription(description);

	QByteArray categoryId = productDataRepresentation.GetCategoryId();
	productInfoPtr->SetCategoryId(categoryId);

	QByteArray features = productDataRepresentation.GetFeatures();
	if (!features.isEmpty()){
		QByteArrayList featureIds = features.split(';');

		for (const QByteArray& featureId : featureIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_featureCollectionCompPtr->GetObjectData(featureId, dataPtr)){
				const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
				if (featureInfoPtr != nullptr){
					productInfoPtr->AddFeature(featureId, *featureInfoPtr);
				}
			}
		}
	}

	return productInstancePtr.PopPtr();
}


bool CProductCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtlic::Products::V1_0::CProductItemGqlRequest& productItemRequest,
			sdl::imtlic::Products::V1_0::CProductDataPayload& representationPayload,
			QString& errorMessage) const
{
	imtlic::CIdentifiableProductInfo* productInfoPtr = const_cast<imtlic::CIdentifiableProductInfo*>(dynamic_cast<const imtlic::CIdentifiableProductInfo*>(&data));
	if (productInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return false;
	}

	sdl::imtlic::Products::V1_0::ProductItemRequestArguments arguments = productItemRequest.GetRequestedArguments();

	sdl::imtlic::Products::V1_0::CProductData productData;

	QByteArray id = arguments.input.GetId();
	productData.SetId(id);

	QByteArray productId = productInfoPtr->GetProductId();
	productData.SetProductId(productId);

	QString productName = productInfoPtr->GetName();
	productData.SetName(productName);
	productData.SetProductName(productName);

	QString description = productInfoPtr->GetProductDescription();
	productData.SetDescription(description);

	QByteArray categoryId = productInfoPtr->GetCategoryId();
	productData.SetCategoryId(categoryId);

	productData.SetFeatures(productInfoPtr->GetFeatures()->GetElementIds().join(';'));

	representationPayload.SetProductData(productData);

	return true;
}


imtbase::CTreeItemModel* CProductCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel);
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray packageId = GetObjectIdFromInputParams(gqlRequest.GetParams());

	int index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Modification Time"), index);
	imtbase::CTreeItemModel* children = dataModelPtr->AddTreeModel("Children", index);

	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(packageId);
	if (metaInfo.IsValid()){
		QString date = metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME).toDateTime().toString("dd.MM.yyyy hh:mm:ss");
		children->SetData("Value", date);
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(packageId, dataPtr)){
		errorMessage = QT_TR_NOOP("Unable to load an object data");

		return nullptr;
	}

	imtbase::IObjectCollection* licensePtr = dynamic_cast<imtbase::IObjectCollection*>(dataPtr.GetPtr());
	if (licensePtr != nullptr){
		QByteArrayList licenseCollectionIds = licensePtr->GetElementIds().toList();
		index = dataModelPtr->InsertNewItem();

		dataModelPtr->SetData("Name", "Licenses", index);
		children = dataModelPtr->AddTreeModel("Children", index);
		int childIndex;
		for (const QByteArray& licenseCollectionId : licenseCollectionIds){
			childIndex = children->InsertNewItem();
			QString licenseName = licensePtr->GetElementInfo(licenseCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString();
			QString value = licenseName + " (" + licenseCollectionId + ")";
			children->SetData("Value", value, childIndex);
		}
	}

	return rootModelPtr.PopPtr();
}


void CProductCollectionControllerComp::SetObjectFilter(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			const imtbase::CTreeItemModel& objectFilterModel,
			iprm::CParamsSet& filterParams) const
{
	if (objectFilterModel.ContainsKey("CategoryId")){
		QByteArray filterValue = objectFilterModel.GetData("CategoryId").toByteArray();
		if (!filterValue.isEmpty()){
			iprm::CTextParam* textParamPtr = new iprm::CTextParam();
			textParamPtr->SetText(filterValue);
			filterParams.SetEditableParameter("CategoryId", textParamPtr, true);
		}
	}
}


imtbase::CTreeItemModel* CProductCollectionControllerComp::RenameObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to get object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QString newName = inputParamPtr->GetFieldArgumentValue("NewName").toByteArray();

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		imtlic::CIdentifiableProductInfo* productInfoPtr = dynamic_cast<imtlic::CIdentifiableProductInfo*>(dataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			QString oldName = productInfoPtr->GetName();
			if (newName != oldName){
				productInfoPtr->SetName(newName);
				productInfoPtr->SetProductId(newName.replace(" ", "").toUtf8());

				istd::TDelPtr<imtbase::IOperationContext> operationContextPtr =  nullptr;
				if (m_operationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Update", objectId, *productInfoPtr);
				}

				if (!m_objectCollectionCompPtr->SetObjectData(objectId, *productInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
					errorMessage = QString("Error when trying rename product with ID '%1'").arg(qPrintable(objectId));
					SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

					return nullptr;
				}
			}
		}
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	dataModelPtr->SetData("Id", objectId);
	dataModelPtr->SetData("Name", newName);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CProductCollectionControllerComp::ImportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> resultModelPtr = BaseClass::ImportObject(gqlRequest, errorMessage);
	if (resultModelPtr.IsValid()){
		if (m_objectCollectionCompPtr.IsValid()){
			const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
			if (inputParamPtr != nullptr){
				bool force = inputParamPtr->GetFieldArgumentValue("force").toBool();
				if (force){
					QByteArray objectId = resultModelPtr->GetData("id").toByteArray();
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
						imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
						if (productInfoPtr != nullptr){
							imtbase::IObjectCollection* featureCollectionPtr = productInfoPtr->GetFeatures();
							if (featureCollectionPtr != nullptr){
								imtbase::ICollectionInfo::Ids featureIds = m_featureCollectionCompPtr->GetElementIds();
								imtbase::ICollectionInfo::Ids productFeatureIds = featureCollectionPtr->GetElementIds();
								for (const imtbase::ICollectionInfo::Id& productFeatureId : productFeatureIds){
									imtbase::IObjectCollection::DataPtr dataPtr;
									if (featureCollectionPtr->GetObjectData(productFeatureId, dataPtr)){
										const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
										if (featureInfoPtr != nullptr){
											if (!featureIds.contains(productFeatureId)){
												QByteArray result = m_featureCollectionCompPtr->InsertNewObject("", "", "", featureInfoPtr, productFeatureId);
												if (result.isEmpty()){
													SendWarningMessage(0, QString("Unable to insert new feature with ID '%1' from product serialization").arg(qPrintable(productFeatureId)), "CProductControllerComp");
												}
											}
											else{
												if (!m_featureCollectionCompPtr->SetObjectData(productFeatureId, *featureInfoPtr)){
													SendWarningMessage(0, QString("Unable to update feature with ID '%1' from product serialization").arg(qPrintable(productFeatureId)), "CProductControllerComp");
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return resultModelPtr.PopPtr();
}


QString CProductCollectionControllerComp::GetExtensionFromMimeType(const imtbase::CMimeType& mimeType) const
{
	if (mimeType.GetSubType() == "xml"){
		return mimeType.GetSubType();
	}

	return QString();
}


QString CProductCollectionControllerComp::GetExportFileName(const QByteArray& objectId) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			return productInfoPtr->GetProductId() + "Features";
		}
	}

	return QString();
}


} // namespace imtlicgql


