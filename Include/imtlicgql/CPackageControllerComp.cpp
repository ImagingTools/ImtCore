#include <imtlicgql/CPackageControllerComp.h>

// ImtCore includes
#include <imtlic/CFeaturePackageCollectionUtility.h>
#include <imtlic/CFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>


namespace imtlicgql
{


imtbase::CTreeItemModel* CPackageControllerComp::ListObjects(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	bool isSetResponce = false;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();
		QByteArray packageId = GetObjectIdFromInputParams(inputParams);

		QByteArrayList featureIds;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(packageId, dataPtr)){
			const imtlic::IFeaturePackage* packagePtr  = dynamic_cast<const imtlic::IFeaturePackage*>(dataPtr.GetPtr());
			if (packagePtr != nullptr){
				QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					int itemIndex = itemsModel->InsertNewItem();
					QString featureId = packagePtr->GetFeatureInfo(featureCollectionId)->GetFeatureId();
					itemsModel->SetData("Id", featureId, itemIndex);
					itemsModel->SetData("FeatureId", featureId, itemIndex);
					QString featureName = packagePtr->GetFeatureInfo(featureCollectionId)->GetFeatureName();
					itemsModel->SetData("FeatureName", featureName, itemIndex);
					QString featureDescription = packagePtr->GetFeatureList().GetElementInfo(featureCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
					itemsModel->SetData("Description", featureDescription, itemIndex);

//					idoc::CStandardDocumentMetaInfo metaInfo;
//					if (packagePtr->GetFeaturePackages()->GetCollectionItemMetaInfo(featureCollectionId, metaInfo)){
//						QString addedStr = metaInfo.GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME).toDateTime()
//								.toString(imtgui::CObjectCollectionViewDelegate::s_dateTimeFormat);
//						itemsModel->SetData("Added", addedStr, itemIndex);

//						QString modificationStr = metaInfo.GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME).toDateTime()
//								.toString(imtgui::CObjectCollectionViewDelegate::s_dateTimeFormat);
//						itemsModel->SetData("ModificationTime", modificationStr, itemIndex);

//					}
				}

			}
		}

		itemsModel->SetIsArray(true);

		dataModel->SetExternTreeModel("items", itemsModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtlicgql


