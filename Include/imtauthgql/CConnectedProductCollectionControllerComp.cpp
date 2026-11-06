// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CConnectedProductCollectionControllerComp.h>


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtauth/CConnectedProductDocumentValidatorComp.h>
#include <imtauth/CProductPermissionSnapshot.h>
#include <imtauthgql/imtauthgql.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ConnectedProducts.h>


namespace imtauthgql
{


// A server with a token reports at least every few minutes: it publishes and its subscriptions are checked
static const qint64 s_onlineSeconds = 10 * 60;


// reimplemented (sdl::V1_0::imtauth::CConnectedProductCollectionControllerCompBase)

bool CConnectedProductCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::V1_0::imtauth::CConnectedProductsListGqlRequest& listRequest,
			sdl::V1_0::imtauth::CConnectedProductItemData& representationObject,
			QString& errorMessage) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	const imtauth::IConnectedProduct* productPtr = objectCollectionIterator.GetObjectData(dataPtr) ? dynamic_cast<const imtauth::IConnectedProduct*>(dataPtr.GetPtr()) : nullptr;
	if (productPtr == nullptr){
		errorMessage = QStringLiteral("Unable to read the product '%1'").arg(QString::fromUtf8(objectCollectionIterator.GetObjectId()));

		return false;
	}

	const auto requestInfo = listRequest.GetRequestInfo();
	if (requestInfo.items.isIdRequested){
		representationObject.id = objectCollectionIterator.GetObjectId();
	}
	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = objectCollectionIterator.GetObjectTypeId();
	}
	if (requestInfo.items.isProductIdRequested){
		representationObject.productId = productPtr->GetProductId();
	}
	if (requestInfo.items.isProductNameRequested){
		representationObject.productName = productPtr->GetProductName();
	}
	if (requestInfo.items.isProductDescriptionRequested){
		representationObject.productDescription = productPtr->GetProductDescription();
	}
	FillServerState(productPtr->GetProductId(), listRequest, representationObject);
	if (requestInfo.items.isAddedRequested){
		representationObject.added = objectCollectionIterator.GetElementInfo("Added").toDateTime().toUTC().toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
	}
	if (requestInfo.items.isLastModifiedRequested){
		representationObject.lastModified = objectCollectionIterator.GetElementInfo("LastModified").toDateTime().toUTC().toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
	}

	return true;
}


istd::IChangeableUniquePtr CConnectedProductCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::V1_0::imtauth::CConnectedProductData& representation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	const QByteArray productId = representation.productId ? QByteArray(*representation.productId) : QByteArray();
	const QString name = representation.name ? QString(*representation.name) : QString();
	if (name.isEmpty()){
		errorMessage = QStringLiteral("Product name can't be empty!");

		return nullptr;
	}

	if (!imtauth::CConnectedProductDocumentValidatorComp::CheckProductIdUnique(*m_objectCollectionCompPtr, QByteArray(), productId, errorMessage)){
		return nullptr;
	}

	imtauth::IConnectedProductUniquePtr instancePtr = m_productFactCompPtr.CreateInstance();
	imtauth::CIdentifiableConnectedProduct* productPtr = dynamic_cast<imtauth::CIdentifiableConnectedProduct*>(instancePtr.GetPtr());
	if (productPtr == nullptr){
		errorMessage = QStringLiteral("Unable to create a product entry");

		return nullptr;
	}

	newObjectId = representation.id ? QByteArray(*representation.id) : QByteArray();
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	productPtr->SetObjectUuid(newObjectId);
	productPtr->SetProductId(productId);
	productPtr->SetProductName(name);
	productPtr->SetProductDescription(representation.description ? QString(*representation.description) : QString());

	return instancePtr;
}


bool CConnectedProductCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::V1_0::imtauth::CConnectedProductItemGqlRequest& /*itemRequest*/,
			sdl::V1_0::imtauth::CConnectedProductData& representationPayload,
			QString& errorMessage) const
{
	const imtauth::CIdentifiableConnectedProduct* productPtr = dynamic_cast<const imtauth::CIdentifiableConnectedProduct*>(&data);
	if (productPtr == nullptr){
		errorMessage = QStringLiteral("Unable to read the product entry");

		return false;
	}

	representationPayload.id = productPtr->GetObjectUuid();
	representationPayload.productId = productPtr->GetProductId();
	representationPayload.name = productPtr->GetProductName();
	representationPayload.description = productPtr->GetProductDescription();

	return true;
}


