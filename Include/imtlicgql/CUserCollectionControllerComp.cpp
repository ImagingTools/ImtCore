#include <imtlicgql/CUserCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtlicgql
{


QVariant CUserCollectionControllerComp::GetObjectInformation(
		const QByteArray &informationId,
		const QByteArray &objectId) const
{
	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
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

	QByteArray userId = GetObjectIdFromInputParams(inputParams);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			imtauth::IUserInfo::RoleIds rolesIds = userInfoPtr->GetRoles();

//			imtauth::IRoleInfoProvider roleInfoProvider = userInfoPtr->GetRoleProvider();
			for (const QByteArray& roleId : rolesIds){

			}
		}


	}

	return nullptr;
}


} // namespace imtlicgql


