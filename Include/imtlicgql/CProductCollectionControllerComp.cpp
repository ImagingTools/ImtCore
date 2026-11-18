// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CProductCollectionControllerComp.h>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QMap>
#include <QtCore/QTemporaryDir>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Products.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/CLicenseDefinition.h>
#include <imtlic/CProductInfo.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlicgql/imtlicgql.h>
#include <imtlicgql/ProductRepresentation.h>


namespace imtlicgql
{


// protected methods


sdl::V1_0::imtbase::CImportObjectPayload CProductCollectionControllerComp::OnImportObject(
			const sdl::V1_0::imtbase::CImportObjectGqlRequest& importObjectRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!CanImportProduct(importObjectRequest, errorMessage)){
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		sdl::V1_0::imtbase::CImportObjectPayload response;
		response.success = false;

		return response;
	}

	sdl::V1_0::imtbase::CImportObjectPayload response = BaseClass::OnImportObject(importObjectRequest, gqlRequest, errorMessage);
	if (response.success && *response.success && response.objectId){
		WriteImportedFeatures(*response.objectId);
	}

	return response;
}


void CProductCollectionControllerComp::WriteImportedFeatures(const QByteArray& objectId) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	imtlic::IProductInfo* productInfoPtr = nullptr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
	}

	imtbase::IObjectCollection* featureCollectionPtr = productInfoPtr != nullptr ? productInfoPtr->GetFeatures() : nullptr;
	if (featureCollectionPtr == nullptr){
		return;
	}

	const imtbase::ICollectionInfo::Ids catalogFeatureIds = m_featureCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& productFeatureId : featureCollectionPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr productFeatureDataPtr;
		if (!featureCollectionPtr->GetObjectData(productFeatureId, productFeatureDataPtr)){
			continue;
		}

		const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(productFeatureDataPtr.GetPtr());
		if (featureInfoPtr == nullptr){
			continue;
		}

		if (!catalogFeatureIds.contains(productFeatureId)){
			if (m_featureCollectionCompPtr->InsertNewObject(QByteArrayLiteral("Feature"), "", "", featureInfoPtr, productFeatureId).isEmpty()){
				SendWarningMessage(0, QStringLiteral("Unable to insert new feature with ID '%1' from product serialization").arg(productFeatureId), "CProductCollectionControllerComp");
			}
		}
		else if (!m_featureCollectionCompPtr->SetObjectData(productFeatureId, *featureInfoPtr)){
			SendWarningMessage(0, QStringLiteral("Unable to update feature with ID '%1' from product serialization").arg(productFeatureId), "CProductCollectionControllerComp");
		}
	}
}


bool CProductCollectionControllerComp::CanImportProduct(
			const sdl::V1_0::imtbase::CImportObjectGqlRequest& importObjectRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::ImportObjectRequestArguments arguments = importObjectRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->fileData || !arguments.input->mimeType){
		return true;
	}

	imtbase::CMimeType mimeType;
	const int index = GetMimeTypeIndex(*arguments.input->mimeType);
	if (index < 0 || index >= m_filePersistenceCompPtr.GetCount() || m_filePersistenceCompPtr[index] == nullptr || !mimeType.FromString(*arguments.input->mimeType)){
		// The import itself reports what is wrong with the request.
		return true;
	}

	QTemporaryDir tempDir;
	const QString filePath = tempDir.path() + QStringLiteral("/Product.") + GetExtensionFromMimeType(mimeType);
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)){
		return true;
	}
	file.write(QByteArray::fromBase64(*arguments.input->fileData));
	file.close();

	imtlic::CIdentifiableProductInfo importedProduct;
	if (m_filePersistenceCompPtr[index]->LoadFromFile(importedProduct, filePath) != ifile::IFilePersistence::OS_OK){
		return true;
	}

	// One product id names one product: the import creates products, it does not replace them.
	const QByteArray productId = importedProduct.GetProductId();
	for (const imtbase::ICollectionInfo::Id& objectId : m_objectCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(dataPtr.GetPtr());
			if (productInfoPtr != nullptr && productInfoPtr->GetProductId() == productId){
				errorMessage = QStringLiteral("The product '%1' already exists").arg(QString::fromUtf8(productId));

				return false;
			}
		}
	}

	// A root feature already in the catalog under another uuid would give two features one path.
	QMap<QByteArray, QByteArray> catalogRootIds;
	for (const imtbase::ICollectionInfo::Id& catalogFeatureId : m_featureCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_featureCollectionCompPtr->GetObjectData(catalogFeatureId, dataPtr)){
			const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
			if (featureInfoPtr != nullptr){
				catalogRootIds.insert(featureInfoPtr->GetFeatureId(), catalogFeatureId);
			}
		}
	}

	QStringList conflicts;
	imtbase::IObjectCollection* featureCollectionPtr = importedProduct.GetFeatures();
	if (featureCollectionPtr != nullptr){
		for (const imtbase::ICollectionInfo::Id& productFeatureId : featureCollectionPtr->GetElementIds()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (!featureCollectionPtr->GetObjectData(productFeatureId, dataPtr)){
				continue;
			}

			const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
			const QByteArray catalogId = featureInfoPtr != nullptr ? catalogRootIds.value(featureInfoPtr->GetFeatureId()) : QByteArray();
			if (!catalogId.isEmpty() && catalogId != productFeatureId){
				conflicts << QString::fromUtf8(featureInfoPtr->GetFeatureId());
			}
		}
	}

	if (!conflicts.isEmpty()){
		errorMessage = QStringLiteral("The feature catalog already has these root features under another uuid: %1").arg(conflicts.join(QStringLiteral(", ")));

		return false;
	}

	return true;
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
		QByteArray typeId = objectCollectionIterator.GetObjectTypeId();
		if (typeId.isEmpty()){
			typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		}

		representationObject.typeId = typeId;
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
		// Same lookup order as the collection's EIT_DESCRIPTION, but from the already fetched record
		QString description;
		idoc::MetaInfoPtr collectionMetaInfoPtr = objectCollectionIterator.GetCollectionMetaInfo();
		if (collectionMetaInfoPtr.IsValid()){
			description = collectionMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
		}

		if (description.isEmpty() && metaInfo.IsValid()){
			description = metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
		}

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

	FillRepresentationFromProduct(*productInfoPtr, representationPayload);

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

	QByteArrayList unresolvedRequirements;
	FillProductFromRepresentation(productDataRepresentation, *m_featureCollectionCompPtr, *productInfoPtr, &unresolvedRequirements);

	for (const QByteArray& unresolvedRequirement : unresolvedRequirements){
		SendWarningMessage(
				0,
				QStringLiteral("Product '%1' requires '%2', which no feature of the catalog carries")
						.arg(productInfoPtr->GetName(), QString::fromUtf8(unresolvedRequirement)),
				"CProductCollectionControllerComp");
	}

	return CheckProductIdentity(*productInfoPtr, objectId, *m_objectCollectionCompPtr, errorMessage);
}


} // namespace imtlicgql


