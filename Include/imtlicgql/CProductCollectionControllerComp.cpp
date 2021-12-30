#include <imtlicgql/CProductCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <idoc/IDocumentMetaInfo.h>
// ImtCore includes
//#include <imtbase/IObjectCollectionInfo.h>
//#include <imtlic/IProductInfo.h>


namespace imtlicgql
{


QVariant CProductCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	idoc::CStandardDocumentMetaInfo metaInfo;
	if (m_objectCollectionCompPtr->GetCollectionItemMetaInfo(objectId, metaInfo)){
		if (informationId == QByteArray("Added")){
			return metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
		else if (informationId == QByteArray("ModificationTime")){
			return metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


} // namespace imtlicgql


