#include <imtauthgql/CUserCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{

// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CUserCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);

	if (metaInfo.IsValid()){
		if (informationId == QByteArray("Username")){
			return metaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_USERNAME);
		}
		else if (informationId == QByteArray("Email")){
			return metaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_CONTACT_EMAIL);
		}
		else if (informationId == QByteArray("Added")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
		else if (informationId == QByteArray("LastModified")){
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


} // namespace imtauth


