#include <imtlicgql/CProductCollectionControllerComp.h>

#include <imtbase/IObjectCollectionInfo.h>
#include <imtlic/IProductInfo.h>

namespace imtlicgql
{

QVariant CProductCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	imtbase::IObjectCollection::MetaInfoPtr metaInfoPtr;

	if (m_objectCollectionCompPtr->GetDataMetaInfo(objectId, metaInfoPtr)){
		if (informationId == QByteArray("Added")){
			return metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
		else if (informationId == QByteArray("ModificationTime")){
			return metaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


} // namespace imtlicgql


