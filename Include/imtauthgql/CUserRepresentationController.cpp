#include <imtauthgql/CUserRepresentationController.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


bool CUserRepresentationController::FillUserInfoFromRepresentation(
			const sdl::imtauth::Users::CUserData::V1_0& representation,
			istd::IChangeable& userObject,
			imtbase::IObjectCollection& userCollection,
			QByteArray& userId,
			QString& errorMessage) const
{
	imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(&userObject);
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");

		return false;
	}

	QByteArray productId;
	if (representation.ProductId){
		productId = *representation.ProductId;
	}

	QByteArray username;
	if (representation.Username){
		username = *representation.Username;
	}

	if (username.isEmpty()){
		errorMessage = QT_TR_NOOP("Username can't be empty!");

		return false;
	}

	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;

	iprm::CTextParam userIdParam;
	userIdParam.SetText(username);

	paramsSet.SetEditableParameter("Id", &userIdParam);
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::Ids userIds = userCollection.GetElementIds(0, -1, &filterParam);
	if (!userIds.isEmpty()){
		QByteArray userObjectId = userIds[0];
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (userCollection.GetObjectData(userObjectId, dataPtr)){
			const imtauth::CUserInfo* currentUserInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(dataPtr.GetPtr());
			if (currentUserInfoPtr != nullptr){
				if (userObjectId != userId){
					QByteArray currentUsername = currentUserInfoPtr->GetId();
					if (currentUsername.toLower() == username.toLower()){
						errorMessage = QT_TR_NOOP("Username already exists");

						return false;
					}
				}
			}
		}
	}

	userInfoPtr->SetId(username);

	QString name;
	if (representation.Name){
		name = *representation.Name;
	}

	if (name.isEmpty()){
		errorMessage = QString("Name cannot be empty");
		return false;
	}

	userInfoPtr->SetName(name);

	QList<sdl::imtauth::Users::CSystemInfo::V1_0> systemInfos;
	if (representation.SystemInfos){
		systemInfos = *representation.SystemInfos;
	}
	if (systemInfos.isEmpty()){
		// User from internal system
		imtauth::IUserInfo::SystemInfo systemInfo;
		userInfoPtr->AddToSystem(systemInfo);
	}
	else{
		for (const sdl::imtauth::Users::CSystemInfo::V1_0& sdlSystemInfo : systemInfos){
			QByteArray systemId;
			if (sdlSystemInfo.Id){
				systemId = *sdlSystemInfo.Id;
			}
			QString systemName;
			if (sdlSystemInfo.Name){
				systemName = *sdlSystemInfo.Name;
			}
			bool enabled = false;
			if (sdlSystemInfo.Enabled){
				enabled = *sdlSystemInfo.Enabled;
			}

			imtauth::IUserInfo::SystemInfo systemInfo;
			systemInfo.systemId = systemId;
			systemInfo.systemName = systemName;
			systemInfo.enabled = enabled;

			userInfoPtr->AddToSystem(systemInfo);
		}
	}

	QString mail;
	if (representation.Email){
		mail = *representation.Email;
	}
	userInfoPtr->SetMail(mail);

	imtauth::IUserInfo::FeatureIds permissions;
	if (representation.Permissions){
		permissions = representation.Permissions->split(';');
	}
	permissions.removeAll("");
	userInfoPtr->SetLocalPermissions(productId, permissions);

	QByteArrayList roleIds;
	if (representation.Roles){
		roleIds = representation.Roles->split(';');
	}
	roleIds.removeAll("");
	if (!roleIds.isEmpty()){
		userInfoPtr->SetRoles(productId, roleIds);
	}
	else{
		userInfoPtr->RemoveProduct(productId);
	}

	QByteArrayList groupIds;
	if (representation.Groups){
		groupIds = representation.Groups->split(';');
	}
	groupIds.removeAll("");
	for (const QByteArray& groupId : groupIds){
		if (!groupId.isEmpty()){
			userInfoPtr->AddToGroup(groupId);
		}
	}

	return true;
}


} // namespace imtauth


