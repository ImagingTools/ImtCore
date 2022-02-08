#include <imtlicgql/CFeaturePackageCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtlic/CFeatureInfo.h>
#include <imtlic/CFeaturePackage.h>

namespace imtlicgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CFeaturePackageCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	if (informationId == QByteArray("Added")){
		idoc::CStandardDocumentMetaInfo metaInfo;
		if (m_objectCollectionCompPtr->GetCollectionItemMetaInfo(objectId, metaInfo)){
			return metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
	}
	else if (informationId == QByteArray("ModificationTime")){
		idoc::CStandardDocumentMetaInfo metaInfo;
		if (m_objectCollectionCompPtr->GetCollectionItemMetaInfo(objectId, metaInfo)){
			return metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CFeaturePackageCollectionControllerComp::SetObjectDescription(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();

	if (!itemData.isEmpty()){
		const imtlic::CFeaturePackage* currentPackagePtr = nullptr;
		imtbase::IObjectCollection::DataPtr objectPtr;

		imtbase::CTreeItemModel itemModel;
		imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* itemsModel = new imtbase::CTreeItemModel();

		itemModel.Parse(itemData);

		QByteArray objectId;
		if (itemModel.ContainsKey("Id")){
			objectId = itemModel.GetData("Id").toByteArray();

			itemsModel->SetData("Id", objectId);
		}

		QString description;
		if (itemModel.ContainsKey("Description")){
			description = itemModel.GetData("Description").toString();

			itemsModel->SetData("Description", description);
		}

		if (m_objectCollectionCompPtr->GetObjectData(objectId, objectPtr)){
			currentPackagePtr = dynamic_cast<const imtlic::CFeaturePackage*>(objectPtr.GetPtr());
		}

		if (currentPackagePtr == nullptr){
			return nullptr;
		}

		m_objectCollectionCompPtr->SetObjectDescription(objectId, description);

		dataModel->SetExternTreeModel("item", itemsModel);
		rootModel->SetExternTreeModel("data", dataModel);

		return rootModel;
	}

	return nullptr;
}

imtbase::CTreeItemModel* CFeaturePackageCollectionControllerComp::RenameObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();

	if (!itemData.isEmpty()){
		const imtlic::CFeaturePackage* currentPackagePtr = nullptr;
		imtbase::IObjectCollection::DataPtr objectPtr;

		imtbase::CTreeItemModel itemModel;
		imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* itemsModel = new imtbase::CTreeItemModel();

		itemModel.Parse(itemData);

		QByteArray objectId;
		if (itemModel.ContainsKey("Id")){
			objectId = itemModel.GetData("Id").toByteArray();

			itemsModel->SetData("Id", objectId);
		}

		QString newObjectName;
		if (itemModel.ContainsKey("NewName")){
			newObjectName = itemModel.GetData("NewName").toString();

			itemsModel->SetData("NewName", newObjectName);
			itemsModel->SetData("NewId", newObjectName);
		}

		if (m_objectCollectionCompPtr->GetObjectData(objectId, objectPtr)){
			currentPackagePtr = dynamic_cast<const imtlic::CFeaturePackage*>(objectPtr.GetPtr());
		}

		if (currentPackagePtr == nullptr){
			return nullptr;
		}

		m_objectCollectionCompPtr->SetObjectName(objectId, newObjectName);

		dataModel->SetExternTreeModel("item", itemsModel);
		rootModel->SetExternTreeModel("data", dataModel);

		return rootModel;
	}

	return nullptr;
}


imtbase::CTreeItemModel* CFeaturePackageCollectionControllerComp::GetMetaInfo(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();

	if (!itemData.isEmpty()){

		imtbase::CTreeItemModel itemModel;
		imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* itemsModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* features = new imtbase::CTreeItemModel();

		itemModel.Parse(itemData);

		QByteArray objectId;
		if (itemModel.ContainsKey("Id")){
			objectId = itemModel.GetData("Id").toByteArray();

			itemsModel->SetData("Id", objectId);
		}

		idoc::CStandardDocumentMetaInfo metaInfo;

		if (m_objectCollectionCompPtr->GetCollectionItemMetaInfo(objectId, metaInfo)){
//			metaInfo.GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
//										.toDateTime().toString(imtgui::CObjectCollectionViewDelegate::s_dateTimeFormat);
			QVariant date = metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME).toDateTime();

			itemsModel->SetData("ModificationTime", date);
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr);

		const imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<const imtlic::IFeatureInfoProvider*>(dataPtr.GetPtr());

		if (packagePtr != nullptr){
			imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
			int index;
			for (const QByteArray& featureCollectionId : featureCollectionIds){
				const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);

				if (featureInfoPtr != nullptr){
					index = features->InsertNewItem();
					QByteArray featureId = featureInfoPtr->GetFeatureId();
					QString featureName = featureInfoPtr->GetFeatureName();

					features->SetData("Id", featureId, index);
					features->SetData("Name", featureName, index);
				}
			}

			itemsModel->SetExternTreeModel("features", features);
		}

		dataModel->SetExternTreeModel("item", itemsModel);
		rootModel->SetExternTreeModel("data", dataModel);

		return rootModel;
	}

	return nullptr;
}


} // namespace imtlicgql


