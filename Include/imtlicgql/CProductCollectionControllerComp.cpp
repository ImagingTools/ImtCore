#include <imtlicgql/CProductCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>
#include <imtlic/IProductInfo.h>


namespace imtlicgql
{


QVariant CProductCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
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


} // namespace imtlicgql


