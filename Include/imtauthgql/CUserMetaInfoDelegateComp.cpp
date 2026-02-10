// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauthgql
{


// protected methods

bool CUserMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray id = metaInfo.GetMetaInfo(imtauth::IUserInfo::MIT_ID).toByteArray();
	representation["UserId"] = QString(id);
	
	QString name = metaInfo.GetMetaInfo(imtauth::IUserInfo::MIT_NAME).toString();
	representation["UserName"] = name;

	QString email = metaInfo.GetMetaInfo(imtauth::IUserInfo::MIT_EMAIL).toString();
	representation["Email"] = email;
	
	QString passwordHash = metaInfo.GetMetaInfo(imtauth::IUserInfo::MIT_PASSWORD_HASH).toString();
	representation["PasswordHash"] = passwordHash;
	
	QByteArray systemId = metaInfo.GetMetaInfo(imtauth::IUserInfo::MIT_SYSTEM_ID).toByteArray();
	representation["SystemId"] = QString(systemId);
	
	QString systemName = metaInfo.GetMetaInfo(imtauth::IUserInfo::MIT_SYSTEM_NAME).toString();
	representation["SystemName"] = systemName;
	
	return true;
}


bool CUserMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const
{
	if (representation.contains("UserId")){
		metaInfo.SetMetaInfo(imtauth::IUserInfo::MIT_ID, representation.value("UserId"));
	}
	
	if (representation.contains("UserName")){
		metaInfo.SetMetaInfo(imtauth::IUserInfo::MIT_NAME, representation.value("UserName"));
	}
	
	if (representation.contains("Mail")){
		metaInfo.SetMetaInfo(imtauth::IUserInfo::MIT_EMAIL, representation.value("Mail"));
	}
	
	if (representation.contains("PasswordHash")){
		metaInfo.SetMetaInfo(imtauth::IUserInfo::MIT_PASSWORD_HASH, representation.value("PasswordHash"));
	}
	
	if (representation.contains("SystemId")){
		metaInfo.SetMetaInfo(imtauth::IUserInfo::MIT_SYSTEM_ID, representation.value("SystemId"));
	}
	
	if (representation.contains("SystemName")){
		metaInfo.SetMetaInfo(imtauth::IUserInfo::MIT_SYSTEM_NAME, representation.value("SystemName"));
	}
	
	return true;
}


} // namespace imtauthgql


