#include <imtlicgql/CProductFeaturesControllerComp.h>

// ImtCore includes

namespace imtlicgql
{


// public methods

// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CProductFeaturesControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return nullptr;
	}

//	if (!m_productCollectionCompPtr.IsValid()){
//		return nullptr;
//	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModelPtr = new imtbase::CTreeItemModel();

	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	const QList<imtgql::CGqlObject>* paramList = gqlRequest.GetParams();

	QByteArray productId = paramList->at(0).GetFieldArgumentValue("ProductId").toByteArray();

//	imtbase::IObjectCollection::DataPtr dataPtr;
//	if (m_productCollectionCompPtr->GetObjectData(productId, dataPtr)){
//		const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr.GetPtr());

//		imtlic::CFeaturePackage* featurePackagePtr = productPtr->GetAllFeatures();

//		QByteArrayList featureCollectionIds = featurePackagePtr->GetFeatureList().GetElementIds().toList();

//		for (const QByteArray& featureCollectionId : featureCollectionIds){
//			const imtlic::IFeatureInfo* featureInfoPtr = featurePackagePtr->GetFeatureInfo(featureCollectionId);

//			QByteArray featureId = featureInfoPtr->GetFeatureId();
//			QString featureName = featureInfoPtr->GetFeatureName();

//			int index = dataModelPtr->InsertNewItem();
//			dataModelPtr->SetData("Id", featureId, index);
//			dataModelPtr->SetData("Name", featureName, index);
//			dataModelPtr->SetData("State", Qt::CheckState::Unchecked, index);
//		}
//	}

	rootModelPtr->SetExternTreeModel("data", dataModelPtr);

	return rootModelPtr;
}


} // namespace imtlicgql


