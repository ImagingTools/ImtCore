// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CProductCollectionControllerComp.h>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Products.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/CLicenseDefinition.h>
#include <imtlic/CProductInfo.h>
#include <imtlic/IFeatureInfo.h>


namespace imtlicgql
{


// protected methods


sdl::V1_0::imtbase::CImportObjectPayload CProductCollectionControllerComp::OnImportObject(
			const sdl::V1_0::imtbase::CImportObjectGqlRequest& importObjectRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CImportObjectPayload response = BaseClass::OnImportObject(importObjectRequest, gqlRequest, errorMessage);
	if (response.success){
		bool success = *response.success;
		if (success){
			QByteArray objectId;
			if (response.objectId){
				objectId = *response.objectId;
			}

			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
				imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					imtbase::IObjectCollection* featureCollectionPtr = productInfoPtr->GetFeatures();
					if (featureCollectionPtr != nullptr){
						imtbase::ICollectionInfo::Ids featureIds = m_featureCollectionCompPtr->GetElementIds();
						imtbase::ICollectionInfo::Ids productFeatureIds = featureCollectionPtr->GetElementIds();
						for (const imtbase::ICollectionInfo::Id& productFeatureId : productFeatureIds){
							imtbase::IObjectCollection::DataPtr productFeatureDataPtr;
							if (featureCollectionPtr->GetObjectData(productFeatureId, productFeatureDataPtr)){
								const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(productFeatureDataPtr.GetPtr());
								if (featureInfoPtr != nullptr){
									if (!featureIds.contains(productFeatureId)){
										QByteArray result = m_featureCollectionCompPtr->InsertNewObject(QByteArrayLiteral("Feature"), "", "", featureInfoPtr, productFeatureId);
										if (result.isEmpty()){
											SendWarningMessage(0, QStringLiteral("Unable to insert new feature with ID '%1' from product serialization").arg(productFeatureId), "CProductControllerComp");
										}
									}
									else{
										if (!m_featureCollectionCompPtr->SetObjectData(productFeatureId, *featureInfoPtr)){
											SendWarningMessage(0, QStringLiteral("Unable to update feature with ID '%1' from product serialization").arg(productFeatureId), "CProductControllerComp");
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

	return response;
}


void CProductCollectionControllerComp::OnAfterSetObjectName(
			const QByteArray& objectId,
			const QString& /*oldName*/,
			const QString& newName,
			const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		imtlic::CIdentifiableProductInfo* productInfoPtr = dynamic_cast<imtlic::CIdentifiableProductInfo*>(dataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			productInfoPtr->SetName(newName);
			QByteArray newId = newName.toUtf8();
			newId.replace(" ", "");
			productInfoPtr->SetProductId(newId);

			istd::TDelPtr<imtbase::IOperationContext> operationContextPtr =  nullptr;
			if (m_operationContextControllerCompPtr.IsValid()){
				operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Update", objectId, productInfoPtr);
			}

			m_objectCollectionCompPtr->SetObjectData(objectId, *productInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr());
		}
	}
}


// reimplemented (sdl::V1_0::imtlic::CProductCollectionControllerCompBase)

bool CProductCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::V1_0::imtlic::CProductsListGqlRequest& productsListRequest,
			sdl::V1_0::imtlic::CProductItem& representationObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to create representation from object. Error: Attribute 'm_objectCollectionCompPtr' was not set");
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
		errorMessage = QStringLiteral("Unable to create representation from object '%1'").arg(objectId);
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return false;
	}

	idoc::MetaInfoPtr metaInfo = objectCollectionIterator.GetDataMetaInfo();

	sdl::V1_0::imtlic::ProductsListRequestInfo requestInfo = productsListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.productId = QByteArray(productInfoPtr->GetProductId());
	}

	if (requestInfo.items.isNameRequested){
		representationObject.name = QString(productInfoPtr->GetName());
	}

	if (requestInfo.items.isProductNameRequested){
		representationObject.productName = QString(productInfoPtr->GetName());
	}

	if (requestInfo.items.isDescriptionRequested){
		QString description = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::ElementInfoType::EIT_DESCRIPTION).toString();
		representationObject.description = description;
	}

	if (requestInfo.items.isCategoryIdRequested){
		representationObject.categoryId = QByteArray(productInfoPtr->GetCategoryId());
	}

	if (requestInfo.items.isFeaturesRequested){
		representationObject.features = QByteArray(productInfoPtr->GetFeatures()->GetElementIds().toList().join(';'));
	}

	if (requestInfo.items.isOptionalFeaturesRequested){
		representationObject.optionalFeatures = CreateOptionalFeaturesRepresentation(*productInfoPtr);
	}

	if (requestInfo.items.isLicensesRequested){
		imtsdl::TElementList<sdl::V1_0::imtlic::CLicenseData> licenseDataList;

		if (m_licenseCollectionCompPtr.IsValid()){
			imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
			fieldFilter.fieldId = "ProductId";
			fieldFilter.filterValue = objectId;

			imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
			groupFilter.fieldFilters << fieldFilter;

			imtbase::CComplexCollectionFilter complexFilter;
			complexFilter.SetFilterExpression(groupFilter);

			iprm::CParamsSet filterParam;
			filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

			imtbase::ICollectionInfo::Ids licenseCollectionIds = m_licenseCollectionCompPtr->GetElementIds(0, -1, &filterParam);

			for (const imtbase::ICollectionInfo::Id& licenseCollectionId : licenseCollectionIds){
				imtbase::IObjectCollection::DataPtr licenseDataPtr;
				if (m_licenseCollectionCompPtr->GetObjectData(licenseCollectionId, licenseDataPtr)){
					const imtlic::CLicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::CLicenseDefinition*>(licenseDataPtr.GetPtr());
					if (licenseInfoPtr != nullptr){
						sdl::V1_0::imtlic::CLicenseData licenseData;

						licenseData.id = QByteArray(licenseCollectionId);
						licenseData.licenseId = QByteArray(licenseInfoPtr->GetLicenseId());
						licenseData.licenseName = QString(licenseInfoPtr->GetLicenseName());
						licenseData.name = licenseData.licenseName;

						licenseDataList << licenseData;
					}
				}
			}
		}

		representationObject.licenses = std::move(licenseDataList);
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime().toUTC();

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.added = QString(added);
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("TimeStamp").toDateTime().toUTC();

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.timeStamp = QString(lastModified);
	}

	return true;
}


istd::IChangeableUniquePtr CProductCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::V1_0::imtlic::CProductData& productDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_productInfoFactCompPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to create object from representation. Error: Attribute 'm_productInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return nullptr;
	}

	imtlic::IProductInfoUniquePtr productInstancePtr = m_productInfoFactCompPtr.CreateInstance();
	if (!productInstancePtr.IsValid()){
		errorMessage = QStringLiteral("Unable to create product instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return nullptr;
	}

	if (!FillObjectFromRepresentation(productDataRepresentation, *productInstancePtr, newObjectId, errorMessage)){
		errorMessage = QStringLiteral("Unable to create product instance from representation. Error: '%1'");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return nullptr;
	}

	return productInstancePtr;
}


bool CProductCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::V1_0::imtlic::CProductItemGqlRequest& productItemRequest,
			sdl::V1_0::imtlic::CProductData& representationPayload,
			QString& errorMessage) const
{
	imtlic::CIdentifiableProductInfo* productInfoPtr = const_cast<imtlic::CIdentifiableProductInfo*>(dynamic_cast<const imtlic::CIdentifiableProductInfo*>(&data));
	if (productInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return false;
	}

	sdl::V1_0::imtlic::ProductItemRequestArguments arguments = productItemRequest.GetRequestedArguments();

	QByteArray id;
	if (arguments.input->id){
		id = *arguments.input->id;
	}
	representationPayload.id = QByteArray(id);

	QByteArray productId = productInfoPtr->GetProductId();
	representationPayload.productId = QByteArray((productId));

	QString productName = productInfoPtr->GetName();
	representationPayload.name = QString((productName));
	representationPayload.productName = QString((productName));

	QString description = productInfoPtr->GetProductDescription();
	representationPayload.description = QString((description));

	QByteArray categoryId = productInfoPtr->GetCategoryId();
	representationPayload.categoryId = QByteArray((categoryId));

	representationPayload.features = QByteArray((productInfoPtr->GetFeatures()->GetElementIds().toList().join(';')));
	representationPayload.optionalFeatures = CreateOptionalFeaturesRepresentation(*productInfoPtr);

	return true;
}


bool CProductCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::V1_0::imtlic::CProductUpdateGqlRequest& productUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	imtlic::CIdentifiableProductInfo* productInfoPtr = dynamic_cast<imtlic::CIdentifiableProductInfo*>(&object);
	if (productInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to update object from representation. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return false;
	}

	sdl::V1_0::imtlic::ProductUpdateRequestArguments arguments = productUpdateRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->id){
		objectId = *productUpdateRequest.GetRequestedArguments().input->id;
	}

