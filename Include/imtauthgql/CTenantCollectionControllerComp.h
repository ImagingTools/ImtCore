// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtauth/ITenantInfoProvider.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


namespace imtauthgql
{


class CTenantCollectionControllerComp: public sdl::imtauth::Tenants::CTenantCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::Tenants::CTenantCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantCollectionControllerComp);
		I_ASSIGN(m_tenantInfoProviderCompPtr, "TenantInfoProvider", "Tenant info provider", false, "TenantInfoProvider");
		I_ASSIGN(m_tenantInfoFactCompPtr, "TenantFactory", "Factory used for creation of the new tenant instance", true, "TenantFactory");
	I_END_COMPONENT;

protected:
	virtual bool FillObjectFromRepresentation(
				const sdl::imtauth::Tenants::CTenantData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& newObjectId,
				QString& errorMessage) const;

	// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::imtauth::Tenants::CTenantCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::Tenants::CGetTenantListGqlRequest& getTenantListRequest,
				sdl::imtauth::Tenants::CTenantItemData::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::imtauth::Tenants::CTenantData::V1_0& tenantDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtauth::Tenants::CTenantUpdateGqlRequest& tenantUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtauth::ITenantInfoProvider, m_tenantInfoProviderCompPtr);
	I_FACT(imtauth::ITenantInfo, m_tenantInfoFactCompPtr);
};


} // namespace imtauthgql


