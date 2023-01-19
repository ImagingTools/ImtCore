#include <imtlicgql/CProductCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CProductCollectionControllerComp::GetObjectInformation(
		const QByteArray &informationId,
		const QByteArray &objectId) const
{
	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
	if (metaInfo.IsValid()){
		if (informationId == QByteArray("Added")){
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

	QByteArray packageId = GetObjectIdFromInputParams(*gqlRequest.GetParams());

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


} // namespace imtlicgql


