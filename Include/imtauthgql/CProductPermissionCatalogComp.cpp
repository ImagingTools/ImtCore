// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CProductPermissionCatalogComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUuid>

// ImtCore includes
#include <imtauth/CProductPermissionSnapshot.h>
#include <imtauth/imtauth.h>
#include <imtauthgql/imtauthgql.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ProductPermissionCatalog.h>


namespace imtauthgql
{


namespace
{


const qsizetype s_maxTreeSize = 16 * 1024 * 1024;


} // namespace


// reimplemented (sdl::V1_0::imtauth::CProductPermissionCatalogGqlHandlerCompBase)

sdl::V1_0::imtauth::CPublishProductPermissionsPayload CProductPermissionCatalogComp::OnPublishProductPermissions(
			const sdl::V1_0::imtauth::CPublishProductPermissionsGqlRequest& publishRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CPublishProductPermissionsPayload response;
	response.success = false;

	const auto arguments = publishRequest.GetRequestedArguments();
	const QByteArray productId = arguments.input.has_value() && arguments.input->productId ? QByteArray(*arguments.input->productId) : QByteArray();

	// Only the server of the product publishes its tree
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr || productId.isEmpty() || gqlContextPtr->GetClientProductId() != productId
				|| !imtauth::HasPermission(gqlContextPtr->GetScopes(), imtauth::s_productServerPublishPermissionsPermission, false)){
		errorMessage = QStringLiteral("The permissions of the product '%1' are published by its server only").arg(QString::fromUtf8(productId));

		return response;
	}

	QJsonObject treeObject;
	if (!arguments.input->tree || !arguments.input->tree->WriteToJsonObject(treeObject)){
		errorMessage = QStringLiteral("The permission tree is not valid");

		return response;
	}

	const QByteArray tree = QJsonDocument(treeObject).toJson(QJsonDocument::Compact);
	if (tree.size() > s_maxTreeSize){
		errorMessage = QStringLiteral("The permission tree is too large");

		return response;
	}

	const QByteArray hash = QCryptographicHash::hash(tree, QCryptographicHash::Sha256).toHex();
	const QString version = arguments.input->productVersion ? QString(*arguments.input->productVersion) : QString();

	// One writer at a time: two publications must not create two trees of a product
	QMutexLocker publishLocker(&m_publishMutex);

	// A tree that could not be found must not be stored as a second one
	bool isRead = false;
	QByteArray snapshotId = imtauth::FindProductPermissionSnapshot(*m_snapshotCollectionCompPtr, productId, &isRead);
	if (!isRead){
		errorMessage = QStringLiteral("Unable to read the stored permission tree");

		return response;
	}

	imtauth::CIdentifiableProductPermissionSnapshot snapshot;
	if (!snapshotId.isEmpty()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const imtauth::CProductPermissionSnapshot* storedPtr = m_snapshotCollectionCompPtr->GetObjectData(snapshotId, dataPtr) ? dynamic_cast<const imtauth::CProductPermissionSnapshot*>(dataPtr.GetPtr()) : nullptr;
		if (storedPtr == nullptr){
			errorMessage = QStringLiteral("Unable to read the stored permission tree");

			return response;
		}

		if (storedPtr->GetTreeHash() == hash && storedPtr->GetProductVersion() == version){
			response.success = true;
			response.changed = false;

			return response;
		}
	}

	snapshot.SetProductId(productId);
	snapshot.SetProductVersion(version);
	snapshot.SetTreeHash(hash);
	snapshot.SetTree(tree);
	snapshot.SetPublishedAt(QDateTime::currentDateTimeUtc());

	bool isStored = false;
	if (snapshotId.isEmpty()){
		snapshotId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
		snapshot.SetObjectUuid(snapshotId);
		isStored = !m_snapshotCollectionCompPtr->InsertNewObject("ProductPermissionSnapshot", QString::fromUtf8(productId), QString(), &snapshot, snapshotId).isEmpty();
	}
	else{
		snapshot.SetObjectUuid(snapshotId);
		isStored = m_snapshotCollectionCompPtr->SetObjectData(snapshotId, snapshot);
	}

	if (!isStored){
		errorMessage = QStringLiteral("Unable to store the permission tree");

		return response;
	}

	SendInfoMessage(0, QStringLiteral("Product '%1' published the permission tree of version '%2'").arg(QString::fromUtf8(productId), version), "CProductPermissionCatalogComp");

	response.success = true;
	response.changed = true;

	return response;
}


sdl::V1_0::imtauth::CProductPermissionStatus CProductPermissionCatalogComp::OnGetProductPermissionStatus(
			const sdl::V1_0::imtauth::CGetProductPermissionStatusGqlRequest& statusRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CProductPermissionStatus response;

	if (!IsSuperuserRequest(gqlRequest)){
		errorMessage = QStringLiteral("The catalog of products is available to a superuser only");

		return response;
	}

	const auto arguments = statusRequest.GetRequestedArguments();
	const QByteArray productId = arguments.input.has_value() && arguments.input->productId ? QByteArray(*arguments.input->productId) : QByteArray();

	response.productId = productId;
	response.isPublished = false;

	const QByteArray snapshotId = imtauth::FindProductPermissionSnapshot(*m_snapshotCollectionCompPtr, productId);
	if (snapshotId.isEmpty()){
		return response;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	const imtauth::CProductPermissionSnapshot* snapshotPtr = m_snapshotCollectionCompPtr->GetObjectData(snapshotId, dataPtr) ? dynamic_cast<const imtauth::CProductPermissionSnapshot*>(dataPtr.GetPtr()) : nullptr;
	if (snapshotPtr == nullptr){
		errorMessage = QStringLiteral("Unable to read the stored permission tree");

		return response;
	}

	response.isPublished = true;
	response.productVersion = snapshotPtr->GetProductVersion();
	response.publishedAt = snapshotPtr->GetPublishedAt().toString(Qt::ISODate);

	return response;
}


} // namespace imtauthgql
