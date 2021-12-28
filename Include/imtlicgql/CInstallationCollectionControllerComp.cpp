#include <imtlicgql/CInstallationCollectionControllerComp.h>

#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/IProductInfo.h>

namespace imtlicgql
{

QVariant CInstallationCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	imtbase::IObjectCollection::MetaInfoPtr metaInfoPtr;

	if (m_objectCollectionCompPtr->GetDataMetaInfo(objectId, metaInfoPtr)){
		if (informationId == QByteArray("InstanceId")){
			return metaInfoPtr->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_INSTANCE_ID);
		}
		else if (informationId == QByteArray("Customer")){
			return metaInfoPtr->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_NAME);
		}
		else if (informationId == QByteArray("Added")){
			return metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
		else if (informationId == QByteArray("ModificationTime")){
			return metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}



} // namespace imtlicgql


