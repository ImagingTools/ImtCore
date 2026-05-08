// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ImtCore includes
#include <imtauth/ITenantInfo.h>


namespace imtauthgql
{


// protected methods

bool CTenantMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo, const QByteArray& typeId) const
{
	Q_UNUSED(typeId);
	
	QByteArray id = metaInfo.GetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_ID).toByteArray();
	representation["TenantId"] = QString(id);
	
	QString name = metaInfo.GetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_NAME).toString();
	representation["TenantName"] = name;
	
	QString description = metaInfo.GetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_DESCRIPTION).toString();
	representation["TenantDescription"] = description;

	QByteArray ownerId = metaInfo.GetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_OWNER_ID).toByteArray();
	representation["TenantOwnerId"] = QString(ownerId);
	
	return true;
}


bool CTenantMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation, const QByteArray& typeId) const
{
	Q_UNUSED(typeId);
	
	if (representation.contains("TenantId")){
		metaInfo.SetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_ID, representation.value("TenantId"));
	}
	
	if (representation.contains("TenantName")){
		metaInfo.SetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_NAME, representation.value("TenantName"));
	}
	
	if (representation.contains("TenantDescription")){
		metaInfo.SetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_DESCRIPTION, representation.value("TenantDescription"));
	}

	if (representation.contains("TenantOwnerId")){
		metaInfo.SetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_OWNER_ID, representation.value("TenantOwnerId"));
	}
	
	return true;
}


} // namespace imtauthgql