bool CConnectedProductCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const ::imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::V1_0::imtauth::CConnectedProductUpdateGqlRequest& updateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	const auto arguments = updateRequest.GetRequestedArguments();
	imtauth::IConnectedProduct* productPtr = dynamic_cast<imtauth::IConnectedProduct*>(&object);
	if (productPtr == nullptr || !arguments.input.has_value() || !arguments.input->item.has_value()){
		errorMessage = QStringLiteral("Unable to update the product entry");

		return false;
	}

	// The tokens, the roles and the permission tree refer to the product by this ID
	const sdl::V1_0::imtauth::CConnectedProductData& representation = *arguments.input->item;
	if (representation.productId && *representation.productId != productPtr->GetProductId()){
		errorMessage = QStringLiteral("The product ID cannot be changed");

		return false;
	}

	const QString name = representation.name ? QString(*representation.name) : QString();
	if (name.isEmpty()){
		errorMessage = QStringLiteral("Product name can't be empty!");

		return false;
	}

	productPtr->SetProductName(name);
	if (representation.description){
		productPtr->SetProductDescription(*representation.description);
	}

	return true;
}


// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

bool CConnectedProductCollectionControllerComp::OnBeforeRemoveElements(const QByteArrayList& elementIds, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	for (const QByteArray& elementId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const imtauth::IConnectedProduct* productPtr = m_objectCollectionCompPtr->GetObjectData(elementId, dataPtr) ? dynamic_cast<const imtauth::IConnectedProduct*>(dataPtr.GetPtr()) : nullptr;
		if (productPtr == nullptr){
			errorMessage = QStringLiteral("Unable to read the product '%1'").arg(QString::fromUtf8(elementId));

			return false;
		}

		if (m_ownProductIdAttrPtr.IsValid() && productPtr->GetProductId() == *m_ownProductIdAttrPtr){
			errorMessage = QStringLiteral("This server cannot be removed from its own catalog");

			return false;
		}

		// A removed product keeps no working server; a failed revocation keeps the product to repeat the removal
		bool isRead = false;
		const QByteArrayList tokenIds = m_tokenManagerCompPtr->GetServerTokenIds(productPtr->GetProductId(), &isRead);
		if (!isRead){
			errorMessage = QStringLiteral("The tokens of the product '%1' could not be read. Try again.").arg(QString::fromUtf8(productPtr->GetProductId()));

			return false;
		}

		for (const QByteArray& tokenId : tokenIds){
			imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerCompPtr->GetToken(tokenId);
			if (!tokenPtr.IsValid() || (!tokenPtr->IsRevoked() && !m_tokenManagerCompPtr->RevokeToken(tokenId))){
				errorMessage = QStringLiteral("Unable to revoke the token '%1' of the product").arg(QString::fromUtf8(tokenId));

				return false;
			}
		}
	}

	return BaseClass::OnBeforeRemoveElements(elementIds, gqlRequest, errorMessage);
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CConnectedProductCollectionControllerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!IsSuperuserRequest(gqlRequest)){
		errorMessage = QStringLiteral("The catalog of products is available to a superuser only");

		return false;
	}

	EnsureOwnProduct();

	return true;
}


// private methods

