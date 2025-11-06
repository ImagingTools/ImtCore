#include <imtauthgql/CUserRepresentationController.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtbase/CComplexCollectionFilter.h>


namespace imtauthgql
{


bool CUserRepresentationController::FillUserInfoFromRepresentation(
			const sdl::imtauth::Users::CUserData::V1_0& representation,
			istd::IChangeable& userObject,
			imtbase::IObjectCollection* userCollectionPtr,
			const QByteArray& userId,
			QString& errorMessage) const
{
	imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(&userObject);
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");

		return false;
	}

	QByteArray productId;
	if (representation.productId){
		productId = *representation.productId;
	}

	QByteArray username;
	if (representation.username){
		username = *representation.username;
	}

	if (username.isEmpty()){
		errorMessage = QT_TR_NOOP("Username can't be empty!");
		return false;
	}

	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "Id";
	fieldFilter.filterValue = username;
	fieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FO_CONTAINS;

	imtbase::IComplexCollectionFilter::GroupFilter groupFilter;
	groupFilter.fieldFilters << fieldFilter;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFieldsFilter(groupFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	if (userCollectionPtr != nullptr){
		imtbase::IObjectCollection::Ids userElementIds = userCollectionPtr->GetElementIds(0, -1, &filterParam);
		for (const imtbase::IObjectCollection::Id& userElementId : userElementIds){
			if (userElementId != userId){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (userCollectionPtr->GetObjectData(userElementId, dataPtr)){
					const imtauth::CUserInfo* currentUserInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(dataPtr.GetPtr());
					if (currentUserInfoPtr != nullptr){
						QByteArray currentUsername = currentUserInfoPtr->GetId();
						if (currentUsername.toLower() == username.toLower()){
							errorMessage = QT_TR_NOOP("Username already exists");
							return false;
						}
					}
				}
			}
		}
	}

	userInfoPtr->SetId(username);

	QString name;
	if (representation.name){
		name = *representation.name;
	}

	if (name.isEmpty()){
		errorMessage = QString("Name cannot be empty");
		return false;
	}

	userInfoPtr->SetName(name);

	imtsdl::TElementList<sdl::imtauth::Users::CSystemInfo::V1_0> systemInfos;
	if (representation.systemInfos){
		systemInfos = *representation.systemInfos;
	}
	if (systemInfos.isEmpty()){
		// User from internal system
		imtauth::IUserInfo::SystemInfo systemInfo;
		userInfoPtr->AddToSystem(systemInfo);
	}
	else{
		for (const istd::TSharedNullable<sdl::imtauth::Users::CSystemInfo::V1_0>& sdlSystemInfo : systemInfos){
			QByteArray systemId;
			if (!sdlSystemInfo.HasValue()){
				continue;
			}
			if (sdlSystemInfo->id){
				systemId = *sdlSystemInfo->id;
			}
			QString systemName;
			if (sdlSystemInfo->name){
				systemName = *sdlSystemInfo->name;
			}
			bool enabled = false;
			if (sdlSystemInfo->enabled){
				enabled = *sdlSystemInfo->enabled;
			}

			imtauth::IUserInfo::SystemInfo systemInfo;
			systemInfo.systemId = systemId;
			systemInfo.systemName = systemName;
			systemInfo.enabled = enabled;

			userInfoPtr->AddToSystem(systemInfo);
		}
	}

	QString mail;
	if (representation.email){
		mail = *representation.email;
	}
	userInfoPtr->SetMail(mail);

	imtauth::IUserInfo::FeatureIds permissions;
	if (representation.permissions){
		permissions = representation.permissions->ToList();
	}
	permissions.removeAll("");
	userInfoPtr->SetLocalPermissions(productId, permissions);

	QByteArrayList roleIds;
	if (representation.roles){
		roleIds = representation.roles->ToList();
	}
	roleIds.removeAll("");
	if (!roleIds.isEmpty()){
		userInfoPtr->SetRoles(productId, roleIds);
	}
	else{
		userInfoPtr->RemoveProduct(productId);
	}

	QByteArrayList groupIds;
	if (representation.groups){
		groupIds = representation.groups->ToList();
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


