// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CPublishedPermissionsProviderComp.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtauth/CProductPermissionSnapshot.h>
#include <imtauth/imtauth.h>
#include <imtauthgql/imtauthgql.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Permissions.h>


namespace imtauthgql
{


// reimplemented (sdl::V1_0::imtauth::CPermissionsGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetProductPermissionsPayload CPublishedPermissionsProviderComp::OnGetProductPermissions(
			const sdl::V1_0::imtauth::CGetProductPermissionsGqlRequest& getProductPermissionsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	const auto arguments = getProductPermissionsRequest.GetRequestedArguments();
	const QByteArray productId = arguments.input.has_value() && arguments.input->productId ? QByteArray(*arguments.input->productId) : QByteArray();
	const QByteArray tenantId = arguments.input.has_value() && arguments.input->tenantId ? QByteArray(*arguments.input->tenantId) : QByteArray();

	return CreatePayload(productId, tenantId, gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetProductPermissionsPayload CPublishedPermissionsProviderComp::OnGetUserPermissions(
			const sdl::V1_0::imtauth::CGetUserPermissionsGqlRequest& getUserPermissionsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	const auto arguments = getUserPermissionsRequest.GetRequestedArguments();
	const QByteArray productId = arguments.input.has_value() && arguments.input->productId ? QByteArray(*arguments.input->productId) : QByteArray();

	return CreatePayload(productId, QByteArray(), gqlRequest, errorMessage);
}


// private methods

sdl::V1_0::imtauth::CGetProductPermissionsPayload CPublishedPermissionsProviderComp::CreatePayload(
			const QByteArray& productId,
			const QByteArray& tenantId,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CGetProductPermissionsPayload response;

	// The trees are shown to a superuser in this version
	if (!IsSuperuserRequest(gqlRequest)){
		errorMessage = QStringLiteral("The permission tree of the product '%1' is not available").arg(QString::fromUtf8(productId));
		response.errorMessage = errorMessage;

		return response;
	}

	auto& groups = response.groups.Emplace();

	const QByteArray snapshotId = imtauth::FindProductPermissionSnapshot(*m_snapshotCollectionCompPtr, productId);
	if (snapshotId.isEmpty()){
		return response;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	const imtauth::CProductPermissionSnapshot* snapshotPtr = m_snapshotCollectionCompPtr->GetObjectData(snapshotId, dataPtr) ? dynamic_cast<const imtauth::CProductPermissionSnapshot*>(dataPtr.GetPtr()) : nullptr;
	if (snapshotPtr == nullptr){
		errorMessage = QStringLiteral("Unable to read the permission tree of the product '%1'").arg(QString::fromUtf8(productId));
		response.errorMessage = errorMessage;

		return response;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const QString languageId = gqlContextPtr != nullptr ? QString::fromUtf8(gqlContextPtr->GetLanguageId()) : QString();

	// A language the product does not have falls back to the first one it published
	const QJsonArray languages = QJsonDocument::fromJson(snapshotPtr->GetTree()).array();
	QJsonObject languageObject = languages.isEmpty() ? QJsonObject() : languages.first().toObject();
	for (const QJsonValue& value : languages){
		if (value.toObject().value("languageId").toString() == languageId){
			languageObject = value.toObject();
		}
	}

	const bool isScoped = !tenantId.isEmpty() && m_tenantManagerCompPtr.IsValid();
	const QByteArrayList allowedPermissions = isScoped ? m_tenantManagerCompPtr->GetTenantPermissions(tenantId) : QByteArrayList();

	const QJsonArray groupArray = languageObject.value("groups").toArray();
	for (const QJsonValue& groupValue : groupArray){
		const QJsonObject groupObject = groupValue.toObject();

		imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionEntry> entries;
		const QJsonArray entryArray = groupObject.value("entries").toArray();
		for (const QJsonValue& entryValue : entryArray){
			const QJsonObject entryObject = entryValue.toObject();
			const QByteArray permissionId = entryObject.value("permissionId").toString().toUtf8();
			if (isScoped && !imtauth::HasPermission(allowedPermissions, permissionId)){
				continue;
			}

			QByteArrayList requirements;
			const QJsonArray requirementArray = entryObject.value("requirements").toArray();
			for (const QJsonValue& requirement : requirementArray){
				requirements.append(requirement.toString().toUtf8());
			}

			sdl::V1_0::imtauth::CPermissionEntry entry;
			entry.permissionId = permissionId;
			entry.displayName = entryObject.value("displayName").toString();
			entry.description = entryObject.value("description").toString();
			entry.requirements.Emplace().FromList(requirements);
			entries.append(entry);
		}

		if (entries.isEmpty()){
			continue;
		}

		sdl::V1_0::imtauth::CPermissionGroup group;
		group.groupId = groupObject.value("groupId").toString().toUtf8();
		group.groupName = groupObject.value("groupName").toString();
		group.entries = entries;
		groups.append(group);
	}

	return response;
}


} // namespace imtauthgql
