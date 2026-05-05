// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/ITenantInfo.h>


namespace imtauthgql
{


// reimplemented (sdl::imtauth::Tenants::CTenantCollectionControllerCompBase)

bool CTenantCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::Tenants::CGetTenantListGqlRequest& getTenantListRequest,
			sdl::imtauth::Tenants::CTenantItemData::V1_0& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();

	const imtauth::ITenantInfo* tenantInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr tenantDataPtr;
	if (objectCollectionIterator.GetObjectData(tenantDataPtr)){
		tenantInfoPtr = dynamic_cast<const imtauth::ITenantInfo*>(tenantDataPtr.GetPtr());
	}

	if (tenantInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CTenantCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Tenants::GetTenantListRequestInfo requestInfo = getTenantListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = objectCollectionIterator.GetObjectTypeId();
	}

	if (requestInfo.items.isNameRequested){
		representationObject.name = QString(tenantInfoPtr->GetTenantName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = QString(tenantInfoPtr->GetTenantDescription());
	}

	if (requestInfo.items.isIsActiveRequested){
		representationObject.isActive = tenantInfoPtr->IsActive();
	}

	if (requestInfo.items.isCreatedAtRequested){
		representationObject.createdAt = tenantInfoPtr->GetCreatedAt();
	}

	if (requestInfo.items.isUpdatedAtRequested){
		representationObject.updatedAt = tenantInfoPtr->GetUpdatedAt();
	}

	return true;
}


} // namespace imtauthgql