void CConnectedProductCollectionControllerComp::EnsureOwnProduct() const
{
	if (!m_ownProductIdAttrPtr.IsValid() || QByteArray(*m_ownProductIdAttrPtr).isEmpty()){
		return;
	}

	QMutexLocker locker(&m_ownProductMutex);
	if (m_isOwnProductEnsured){
		return;
	}

	const QByteArray productId = *m_ownProductIdAttrPtr;
	const QString name = m_ownProductNameAttrPtr.IsValid() ? QString(*m_ownProductNameAttrPtr) : QString::fromUtf8(productId);

	if (imtauth::CConnectedProductDocumentValidatorComp::HasProduct(*m_objectCollectionCompPtr, productId)){
		m_isOwnProductEnsured = true;

		return;
	}

	imtauth::IConnectedProductUniquePtr instancePtr = m_productFactCompPtr.CreateInstance();
	imtauth::CIdentifiableConnectedProduct* productPtr = dynamic_cast<imtauth::CIdentifiableConnectedProduct*>(instancePtr.GetPtr());
	if (productPtr == nullptr){
		return;
	}

	const QByteArray objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	productPtr->SetObjectUuid(objectId);
	productPtr->SetProductId(productId);
	productPtr->SetProductName(name);

	m_isOwnProductEnsured = !m_objectCollectionCompPtr->InsertNewObject("ConnectedProduct", name, QString(), productPtr, objectId).isEmpty();
}


void CConnectedProductCollectionControllerComp::FillServerState(
			const QByteArray& productId,
			const sdl::V1_0::imtauth::CConnectedProductsListGqlRequest& listRequest,
			sdl::V1_0::imtauth::CConnectedProductItemData& representationObject) const
{
	const auto requestInfo = listRequest.GetRequestInfo();

	if (requestInfo.items.isProductVersionRequested && m_snapshotCollectionCompPtr.IsValid()){
		const QByteArray snapshotId = imtauth::FindProductPermissionSnapshot(*m_snapshotCollectionCompPtr, productId);
		imtbase::IObjectCollection::DataPtr dataPtr;
		const imtauth::CProductPermissionSnapshot* snapshotPtr = !snapshotId.isEmpty() && m_snapshotCollectionCompPtr->GetObjectData(snapshotId, dataPtr) ? dynamic_cast<const imtauth::CProductPermissionSnapshot*>(dataPtr.GetPtr()) : nullptr;
		if (snapshotPtr != nullptr){
			representationObject.productVersion = snapshotPtr->GetProductVersion();
		}
	}

	if (!requestInfo.items.isStatusRequested && !requestInfo.items.isTokenCreatedAtRequested && !requestInfo.items.isLastUsedAtRequested){
		return;
	}

	QDateTime oldestTokenTime;
	QDateTime lastUsedTime;
	bool hasActiveToken = false;
	for (const QByteArray& tokenId : m_tokenManagerCompPtr->GetServerTokenIds(productId)){
		imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerCompPtr->GetToken(tokenId);
		if (!tokenPtr.IsValid() || tokenPtr->IsRevoked()){
			continue;
		}

		hasActiveToken = true;
		if (!oldestTokenTime.isValid() || tokenPtr->GetCreatedAt() < oldestTokenTime){
			oldestTokenTime = tokenPtr->GetCreatedAt();
		}
		if (tokenPtr->GetLastUsedAt().isValid() && (!lastUsedTime.isValid() || tokenPtr->GetLastUsedAt() > lastUsedTime)){
			lastUsedTime = tokenPtr->GetLastUsedAt();
		}
	}

	if (requestInfo.items.isTokenCreatedAtRequested && oldestTokenTime.isValid()){
		representationObject.tokenCreatedAt = oldestTokenTime.toLocalTime().toString("dd.MM.yyyy");
	}
	if (requestInfo.items.isLastUsedAtRequested && lastUsedTime.isValid()){
		representationObject.lastUsedAt = lastUsedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
	}
	if (requestInfo.items.isStatusRequested){
		QString status;
		if (m_ownProductIdAttrPtr.IsValid() && productId == *m_ownProductIdAttrPtr){
			status = QStringLiteral("This server");
		}
		else if (!hasActiveToken){
			status = QStringLiteral("No token");
		}
		else if (!lastUsedTime.isValid()){
			status = QStringLiteral("Never connected");
		}
		else{
			status = lastUsedTime.secsTo(QDateTime::currentDateTimeUtc()) < s_onlineSeconds ? QStringLiteral("Online") : QStringLiteral("Offline");
		}

		representationObject.status = status;
	}
}


} // namespace imtauthgql
