#include <imtlicgql/CProductCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <istd/TDelPtr.h>
#include <iprm/CTextParam.h>
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CLicenseDefinition.h>
#include <imtlic/CProductInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

bool CProductCollectionControllerComp::SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const
{
	if (objectCollectionIterator == nullptr){
		return false;
	}

	bool retVal = true;

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		imtlic::IProductInfo* productLicensingInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (objectCollectionIterator->GetObjectData(dataPtr)){
			productLicensingInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
		}

		QByteArray collectionId = objectCollectionIterator->GetObjectId();

		if (productLicensingInfoPtr != nullptr){
			for (const QByteArray& informationId : informationIds){
				QVariant elementInformation;

				if(informationId == "Id"){
					elementInformation = collectionId;
				}
				if(informationId == "ProductId"){
					elementInformation = productLicensingInfoPtr->GetProductId();
				}
				else if(informationId == "ProductName"){
					elementInformation = productLicensingInfoPtr->GetName();
				}
				else if(informationId == "ProductDescription"){
					elementInformation = productLicensingInfoPtr->GetProductDescription();
				}
				else if(informationId == "Added"){
					QDateTime addedTime =  objectCollectionIterator->GetElementInfo("Added").toDateTime();
					elementInformation = addedTime.toString("dd.MM.yyyy hh:mm:ss");
				}
				else if(informationId == "LastModified"){
					QDateTime lastTime =  objectCollectionIterator->GetElementInfo("LastModified").toDateTime();
					elementInformation = lastTime.toString("dd.MM.yyyy hh:mm:ss");
				}
				else if(informationId == "CategoryId"){
					elementInformation = productLicensingInfoPtr->GetCategoryId();
				}
				else if(informationId == "Features"){
					QByteArrayList featureList = productLicensingInfoPtr->GetFeatureIds();

					elementInformation = featureList.join(';');
				}
				else if(informationId == "Licenses"){
					if (m_licenseCollectionCompPtr.IsValid()){
						imtbase::CTreeItemModel* licenseModelPtr = model.AddTreeModel("Licenses", itemIndex);

						iprm::CIdParam idParam;
						idParam.SetId(collectionId);

						iprm::CParamsSet paramsSet1;
						paramsSet1.SetEditableParameter("ProductId", &idParam);

						iprm::CParamsSet filterParam;
						filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

						imtbase::ICollectionInfo::Ids licenseCollectionIds = m_licenseCollectionCompPtr->GetElementIds(0, -1, &filterParam);

						for (const imtbase::ICollectionInfo::Id& licenseCollectionId : licenseCollectionIds){
							imtbase::IObjectCollection::DataPtr dataPtr;
							if (m_licenseCollectionCompPtr->GetObjectData(licenseCollectionId, dataPtr)){
								const imtlic::CLicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::CLicenseDefinition*>(dataPtr.GetPtr());
								if (licenseInfoPtr != nullptr){
									int index = licenseModelPtr->InsertNewItem();

									licenseModelPtr->SetData("Id", licenseCollectionId, index);
									licenseModelPtr->SetData("LicenseId", licenseInfoPtr->GetLicenseId(), index);
									licenseModelPtr->SetData("LicenseName", licenseInfoPtr->GetLicenseName(), index);
								}
							}
						}
					}

					continue;
				}

				if (elementInformation.isNull()){
					elementInformation = "";
				}

				retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
			}
		}

		return true;
	}

	return false;
}


imtbase::CTreeItemModel* CProductCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

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


void CProductCollectionControllerComp::SetObjectFilter(const imtgql::CGqlRequest &gqlRequest,
			const imtbase::CTreeItemModel &objectFilterModel,
			iprm::CParamsSet &filterParams) const
{
	if (objectFilterModel.ContainsKey("CategoryId")){
		QByteArray filterValue = objectFilterModel.GetData("CategoryId").toByteArray();
		if (!filterValue.isEmpty()){
			istd::TDelPtr<iprm::CIdParam> textParamPtr(new iprm::CIdParam());
			textParamPtr->SetId(filterValue);
			filterParams.SetEditableParameter("CategoryId", textParamPtr.PopPtr());
		}
	}
}


imtbase::CTreeItemModel* CProductCollectionControllerComp::RenameObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to get object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return nullptr;
	}

	QString oldName;

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QString newName = inputParamPtr->GetFieldArgumentValue("NewName").toByteArray();

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		imtlic::CIdentifiableProductInfo* productInfoPtr = dynamic_cast<imtlic::CIdentifiableProductInfo*>(dataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			oldName = productInfoPtr->GetName();
			if (newName != oldName){
				productInfoPtr->SetName(newName);
				productInfoPtr->SetProductId(newName.replace(" ", "").toUtf8());

				imtbase::IOperationContext* operationContextPtr =  nullptr;
				if (m_operationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, objectId, productInfoPtr);
				}

				if (!m_objectCollectionCompPtr->SetObjectData(objectId, *productInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
					errorMessage = QString("Error when trying rename product with ID %1").arg(qPrintable(objectId));
					SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

					return nullptr;
				}
			}
		}
	}

	SendInfoMessage(0, QString("The product %1 successfully renamed to %2").arg(oldName).arg(newName));

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	dataModelPtr->SetData("Id", objectId);
	dataModelPtr->SetData("Name", newName);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CProductCollectionControllerComp::SetObjectDescription(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to get object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QString description = inputParamPtr->GetFieldArgumentValue("Description").toByteArray();

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		imtlic::CIdentifiableProductInfo* productInfoPtr = dynamic_cast<imtlic::CIdentifiableProductInfo*>(dataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			QString oldDescription = productInfoPtr->GetProductDescription();
			if (description != oldDescription){

				productInfoPtr->SetProductDescription(description);
				imtbase::IOperationContext* operationContextPtr =  nullptr;
				if (m_operationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, objectId, productInfoPtr);
				}

				if (!m_objectCollectionCompPtr->SetObjectData(objectId, *productInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
					errorMessage = QString("Error when trying set description product with ID %1").arg(qPrintable(objectId));
					SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

					return nullptr;
				}
			}
		}
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	dataModelPtr->SetData("Id", objectId);
	dataModelPtr->SetData("Description", description);

	return rootModelPtr.PopPtr();
}


} // namespace imtlicgql


