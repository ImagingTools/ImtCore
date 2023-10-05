#include <imtlicgql/CFeatureCollectionControllerComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

QVariant CFeatureCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	if (informationId == QByteArray("Added")){
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);;
		if (metaInfo.IsValid()){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
	}
	else if (informationId == QByteArray("LastModified")){
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
		if (metaInfo.IsValid()){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CFeatureCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
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
		errorMessage = QT_TR_NOOP("Unable to load an object data!");
		return nullptr;
	}

	const imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<const imtlic::IFeatureInfoProvider*>(dataPtr.GetPtr());
	if (packagePtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to get an package info provider!");
		return nullptr;
	}

	imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Features"), index);
	children = dataModelPtr->AddTreeModel("Children", index);
	int childIndex;

	for (const QByteArray& featureCollectionId : featureCollectionIds){
		const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);

		if (featureInfoPtr != nullptr){
			childIndex = children->InsertNewItem();
			QByteArray featureId = featureInfoPtr->GetFeatureId();
			QString featureName = featureInfoPtr->GetFeatureName();
			children->SetData("Value", featureName, childIndex);
		}
	}

	return rootModelPtr.PopPtr();
}


bool CFeatureCollectionControllerComp::SetupGqlItem(
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

	QByteArray objectId = objectCollectionIterator->GetObjectId();

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		for (QByteArray informationId : informationIds){
			QVariant elementInformation;

			QByteArray json = objectCollectionIterator->GetElementInfo("Document").toByteArray();
			QJsonDocument jsonDocument = QJsonDocument::fromJson(json);

			if(informationId == "Id"){
				elementInformation = objectId;
			}
			else if(informationId == "FeatureId"){
				elementInformation = jsonDocument["Id"].toString();

			}
			else if(informationId == "Name"){
				elementInformation = jsonDocument["Name"].toString();
			}
			else if(informationId == "Description"){
			}
			else if(informationId == "Added"){
				QDateTime addedTime =  objectCollectionIterator->GetElementInfo("Added").toDateTime();
				elementInformation = addedTime.toString("dd.MM.yyyy hh:mm:ss");
			}
			else if(informationId == "LastModified"){
				QDateTime lastTime =  objectCollectionIterator->GetElementInfo("LastModified").toDateTime();
				elementInformation = lastTime.toString("dd.MM.yyyy hh:mm:ss");
			}

			if (elementInformation.isNull()){
				elementInformation = "";
			}

			retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
		}

		return true;
	}

	return false;
}


} // namespace imtlicgql


