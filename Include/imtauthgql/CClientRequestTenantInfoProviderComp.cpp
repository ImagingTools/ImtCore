// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestTenantInfoProviderComp.h>


// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtauth::ITenantInfoProvider)

const imtbase::ICollectionInfo& CClientRequestTenantInfoProviderComp::GetTenantList() const
{
	static imtbase::CCollectionInfo emptyCollection;

	return emptyCollection;
}


imtauth::ITenantInfoSharedPtr CClientRequestTenantInfoProviderComp::GetTenant(const QByteArray& tenantId, const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!m_tenantFactoryCompPtr.IsValid()){
		return nullptr;
	}

	namespace tenantssdl = sdl::imtauth::Tenants;

	tenantssdl::TenantItemRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = tenantId;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantssdl::CTenantItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	imtgql::IGqlContext* gqlContextPtr = imtgql::CGqlRequestContextManager::GetContext();
	if (gqlContextPtr != nullptr){
		istd::IChangeableUniquePtr clonedPtr = gqlContextPtr->CloneMe();
		imtgql::IGqlContextUniquePtr castedPtr;
		castedPtr.MoveCastedPtr(std::move(clonedPtr));
		gqlRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(castedPtr));
	}

	QString errorMessage;
	tenantssdl::CTenantData payload = SendModelRequest<tenantssdl::CTenantData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	if (!payload.Version_1_0.HasValue()){
		return nullptr;
	}

	imtauth::ITenantInfoUniquePtr tenantInfoPtr = m_tenantFactoryCompPtr.CreateInstance();
	if (!tenantInfoPtr.IsValid()){
		return nullptr;
	}

	if (payload.Version_1_0->name){
		tenantInfoPtr->SetTenantName(*payload.Version_1_0->name);
	}

	if (payload.Version_1_0->description){
		tenantInfoPtr->SetTenantDescription(*payload.Version_1_0->description);
	}

	if (payload.Version_1_0->isActive){
		tenantInfoPtr->SetActive(*payload.Version_1_0->isActive);
	}

	return imtauth::ITenantInfoSharedPtr::CreateFromUnique(tenantInfoPtr);
}


} // namespace imtauthgql


