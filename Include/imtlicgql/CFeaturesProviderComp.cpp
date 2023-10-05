#include <imtlicgql/CFeaturesProviderComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicgql
{


// reimplemented (imtgql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CFeaturesProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){

		return nullptr;
	}

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (const QByteArray& collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			const imtlic::CFeaturePackage* packagePtr  = dynamic_cast<const imtlic::CFeaturePackage*>(dataPtr.GetPtr());

		}
	}

}


} // namespace imtlicgql


