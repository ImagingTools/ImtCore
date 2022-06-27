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


imtbase::CTreeItemModel* CFeaturePackageCollectionControllerComp::GetMetaInfo(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* metaInfoModel = nullptr;
	imtbase::CTreeItemModel* childs = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		metaInfoModel = new imtbase::CTreeItemModel();

		idoc::CStandardDocumentMetaInfo metaInfo;

		QByteArray packageId = GetObjectIdFromInputParams(inputParams);

		int index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", QT_TR_NOOP("Modification Time"), index);
		childs = metaInfoModel->AddTreeModel("Childs", index);

		if (m_objectCollectionCompPtr->GetCollectionItemMetaInfo(packageId, metaInfo)){
			QString date = metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME).toDateTime().toString("dd.MM.yyyy hh:mm:ss");
			childs->SetData("Value", date);
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

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", QT_TR_NOOP("Features"), index);
		childs = metaInfoModel->AddTreeModel("Childs", index);
		int childIndex;

		for (const QByteArray& featureCollectionId : featureCollectionIds){
			const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);

			if (featureInfoPtr != nullptr){
				childIndex = childs->InsertNewItem();
				QByteArray featureId = featureInfoPtr->GetFeatureId();
				QString featureName = featureInfoPtr->GetFeatureName();
				childs->SetData("Value", featureName, childIndex);
			}
		}
		dataModel->SetExternTreeModel("metaInfo", metaInfoModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtlicgql


