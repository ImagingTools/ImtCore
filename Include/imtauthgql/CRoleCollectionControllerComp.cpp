#include <imtauthgql/CRoleCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtauth/CRole.h>


namespace imtauthgql
{

// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CRoleCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	if (informationId == QByteArray("Added")){
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);;
		if (metaInfo.IsValid()){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
	}
	else if (informationId == QByteArray("LastModified")){
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
		if (metaInfo.IsValid()){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CRoleCollectionControllerComp::GetMetaInfo(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	return nullptr;
}


} // namespace imtauth


