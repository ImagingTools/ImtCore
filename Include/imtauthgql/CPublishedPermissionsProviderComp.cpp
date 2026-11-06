// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CPublishedPermissionsProviderComp.h>


// Qt includes
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

	sdl::V1_0::imtauth::CPermissionTree tree;
	if (!tree.ReadFromJsonObject(QJsonDocument::fromJson(snapshotPtr->GetTree()).object())){
		errorMessage = QStringLiteral("Unable to read the permission tree of the product '%1'").arg(QString::fromUtf8(productId));
		response.errorMessage = errorMessage;

		return response;
	}

	if (!tree.groups){
		return response;
	}

	if (tenantId.isEmpty() || !m_tenantManagerCompPtr.IsValid()){
		groups = *tree.groups;

		return response;
	}

	const QByteArrayList allowedPermissions = m_tenantManagerCompPtr->GetTenantPermissions(tenantId);
	for (sdl::V1_0::imtauth::CPermissionGroup group : tree.groups->ToList()){
		imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionEntry> entries;
		if (group.entries){
			for (const sdl::V1_0::imtauth::CPermissionEntry& entry : group.entries->ToList()){
				if (entry.permissionId && imtauth::HasPermission(allowedPermissions, *entry.permissionId)){
					entries.append(entry);
				}
			}
		}

		if (!entries.isEmpty()){
			group.entries = entries;
			groups.append(group);
		}
	}

	return response;
}


} // namespace imtauthgql
