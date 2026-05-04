// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestTenantManagerComp.h>


// ImtCore includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantCollectionDocumentManager.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::ITenantManager)

QByteArrayList CClientRequestTenantManagerComp::GetTenantIds() const
{
	return GetElementIds(QByteArrayLiteral("Tenants"));
}


QByteArray CClientRequestTenantManagerComp::CreateTenant(const QString& tenantName, const QString& description)
{
	namespace tenantssdl = sdl::imtauth::Tenants;

	tenantssdl::TenantAddRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	arguments.input.Version_1_0->typeId = QByteArrayLiteral("Tenant");
	arguments.input.Version_1_0->name = tenantName;
	arguments.input.Version_1_0->description = description;

	tenantssdl::CTenantData::V1_0 tenantData;
	tenantData.description = description;
	tenantData.name = tenantName;
	tenantData.isActive = true;

	arguments.input.Version_1_0->item = tenantData;

	sdl::imtbase::ImtCollection::CAddedNotificationPayload payload;
	bool ok = SendModelRequestInternal<tenantssdl::TenantAddRequestArguments, sdl::imtbase::ImtCollection::CAddedNotificationPayload, tenantssdl::CTenantAddGqlRequest>(arguments, payload);
	if (!ok){
		return QByteArray();
	}

	if (!payload.Version_1_0->id.HasValue()){
		return QByteArray();
	}

	return *payload.Version_1_0->id;
}


bool CClientRequestTenantManagerComp::RemoveTenant(const QByteArray& tenantId)
{
	return RemoveElements(QByteArrayLiteral("Tenants"), {tenantId});
}


imtauth::ITenantInfoUniquePtr CClientRequestTenantManagerComp::GetTenant(const QByteArray& tenantId) const
{
	if (!m_tenantFactoryCompPtr.IsValid()){
		return nullptr;
	}

	sdl::imtauth::Tenants::CTenantData::V1_0 tenantData;
	bool ok = GetTenantDataSdl(tenantId, tenantData);
	if (!ok){
		return nullptr;
	}

	imtauth::ITenantInfoUniquePtr tenantInfoPtr = m_tenantFactoryCompPtr.CreateInstance();
	if (!tenantInfoPtr.IsValid()){
		return nullptr;
	}

	if (tenantData.name){
		tenantInfoPtr->SetTenantName(*tenantData.name);
	}

	if (tenantData.description){
		tenantInfoPtr->SetTenantDescription(*tenantData.description);
	}

	if (tenantData.isActive){
		tenantInfoPtr->SetActive(*tenantData.isActive);
	}

	return tenantInfoPtr;
}


bool CClientRequestTenantManagerComp::UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description)
{
	sdl::imtauth::Tenants::CTenantData::V1_0 tenantData;
	bool ok = GetTenantDataSdl(tenantId, tenantData);
	if (!ok){
		return false;
	}

	tenantData.name = tenantName;
	tenantData.description = description;

	return SetTenantDataSdl(tenantId, tenantData);
}


bool CClientRequestTenantManagerComp::SetTenantActive(const QByteArray& tenantId, bool isActive)
{
	sdl::imtauth::Tenants::CTenantData::V1_0 tenantData;
	bool ok = GetTenantDataSdl(tenantId, tenantData);
	if (!ok){
		return false;
	}

	tenantData.isActive = isActive;

	return SetTenantDataSdl(tenantId, tenantData);
}


// private methods

bool CClientRequestTenantManagerComp::GetTenantDataSdl(const QByteArray& tenantId, sdl::imtauth::Tenants::CTenantData::V1_0& tenantData) const
{
	namespace docMgrSdl = sdl::imtauth::TenantCollectionDocumentManager;

	docMgrSdl::GetTenantRepresentationRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->documentId = tenantId;

	docMgrSdl::CTenantData payload;
	bool ok = SendModelRequestInternal<docMgrSdl::GetTenantRepresentationRequestArguments, docMgrSdl::CTenantData, docMgrSdl::CGetTenantRepresentationGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	tenantData = *payload.Version_1_0;

	return true;
}


bool CClientRequestTenantManagerComp::SetTenantDataSdl(const QByteArray& tenantId, const sdl::imtauth::Tenants::CTenantData::V1_0& tenantData) const
{
	namespace tenantssdl = sdl::imtauth::Tenants;

	tenantssdl::TenantUpdateRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = tenantId;
	arguments.input.Version_1_0->typeId = QByteArray("Tenant");
	arguments.input.Version_1_0->item = tenantData;

	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload payload;
	bool ok = SendModelRequestInternal<tenantssdl::TenantUpdateRequestArguments, sdl::imtbase::ImtCollection::CUpdatedNotificationPayload, tenantssdl::CTenantUpdateGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.Version_1_0->id){
		return false;
	}

	return !payload.Version_1_0->id->isEmpty();
}


} // namespace imtauthgql


