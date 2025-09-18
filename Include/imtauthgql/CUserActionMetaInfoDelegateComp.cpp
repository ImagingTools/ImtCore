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

	QByteArray targetId = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_ID).toByteArray();
	representation["targetId"] = QString(targetId);

	QByteArray targetTypeId = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_ID).toByteArray();
	representation["targetTypeId"] = QString(targetTypeId);

	int actionType = metaInfo.GetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE).toInt();
	representation["actionType"] = actionType;

	return true;
}


bool CUserActionMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const
{
	if (representation.contains("userId")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_USER_ID, representation.value("userId"));
	}

	if (representation.contains("targetId")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_ID, representation.value("targetId"));
	}

	if (representation.contains("targetTypeId")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_TARGET_TYPE_ID, representation.value("targetTypeId"));
	}

	if (representation.contains("actionType")){
		metaInfo.SetMetaInfo(imtauth::IUserRecentAction::MIT_ACTION_TYPE, representation.value("actionType"));
	}

	return true;
}


} // namespace imtauthgql


