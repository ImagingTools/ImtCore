// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRoleMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ImtCore includes
#include <imtauth/IRole.h>


namespace imtauthgql
{


// protected methods

bool CRoleMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo, const QByteArray& typeId) const
{
	Q_UNUSED(typeId);
	
	QByteArray roleId = metaInfo.GetMetaInfo(imtauth::IRole::MIT_ROLE_ID).toByteArray();
	representation["RoleId"] = QString(roleId);

	QString roleName = metaInfo.GetMetaInfo(imtauth::IRole::MIT_ROLE_NAME).toString();
	representation["RoleName"] = roleName;

	QString description = metaInfo.GetMetaInfo(imtauth::IRole::MIT_ROLE_DESCRIPTION).toString();
	representation["RoleDescription"] = description;

	QByteArray productId = metaInfo.GetMetaInfo(imtauth::IRole::MIT_PRODUCT_ID).toByteArray();
	representation["ProductId"] = QString(productId);

	return true;
}


bool CRoleMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation, const QByteArray& typeId) const
{
	Q_UNUSED(typeId);
	
	if (representation.contains("RoleId")){
		metaInfo.SetMetaInfo(imtauth::IRole::MIT_ROLE_ID, representation.value("RoleId"));
	}

	if (representation.contains("RoleName")){
		metaInfo.SetMetaInfo(imtauth::IRole::MIT_ROLE_NAME, representation.value("RoleName"));
	}

	if (representation.contains("RoleDescription")){
		metaInfo.SetMetaInfo(imtauth::IRole::MIT_ROLE_DESCRIPTION, representation.value("RoleDescription"));
	}

	if (representation.contains("ProductId")){
		metaInfo.SetMetaInfo(imtauth::IRole::MIT_PRODUCT_ID, representation.value("ProductId"));
	}

	return true;
}


} // namespace imtauthgql


