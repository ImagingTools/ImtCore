// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CConnectedProductCollectionControllerComp.h>


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtauth/CConnectedProductDocumentValidatorComp.h>
#include <imtauthgql/imtauthgql.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ConnectedProducts.h>


namespace imtauthgql
{


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

		// A removed product keeps no working server; a failed revocation keeps the product to repeat the removal
		for (const QByteArray& tokenId : m_tokenManagerCompPtr->GetServerTokenIds(productPtr->GetProductId())){
			imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerCompPtr->GetToken(tokenId);
			if (!tokenPtr.IsValid() || (!tokenPtr->IsRevoked() && !m_tokenManagerCompPtr->RevokeToken(tokenId))){
				errorMessage = QStringLiteral("Unable to revoke the token '%1' of the product").arg(QString::fromUtf8(tokenId));

				return false;
			}

			if (m_tokenCacheCompPtr.IsValid()){
				m_tokenCacheCompPtr->ForgetToken(tokenId);
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

	return true;
}


} // namespace imtauthgql
