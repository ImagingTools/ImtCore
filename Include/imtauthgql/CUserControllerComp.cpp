#include <imtauthgql/CUserControllerComp.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CUserControllerComp::GetObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemModel = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemModel = new imtbase::CTreeItemModel();

		QByteArray userId = GetObjectIdFromInputParams(inputParams);

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
			const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());

			if (userInfoPtr == nullptr){
				errorMessage = QT_TR_NOOP("Unable to get an user info");
				return nullptr;
			}

			QByteArray id = userInfoPtr->GetId();
			QString username = userInfoPtr->GetUsername();
			QString name = userInfoPtr->GetName();
			QByteArray passwordHash = userInfoPtr->GetPasswordHash();
			QString mail = userInfoPtr->GetMail();

			itemModel->SetData("Id", id);
			itemModel->SetData("Username", username);
			itemModel->SetData("Name", name);
			itemModel->SetData("PasswordHash", passwordHash);
			itemModel->SetData("Mail", mail);
		}
		dataModel->SetExternTreeModel("item", itemModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


istd::IChangeable* CUserControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray &objectId,
			QString &name,
			QString &description,
			QString& errorMessage) const
{
	if (!m_userInfoFactCompPtr.IsValid()){
		errorMessage = QObject::tr("Can not create User: %1").arg(QString(objectId));
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		imtauth::CUserInfo *userInfoPtr = new imtauth::CUserInfo();
		if (userInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an account info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

		if (itemModel.ContainsKey("Id")){
			QByteArray userId = itemModel.GetData("Id").toByteArray();
			if (!userId.isEmpty()){
				userInfoPtr->SetId(userId);
			}
			else{
				if (objectId.isEmpty()){
					errorMessage = QT_TR_NOOP("User Id can't be empty!");
					return nullptr;
				}
				else{
					userInfoPtr->SetId(objectId);
				}
			}
		}

		if (itemModel.ContainsKey("Username")){
			QString username = itemModel.GetData("Username").toString();
			userInfoPtr->SetUsername(username);
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Username").toString();
			userInfoPtr->SetUsername(name);
		}

		if (itemModel.ContainsKey("PasswordHash")){
			QByteArray passwordHash = itemModel.GetData("PasswordHash").toByteArray();
			userInfoPtr->SetPasswordHash(passwordHash);
		}

		if (itemModel.ContainsKey("Mail")){
			QString mail = itemModel.GetData("Mail").toString();
			userInfoPtr->SetMail(mail);
		}


		return userInfoPtr;
	}

	errorMessage = QObject::tr("Can not create user: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace imtauthgql


