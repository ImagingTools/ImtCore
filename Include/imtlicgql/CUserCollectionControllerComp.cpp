#include <imtlicgql/CUserCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <idoc/IDocumentMetaInfo.h>
// ImtCore includes
//#include <imtbase/IObjectCollectionInfo.h>
//#include <imtlic/IProductInfo.h>

#include <imtlic/CProductLicensingInfo.h>
#include <imtlic/CLicenseInfo.h>


namespace imtlicgql
{


QVariant CUserCollectionControllerComp::GetObjectInformation(
		const QByteArray &informationId,
		const QByteArray &objectId) const
{
	imtbase::ICollectionInfo::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
	if (metaInfo.IsValid()){
		if (informationId == QByteArray("Added")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
		else if (informationId == QByteArray("ModificationTime")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CUserCollectionControllerComp::GetMetaInfo(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{

	return nullptr;
}


} // namespace imtlicgql


