#include <imtauthgql/CUserGroupControllerComp.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CUserGroupControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	dataModel->SetData("Id", "");
	dataModel->SetData("Name", "");

	QByteArray userGroupId = GetObjectIdFromInputParams(*gqlRequest.GetParams());
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userGroupId, dataPtr)){
		const imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
		if (userGroupInfoPtr != nullptr){
			QByteArray groupId = userGroupInfoPtr->GetId();
			QString name = userGroupInfoPtr->GetName();
			QString description = userGroupInfoPtr->GetDescription();

			dataModel->SetData("Id", groupId);
			dataModel->SetData("Name", name);
			dataModel->SetData("Description", description);
			dataModel->SetData("Users", "");

			imtauth::IUserGroupInfo::UserIds userGroupIds = userGroupInfoPtr->GetUsers();
			if (!userGroupIds.empty()){
				QByteArray users = userGroupIds.values().join(';');
				dataModel->SetData("Users", users);
			}
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CUserGroupControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray &objectId,
		QString &name,
		QString &description,
		QString& errorMessage) const
{

	if (!m_userInfoFactCompPtr.IsValid() || !m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<imtauth::IUserGroupInfo> userGroupInfoPtr = m_userInfoFactCompPtr.CreateInstance();
		if (userGroupInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an group info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			userGroupInfoPtr->SetName(name);
		}

		if (itemModel.ContainsKey("Description")){
			QString description = itemModel.GetData("Description").toString();

			userGroupInfoPtr->SetDescription(description);
		}

		if (itemModel.ContainsKey("Users")){
			QByteArray users = itemModel.GetData("Users").toByteArray();
			if (!users.isEmpty()){
				QByteArrayList userIds = users.split(';');
				userGroupInfoPtr->SetUsers(imtauth::IUserGroupInfo::UserIds(userIds.begin(), userIds.end()));
			}
		}

		return userGroupInfoPtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create group: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace imtauthgql