	sdl::V1_0::imtlic::CProductData productData;
	if (arguments.input->item){
		productData = *productUpdateRequest.GetRequestedArguments().input->item;
	}

	productInfoPtr->ResetData();

	productInfoPtr->SetObjectUuid(objectId);

	if (!FillObjectFromRepresentation(productData, *productInfoPtr, objectId, errorMessage)){
		errorMessage = QStringLiteral("Unable to update product from representation. Error: '%1'").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return false;
	}

	return true;
}


QString CProductCollectionControllerComp::GetExtensionFromMimeType(const imtbase::CMimeType& mimeType) const
{
	if (mimeType.GetSubType() == "xml"){
		return mimeType.GetSubType();
	}

	if (mimeType.GetSubType() == "x-c"){
		return QStringLiteral("h");
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


// private methods

bool CProductCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::V1_0::imtlic::CProductData& productDataRepresentation,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& errorMessage) const
{
	imtlic::CIdentifiableProductInfo* productInfoPtr = dynamic_cast<imtlic::CIdentifiableProductInfo*>(&object);
	if (productInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to cast product instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return false;
	}

	if (productDataRepresentation.id){
		objectId = *productDataRepresentation.id;
	}
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	productInfoPtr->SetObjectUuid(objectId);

	QByteArray productId;
	if (productDataRepresentation.productId){
		productId = *productDataRepresentation.productId;
	}

	if (productId.isEmpty()){
		errorMessage = QT_TR_NOOP("Product Name cannot be empty");
		SendErrorMessage(0, errorMessage, "Product controller");

		return false;
	}

	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "ProductId";
	fieldFilter.filterValue = productId;

	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(groupFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		QByteArray id = collectionIds[0];
		if (objectId != id){
			errorMessage = QT_TR_NOOP(QStringLiteral("Product '%1' already exists")).arg(productId);

			return false;
		}
	}

	productInfoPtr->SetProductId(productId);

	QString name;
	QString description;

	if (productDataRepresentation.productName){
		name = *productDataRepresentation.productName;
	}
	if (name.isEmpty()){
		errorMessage = QT_TR_NOOP("Product Name is empty");
		SendErrorMessage(0, errorMessage, "Product controller");

		return false;
	}

	imtbase::IComplexCollectionFilter::FieldFilter nameFieldFilter;
	nameFieldFilter.fieldId = "ProductName";
	nameFieldFilter.filterValue = name;

	imtbase::IComplexCollectionFilter::FilterExpression nameGroupFilter;
	nameGroupFilter.fieldFilters << nameFieldFilter;

	imtbase::CComplexCollectionFilter nameComplexFilter;
	nameComplexFilter.SetFilterExpression(nameGroupFilter);

	iprm::CParamsSet nameFilterParam;
	nameFilterParam.SetEditableParameter("ComplexFilter", &nameComplexFilter);

	imtbase::ICollectionInfo::Ids collectionIds2 = m_objectCollectionCompPtr->GetElementIds(0, -1, &nameFilterParam);
	if (!collectionIds2.isEmpty()){
		QByteArray id = collectionIds2[0];
		if (objectId != id){
			errorMessage = QT_TR_NOOP(QStringLiteral("Product name '%1' already exists, please rename")).arg(name);

			return false;
		}
	}

	productInfoPtr->SetName(name);

	if (productDataRepresentation.description){
		description = *productDataRepresentation.description;
	}
	productInfoPtr->SetProductDescription(description);

	QByteArray categoryId;
	if (productDataRepresentation.categoryId){
		categoryId = *productDataRepresentation.categoryId;
	}
	productInfoPtr->SetCategoryId(categoryId);

	QByteArray features;
	if (productDataRepresentation.features){
		features = *productDataRepresentation.features;
	}

	QByteArrayList featureIds;
	if (!features.isEmpty()){
		featureIds = features.split(';');

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

	// Kept out of the loop above on purpose. Every id in "features" is a feature
	// document id and is looked up as one; the ids below are nodes inside such a
	// document and are never resolved against the collection. featureId is only
	// matched against what the product actually has, to drop stale entries.
	imtlic::IProductInfo::OptionalFeatureInfos optionalFeatures;
	if (productDataRepresentation.optionalFeatures){
		for (const sdl::V1_0::imtlic::CProductOptionalFeature& optionalFeature : productDataRepresentation.optionalFeatures->ToList()){
			if (!optionalFeature.featureId){
				continue;
			}

			const QByteArray featureId = *optionalFeature.featureId;
			if (featureId.isEmpty() || !featureIds.contains(featureId)){
				continue;
			}

			imtlic::IProductInfo::OptionalFeatureInfo optionalFeatureInfo;
			optionalFeatureInfo.featureId = featureId;

			if (optionalFeature.subFeatureIds){
				optionalFeatureInfo.subFeatureIds = optionalFeature.subFeatureIds->ToList();
				optionalFeatureInfo.subFeatureIds.removeAll(QByteArray());
			}

			if (!optionalFeatureInfo.subFeatureIds.isEmpty()){
				optionalFeatures << optionalFeatureInfo;
			}
		}
	}

	productInfoPtr->SetOptionalFeatures(optionalFeatures);

	return true;
}


// private methods

imtsdl::TElementList<sdl::V1_0::imtlic::CProductOptionalFeature> CProductCollectionControllerComp::CreateOptionalFeaturesRepresentation(
			const imtlic::IProductInfo& productInfo)
{
	imtsdl::TElementList<sdl::V1_0::imtlic::CProductOptionalFeature> retVal;

	const imtlic::IProductInfo::OptionalFeatureInfos optionalFeatures = productInfo.GetOptionalFeatures();
	for (const imtlic::IProductInfo::OptionalFeatureInfo& optionalFeature : optionalFeatures){
		sdl::V1_0::imtlic::CProductOptionalFeature representation;
		representation.featureId = optionalFeature.featureId;
		representation.subFeatureIds.Emplace();
		representation.subFeatureIds->FromList(optionalFeature.subFeatureIds);

		retVal << representation;
	}

	return retVal;
}


} // namespace imtlicgql


