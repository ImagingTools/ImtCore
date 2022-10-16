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
			return metaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_EMAIL);
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
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* metaInfoModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* children = nullptr;

	QByteArray userId = GetObjectIdFromInputParams(inputParams);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			imtauth::IUserInfo::RoleIds rolesIds = userInfoPtr->GetRoles();

			int index = metaInfoModel->InsertNewItem();
			metaInfoModel->SetData("Name", "Roles", index);
			children = metaInfoModel->AddTreeModel("Children", index);

			for (const QByteArray& productRoleId : rolesIds){
				QStringList data = QString(productRoleId).split(*m_separatorObjectIdAttrPtr);

				if (data.size() == 2){
					QByteArray roleId = data[0].toUtf8();
					QByteArray productId = data[1].toUtf8();

					int childrenIndex = children->InsertNewItem();
					children->SetData("Value", roleId + " (" + productId + ")", childrenIndex);
				}
			}
		}

		dataModel->SetExternTreeModel("metaInfo", metaInfoModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtauth


