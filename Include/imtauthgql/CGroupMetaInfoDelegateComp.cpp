// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CGroupMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ImtCore includes
#include <imtauth/IUserGroupInfo.h>


namespace imtauthgql
{


// protected methods

bool CGroupMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray id = metaInfo.GetMetaInfo(imtauth::IUserGroupInfo::MIT_ID).toByteArray();
	representation["GroupId"] = QString(id);
	
	QString name = metaInfo.GetMetaInfo(imtauth::IUserGroupInfo::MIT_NAME).toString();
	representation["GroupName"] = name;
	
	QString description = metaInfo.GetMetaInfo(imtauth::IUserGroupInfo::MIT_DESCRIPTION).toString();
	representation["GroupDescription"] = description;
	
	return true;
}


bool CGroupMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const
{
	if (representation.contains("GroupId")){
		metaInfo.SetMetaInfo(imtauth::IUserGroupInfo::MIT_ID, representation.value("GroupId"));
	}
	
	if (representation.contains("GroupName")){
		metaInfo.SetMetaInfo(imtauth::IUserGroupInfo::MIT_NAME, representation.value("GroupName"));
	}
	
	if (representation.contains("GroupDescription")){
		metaInfo.SetMetaInfo(imtauth::IUserGroupInfo::MIT_DESCRIPTION, representation.value("GroupDescription"));
	}
	
	return true;
}


} // namespace imtauthgql


