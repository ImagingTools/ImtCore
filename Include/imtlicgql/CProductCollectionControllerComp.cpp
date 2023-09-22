#include <imtlicgql/CProductCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <istd/TDelPtr.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtlic/CLicenseInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

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
		imtlic::IProductLicensingInfo* productLicensingInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (objectCollectionIterator->GetObjectData(dataPtr)){
			productLicensingInfoPtr = dynamic_cast<imtlic::IProductLicensingInfo*>(dataPtr.GetPtr());
		}

		QByteArray collectionId = objectCollectionIterator->GetObjectId();

		if (productLicensingInfoPtr != nullptr){
			for (const QByteArray& informationId : informationIds){
				QVariant elementInformation;

				if(informationId == "Id"){
					elementInformation = productLicensingInfoPtr->GetProductId();
				}
				else if(informationId == "Name"){
					elementInformation = productLicensingInfoPtr->GetName();
				}
				else if(informationId == "Description"){
					elementInformation = objectCollectionIterator->GetElementInfo("Description");
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
				else if(informationId == "Licenses"){
					imtbase::CTreeItemModel* licenseModelPtr = model.AddTreeModel("Licenses", itemIndex);

					imtbase::IObjectCollection* licenseCollectionPtr = dynamic_cast<imtbase::IObjectCollection*>(productLicensingInfoPtr);
					if (licenseCollectionPtr == nullptr){
						return false;
					}

					imtbase::ICollectionInfo::Ids elementIds = licenseCollectionPtr->GetElementIds();
					for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
						imtbase::IObjectCollection::DataPtr elementDataPtr;
						if (licenseCollectionPtr->GetObjectData(elementId, elementDataPtr)){
							const imtlic::CLicenseInfo* licenseInfoPtr = dynamic_cast<const imtlic::CLicenseInfo*>(elementDataPtr.GetPtr());
							if (licenseInfoPtr != nullptr){
								int index = licenseModelPtr->InsertNewItem();

								licenseModelPtr->SetData("Id", licenseInfoPtr->GetLicenseId(), index);
								licenseModelPtr->SetData("Name", licenseInfoPtr->GetLicenseName(), index);
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


QVariant CProductCollectionControllerComp::GetObjectInformation(
		const QByteArray &informationId,
		const QByteArray &objectId) const
{
	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
	if (metaInfo.IsValid()){
		if (informationId == QByteArray("CategoryId")){
			return metaInfo->GetMetaInfo(imtlic::IProductLicensingInfo::MIT_PRODUCT_CATEGORY_ID);
		}
		else if (informationId == QByteArray("Added")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
		else if (informationId == QByteArray("LastModified")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
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
			istd::TDelPtr<iprm::CTextParam> textParamPtr(new iprm::CTextParam());
			textParamPtr->SetText(filterValue);
			filterParams.SetEditableParameter("CategoryId", textParamPtr.PopPtr());
		}
	}
}


} // namespace imtlicgql


