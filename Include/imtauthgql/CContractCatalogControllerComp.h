// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtauth/IContractTypeCatalog.h>
#include <imtauth/IPartyRoleCatalog.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ContractCatalogs_fwd.h>


namespace imtauthgql
{


/**
	The roles of the parties and the types of contracts an organization uses: the system ones and its own.
*/
class CContractCatalogControllerComp: public sdl::V1_0::imtauth::CContractCatalogsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CContractCatalogsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CContractCatalogControllerComp);
		I_ASSIGN(m_roleCatalogCompPtr, "PartyRoleCatalog", "Roles of the parties", true, "PartyRoleCatalog");
		I_ASSIGN(m_roleFactoryCompPtr, "RoleFactory", "Factory of a role", true, "PartyRoleInfo");
		I_ASSIGN(m_typeCatalogCompPtr, "ContractTypeCatalog", "Types of contracts", true, "ContractTypeCatalog");
		I_ASSIGN(m_typeFactoryCompPtr, "TypeFactory", "Factory of a type", true, "ContractTypeInfo");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Organizations", true, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Members and their permissions in the organizations", true, "TenantMembershipManager");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translations of the system roles and types", false, "TranslationManager");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CContractCatalogsGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetPartyRolesPayload OnGetPartyRoles(
				const sdl::V1_0::imtauth::CGetPartyRolesGqlRequest& getPartyRolesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetContractTypesPayload OnGetContractTypes(
				const sdl::V1_0::imtauth::CGetContractTypesGqlRequest& getContractTypesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCatalogCommandPayload OnSavePartyRole(
				const sdl::V1_0::imtauth::CSavePartyRoleGqlRequest& savePartyRoleRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCatalogCommandPayload OnSetPartyRoleArchived(
				const sdl::V1_0::imtauth::CSetPartyRoleArchivedGqlRequest& setPartyRoleArchivedRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCatalogCommandPayload OnSaveContractType(
				const sdl::V1_0::imtauth::CSaveContractTypeGqlRequest& saveContractTypeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCatalogCommandPayload OnSetContractTypeArchived(
				const sdl::V1_0::imtauth::CSetContractTypeArchivedGqlRequest& setContractTypeArchivedRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	QByteArray GetLanguageId(const ::imtgql::CGqlRequest& gqlRequest) const;

private:
	I_REF(imtauth::IPartyRoleCatalog, m_roleCatalogCompPtr);
	I_FACT(imtauth::IPartyRoleInfo, m_roleFactoryCompPtr);
	I_REF(imtauth::IContractTypeCatalog, m_typeCatalogCompPtr);
	I_FACT(imtauth::IContractTypeInfo, m_typeFactoryCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtauthgql


