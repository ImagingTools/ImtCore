// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserActionCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/IUserRecentAction.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/CUserGroupFilter.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::UserActions::CUserCollectionControllerCompBase)

bool CUserActionCollectionControllerComp::CreateRepresentationFromObject(
			const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::UserActions::CGetUserActionsGqlRequest& getUserActionsRequest,
			sdl::imtauth::UserActions::CUserActionItemData::V1_0& representationObject,
			QString& errorMessage) const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		return false;
	}

	QByteArray objectId = objectCollectionIterator.GetObjectId();
	sdl::imtauth::UserActions::GetUserActionsRequestInfo requestInfo = getUserActionsRequest.GetRequestInfo();

	const imtauth::IUserRecentAction* userRecentActionPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		userRecentActionPtr = dynamic_cast<const imtauth::IUserRecentAction*>(dataPtr.GetPtr());
	}

	QString applicationId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID);

	idoc::MetaInfoPtr metaInfoPtr = objectCollectionIterator.GetDataMetaInfo();
	if (!metaInfoPtr.IsValid()){
		return false;
	}

	if (userRecentActionPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CUserActionCollectionControllerComp");
		return false;
	}

	if (requestInfo.items.isIdRequested){
		representationObject.id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	QString scheme = "applink";
	QByteArray targetId = metaInfoPtr->GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_ID).toString().toUtf8();
	QByteArray targetTypeId = metaInfoPtr->GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_ID).toString().toUtf8();
	QString targetTypeName = metaInfoPtr->GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_NAME).toString();
	QString targetSource = metaInfoPtr->GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_SOURCE).toString();

	if (requestInfo.items.isTargetTypeLinkRequested){
		sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
		objectLink.id = targetTypeId;
		objectLink.typeId = targetTypeId;
		objectLink.name = targetTypeName;

		sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
		urlParam.scheme = scheme;
		urlParam.host = applicationId;
		urlParam.path = targetSource + "/" + targetTypeId;

		objectLink.url = urlParam;

		representationObject.targetTypeLink = objectLink;
	}

	if (requestInfo.items.isTargetLinkRequested){
		sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
		objectLink.id = targetId;
		objectLink.name = metaInfoPtr->GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_NAME).toString();
		objectLink.typeId = targetTypeId;

		sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
		urlParam.scheme = scheme;
		urlParam.host = applicationId;

		urlParam.path = targetSource;
		if (!targetTypeId.isEmpty()){
			urlParam.path = *urlParam.path + QStringLiteral("/") + targetTypeId;
		}

		if (!targetId.isEmpty()){
			urlParam.path = *urlParam.path + QStringLiteral("/") + targetId;
		}

		objectLink.url = urlParam;

		representationObject.targetLink = objectLink;
	}

	if (requestInfo.items.isUserLinkRequested){
		sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
		objectLink.id = metaInfoPtr->GetMetaInfo(imtauth::IUserRecentAction::MIT_USER_ID).toString().toUtf8();
		objectLink.typeId = QByteArrayLiteral("User");
		objectLink.name = metaInfoPtr->GetMetaInfo(imtauth::IUserRecentAction::MIT_USER_NAME).toString();

		sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
		urlParam.scheme = "applink";
		urlParam.host = applicationId;
		urlParam.path = QByteArrayLiteral("Administration/Users/User/") + *objectLink.id;

		objectLink.url = urlParam;

		representationObject.userLink = objectLink;
	}

	if (requestInfo.items.isActionTypeRequested){
		imtauth::IUserRecentAction::ActionTypeInfo actionType = userRecentActionPtr->GetActionTypeInfo();
		representationObject.actionType = actionType.name;
	}

	if (requestInfo.items.isDescriptionRequested){
		imtauth::IUserRecentAction::ActionTypeInfo actionType = userRecentActionPtr->GetActionTypeInfo();
		representationObject.description = actionType.description;
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime timestamp = userRecentActionPtr->GetTimestamp();
		representationObject.timeStamp = QString(timestamp.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"));
	}

	return true;
}


void CUserActionCollectionControllerComp::SetAdditionalFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlParamObject& /*viewParamsGql*/,
			iprm::CParamsSet* filterParamsPtr) const
{
	if (filterParamsPtr == nullptr){
		return;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			if (!userInfoPtr->IsAdmin()){
				QByteArray productId = gqlRequest.GetHeader("productid");
				QByteArrayList userPermissions = userInfoPtr->GetPermissions(productId);
				bool viewAllUserActions = false;

				if (m_checkPermissionCompPtr.IsValid()){
					viewAllUserActions = m_checkPermissionCompPtr->CheckPermission(userPermissions, QByteArrayList {QByteArrayLiteral("ViewAllUserActions")});
				}

				if (!viewAllUserActions){
					istd::TDelPtr<imtauth::CUserGroupFilter> groupFilterPtr = new imtauth::CUserGroupFilter();
					groupFilterPtr->SetUserId(userInfoPtr->GetId());
					groupFilterPtr->SetGroupIds(userInfoPtr->GetGroups());
					filterParamsPtr->SetEditableParameter("GroupFilter", groupFilterPtr.PopPtr(), true);
				}
			}
		}
	}
}


} // namespace imtauth


