// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserActionMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ImtCore includes
#include <imtauth/IUserRecentAction.h>



namespace imtauthgql
{


// protected methods

bool CUserActionMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray userId = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_USER_ID).toByteArray();
	representation["userId"] = QString(userId);

	QString userName = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_USER_NAME).toString();
	representation["userName"] = QString(userName);

	QByteArray targetId = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_ID).toByteArray();
	representation["targetId"] = QString(targetId);

	QString targetName = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_NAME).toString();
	representation["targetName"] = QString(targetName);

	QString targetSource = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_SOURCE).toString();
	representation["targetSource"] = QString(targetSource);

	QByteArray targetTypeId = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_ID).toByteArray();
	representation["targetTypeId"] = QString(targetTypeId);

	QString targetTypeName = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_NAME).toString();
	representation["targetTypeName"] = QString(targetTypeName);

	QByteArray actionTypeId = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE_ID).toByteArray();
	representation["actionTypeId"] = QString(actionTypeId);

	QString actionTypeName = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE_NAME).toByteArray();
	representation["actionTypeName"] = QString(actionTypeName);

	QString actionTypeDescription = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE_DESCRIPTION).toByteArray();
	representation["actionTypeDescription"] = QString(actionTypeDescription);

	return true;
}


bool CUserActionMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const
{
	if (representation.contains("userId")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_USER_ID, representation.value("userId"));
	}

	if (representation.contains("userName")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_USER_NAME, representation.value("userName"));
	}

	if (representation.contains("targetId")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_ID, representation.value("targetId"));
	}

	if (representation.contains("targetName")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_NAME, representation.value("targetName"));
	}

	if (representation.contains("targetSource")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_SOURCE, representation.value("targetSource"));
	}

	if (representation.contains("targetTypeId")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_ID, representation.value("targetTypeId"));
	}

	if (representation.contains("targetTypeName")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_NAME, representation.value("targetTypeName"));
	}

	if (representation.contains("actionTypeId")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE_ID, representation.value("actionTypeId"));
	}

	if (representation.contains("actionTypeName")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE_NAME, representation.value("actionTypeName"));
	}

	if (representation.contains("actionTypeDescription")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE_DESCRIPTION, representation.value("actionTypeDescription"));
	}

	return true;
}


} // namespace imtauthgql


