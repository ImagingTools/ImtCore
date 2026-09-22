// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserRepresentationController.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtbase/CComplexCollectionFilter.h>


namespace imtauthgql
{


bool CUserRepresentationController::FillUserInfoFromRepresentation(
			const sdl::V1_0::imtauth::CUserData& representation,
			istd::IChangeable& userObject,
			imtbase::IObjectCollection* userCollectionPtr,
			const QByteArray& userId,
			QString& errorMessage) const
{
	auto userInfoPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(&userObject);
	if (userInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to create representation from object. Error: Object is invalid");

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

	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(groupFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	if (userCollectionPtr != nullptr){
		imtbase::IObjectCollection::Ids userElementIds = userCollectionPtr->GetElementIds(0, -1, &filterParam);
		for (const imtbase::IObjectCollection::Id& userElementId : userElementIds){
			if (userElementId != userId){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (userCollectionPtr->GetObjectData(userElementId, dataPtr)){
					auto currentUserInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(dataPtr.GetPtr());
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
		errorMessage = QStringLiteral("Name cannot be empty");
		return false;
	}

	userInfoPtr->SetName(name);

	imtsdl::TElementList<sdl::V1_0::imtauth::CSystemInfo> systemInfos;
	if (representation.systemInfos){
		systemInfos = *representation.systemInfos;
	}
	if (systemInfos.isEmpty()){
		// User from internal system
		imtauth::IUserInfo::SystemInfo systemInfo;
		userInfoPtr->AddToSystem(systemInfo);
	}
	else{
		for (const istd::TNullableValue<sdl::V1_0::imtauth::CSystemInfo>& sdlSystemInfo : systemInfos){
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

	if (representation.enabled){
		userInfoPtr->SetEnabled(*representation.enabled);
	}

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


