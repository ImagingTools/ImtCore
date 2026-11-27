// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CContractCatalogControllerComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtsdl/imtsdl.h>
#include <imtsdl/TElementList.h>
#include <imtauthgql/TenantAccess.h>
#include <imtgql/IGqlContext.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ContractCatalogs.h>


namespace imtauthgql
{


namespace sdlauth = sdl::V1_0::imtauth;


namespace
{


const QByteArrayList s_viewCatalogPermissions = {s_viewContractsPermission, s_editContractsPermission, s_editPartyRolesPermission, s_editContractTypesPermission};


} // anonymous namespace


// protected methods

// reimplemented (sdl::V1_0::imtauth::CContractCatalogsGqlHandlerCompBase)

sdlauth::CGetPartyRolesPayload CContractCatalogControllerComp::OnGetPartyRoles(
			const sdlauth::CGetPartyRolesGqlRequest& getPartyRolesRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CGetPartyRolesPayload response;

	const sdlauth::GetPartyRolesRequestArguments arguments = getPartyRolesRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();
	const bool includeArchived = arguments.input ? arguments.input->includeArchived.value_or(false) : false;

	QString accessError;
	if (!HasTenantManageAccess(*m_tenantManagerCompPtr, m_membershipManagerCompPtr.GetPtr(), gqlRequest, tenantId, s_viewCatalogPermissions, accessError)){
		response.errorMessage = accessError;

		return response;
	}

	bool isRead = false;
	const imtauth::PartyRoleInfoList roles = m_roleCatalogCompPtr->GetRoles(tenantId, includeArchived, &isRead);
	if (!isRead){
		response.errorMessage = QStringLiteral("The roles of the organization could not be read");

		return response;
	}

	const QByteArray languageId = GetLanguageId(gqlRequest);

	response.roles.Emplace();
	for (const imtauth::IPartyRoleInfoSharedPtr& rolePtr : roles){
		const bool isSystem = rolePtr->GetOwnerTenantId().isEmpty();

		sdlauth::CPartyRole roleData;
		roleData.roleId = rolePtr->GetRoleId();
		roleData.name = isSystem ? imtbase::TranslateAttributeText(m_translationManagerCompPtr.GetPtr(), rolePtr->GetName(), languageId) : rolePtr->GetName();
		roleData.description = isSystem ? imtbase::TranslateAttributeText(m_translationManagerCompPtr.GetPtr(), rolePtr->GetDescription(), languageId) : rolePtr->GetDescription();
		roleData.ownerTenantId = rolePtr->GetOwnerTenantId();
		roleData.isPartyRole = (rolePtr->GetApplicability() & imtauth::IPartyRoleInfo::RA_PARTY) != 0;
		roleData.isLinkRole = (rolePtr->GetApplicability() & imtauth::IPartyRoleInfo::RA_LINK) != 0;
		roleData.pairedRoleId = rolePtr->GetPairedRoleId();
		roleData.templateProductId = rolePtr->GetTemplateProductId();
		roleData.templatePermissionIds = imtsdl::ToElementList(rolePtr->GetTemplatePermissionIds());
		roleData.isArchived = rolePtr->IsArchived();
		roleData.isSystem = isSystem;
		response.roles->push_back(roleData);
	}

	return response;
}


sdlauth::CGetContractTypesPayload CContractCatalogControllerComp::OnGetContractTypes(
			const sdlauth::CGetContractTypesGqlRequest& getContractTypesRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CGetContractTypesPayload response;

	const sdlauth::GetContractTypesRequestArguments arguments = getContractTypesRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();
	const bool includeArchived = arguments.input ? arguments.input->includeArchived.value_or(false) : false;

	QString accessError;
	if (!HasTenantManageAccess(*m_tenantManagerCompPtr, m_membershipManagerCompPtr.GetPtr(), gqlRequest, tenantId, s_viewCatalogPermissions, accessError)){
		response.errorMessage = accessError;

		return response;
	}

	bool isRead = false;
	const imtauth::ContractTypeInfoList types = m_typeCatalogCompPtr->GetTypes(tenantId, includeArchived, &isRead);
	if (!isRead){
		response.errorMessage = QStringLiteral("The types of contracts of the organization could not be read");

		return response;
	}

	const QByteArray languageId = GetLanguageId(gqlRequest);

	response.types.Emplace();
	for (const imtauth::IContractTypeInfoSharedPtr& typePtr : types){
		const bool isSystem = typePtr->GetOwnerTenantId().isEmpty();

		sdlauth::CContractType typeData;
		typeData.typeId = typePtr->GetTypeId();
		typeData.name = isSystem ? imtbase::TranslateAttributeText(m_translationManagerCompPtr.GetPtr(), typePtr->GetName(), languageId) : typePtr->GetName();
		typeData.description = isSystem ? imtbase::TranslateAttributeText(m_translationManagerCompPtr.GetPtr(), typePtr->GetDescription(), languageId) : typePtr->GetDescription();
		typeData.ownerTenantId = typePtr->GetOwnerTenantId();
		typeData.numberPrefix = typePtr->GetNumberPrefix();
		typeData.linkScheme = imtsdl::ConvertEnum<sdlauth::ContractLinkScheme>(typePtr->GetLinkScheme());
		typeData.starRoleId = typePtr->GetStarRoleId();
		typeData.termDays = typePtr->GetTermDays();
		typeData.autoRenew = typePtr->IsAutoRenew();
		typeData.renewalPeriodDays = typePtr->GetRenewalPeriodDays();
		typeData.renewalNoticeDays = typePtr->GetRenewalNoticeDays();
		typeData.terminationNoticeDays = typePtr->GetTerminationNoticeDays();
		typeData.isArchived = typePtr->IsArchived();
		typeData.isSystem = isSystem;

		typeData.roleSlots.Emplace();
		for (const imtauth::IContractInfo::RoleSlot& slot : typePtr->GetRoleSlots()){
			sdlauth::CContractTypeRoleSlot slotData;
			slotData.roleId = slot.roleId;
			slotData.minCount = slot.minCount;
			slotData.maxCount = slot.maxCount;
			typeData.roleSlots->push_back(slotData);
		}

		typeData.allocationTemplates.Emplace();
		for (const imtauth::IContractTypeInfo::AllocationTemplate& allocationTemplate : typePtr->GetAllocationTemplates()){
			sdlauth::CContractAllocationTemplate templateData;
			templateData.grantorRoleId = allocationTemplate.grantorRoleId;
			templateData.granteeRoleId = allocationTemplate.granteeRoleId;
			templateData.productId = allocationTemplate.productId;
			templateData.permissionIds = imtsdl::ToElementList(allocationTemplate.permissionIds);
			typeData.allocationTemplates->push_back(templateData);
		}

		response.types->push_back(typeData);
	}

	return response;
}


sdlauth::CCatalogCommandPayload CContractCatalogControllerComp::OnSavePartyRole(
			const sdlauth::CSavePartyRoleGqlRequest& savePartyRoleRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CCatalogCommandPayload response;
	response.success = false;

	const sdlauth::SavePartyRoleRequestArguments arguments = savePartyRoleRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();

	QString error;
	if (!HasTenantManageAccess(*m_tenantManagerCompPtr, m_membershipManagerCompPtr.GetPtr(), gqlRequest, tenantId, {s_editPartyRolesPermission}, error)){
		response.errorMessage = error;

		return response;
	}

	imtauth::IPartyRoleInfoUniquePtr rolePtr = m_roleFactoryCompPtr.CreateInstance();
	if (!rolePtr.IsValid() || !arguments.input->role){
		response.errorMessage = QStringLiteral("The role is missing");

		return response;
	}

	const sdlauth::CPartyRoleInput& input = *arguments.input->role;
	int applicability = 0;
	if (input.isPartyRole.value_or(true)){
		applicability |= imtauth::IPartyRoleInfo::RA_PARTY;
	}

	if (input.isLinkRole.value_or(true)){
		applicability |= imtauth::IPartyRoleInfo::RA_LINK;
	}

	rolePtr->SetRoleId(input.roleId.value_or(QByteArray()));
	rolePtr->SetName(input.name.value_or(QString()));
	rolePtr->SetDescription(input.description.value_or(QString()));
	rolePtr->SetOwnerTenantId(tenantId);
	rolePtr->SetApplicability(applicability);
	rolePtr->SetPairedRoleId(input.pairedRoleId.value_or(QByteArray()));
	rolePtr->SetTemplateProductId(input.templateProductId.value_or(QByteArray()));
	rolePtr->SetTemplatePermissionIds(imtsdl::FromElementList(input.templatePermissionIds));

	QByteArray roleId = rolePtr->GetRoleId();
	if (roleId.isEmpty()){
		roleId = m_roleCatalogCompPtr->CreateRole(*rolePtr, error);
	}
	else if (!m_roleCatalogCompPtr->UpdateRole(*rolePtr, error)){
		roleId.clear();
	}

	if (roleId.isEmpty()){
		response.errorMessage = error;

		return response;
	}

	response.success = true;
	response.itemId = roleId;

	return response;
}


sdlauth::CCatalogCommandPayload CContractCatalogControllerComp::OnSetPartyRoleArchived(
			const sdlauth::CSetPartyRoleArchivedGqlRequest& setPartyRoleArchivedRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CCatalogCommandPayload response;
	response.success = false;

	const sdlauth::SetPartyRoleArchivedRequestArguments arguments = setPartyRoleArchivedRequest.GetRequestedArguments();
	const sdlauth::CSetCatalogItemArchivedInput input = arguments.input ? *arguments.input : sdlauth::CSetCatalogItemArchivedInput();
	const QByteArray tenantId = input.tenantId.value_or(QByteArray());
	const QByteArray roleId = input.itemId.value_or(QByteArray());

	QString error;
	if (!HasTenantManageAccess(*m_tenantManagerCompPtr, m_membershipManagerCompPtr.GetPtr(), gqlRequest, tenantId, {s_editPartyRolesPermission}, error)
				|| !m_roleCatalogCompPtr->SetRoleArchived(tenantId, roleId, input.isArchived.value_or(true), error)){
		response.errorMessage = error;

		return response;
	}

	response.success = true;
	response.itemId = roleId;

	return response;
}


sdlauth::CCatalogCommandPayload CContractCatalogControllerComp::OnSaveContractType(
			const sdlauth::CSaveContractTypeGqlRequest& saveContractTypeRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CCatalogCommandPayload response;
	response.success = false;

	const sdlauth::SaveContractTypeRequestArguments arguments = saveContractTypeRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();

	QString error;
	if (!HasTenantManageAccess(*m_tenantManagerCompPtr, m_membershipManagerCompPtr.GetPtr(), gqlRequest, tenantId, {s_editContractTypesPermission}, error)){
		response.errorMessage = error;

		return response;
	}

	imtauth::IContractTypeInfoUniquePtr typePtr = m_typeFactoryCompPtr.CreateInstance();
	if (!typePtr.IsValid() || !arguments.input->contractType){
		response.errorMessage = QStringLiteral("The type of contracts is missing");

		return response;
	}

	const sdlauth::CContractTypeInput& input = *arguments.input->contractType;
	typePtr->SetTypeId(input.typeId.value_or(QByteArray()));
	typePtr->SetName(input.name.value_or(QString()));
	typePtr->SetDescription(input.description.value_or(QString()));
	typePtr->SetOwnerTenantId(tenantId);
	typePtr->SetNumberPrefix(input.numberPrefix.value_or(QString()));
	typePtr->SetLinkScheme(input.linkScheme ? imtsdl::ConvertEnum<imtauth::IContractTypeInfo::LinkScheme>(*input.linkScheme) : imtauth::IContractTypeInfo::LS_MANUAL);
	typePtr->SetStarRoleId(input.starRoleId.value_or(QByteArray()));
	typePtr->SetTermDays(input.termDays.value_or(0));
	typePtr->SetAutoRenew(input.autoRenew.value_or(false));
	typePtr->SetRenewalPeriodDays(input.renewalPeriodDays.value_or(0));
	typePtr->SetRenewalNoticeDays(input.renewalNoticeDays.value_or(0));
	typePtr->SetTerminationNoticeDays(input.terminationNoticeDays.value_or(0));

	imtauth::IContractInfo::RoleSlots roleSlots;
	if (input.roleSlots){
		for (const istd::TNullableValue<sdlauth::CContractTypeRoleSlotInput>& item : *input.roleSlots){
			if (item){
				imtauth::IContractInfo::RoleSlot slot;
				slot.roleId = item->roleId.value_or(QByteArray());
				slot.minCount = item->minCount.value_or(0);
				slot.maxCount = item->maxCount.value_or(0);
				roleSlots.append(slot);
			}
		}
	}

	typePtr->SetRoleSlots(roleSlots);

	imtauth::IContractTypeInfo::AllocationTemplates allocationTemplates;
	if (input.allocationTemplates){
		for (const istd::TNullableValue<sdlauth::CContractAllocationTemplateInput>& item : *input.allocationTemplates){
			if (item){
				imtauth::IContractTypeInfo::AllocationTemplate allocationTemplate;
				allocationTemplate.grantorRoleId = item->grantorRoleId.value_or(QByteArray());
				allocationTemplate.granteeRoleId = item->granteeRoleId.value_or(QByteArray());
				allocationTemplate.productId = item->productId.value_or(QByteArray());
				allocationTemplate.permissionIds = imtsdl::FromElementList(item->permissionIds);
				allocationTemplates.append(allocationTemplate);
			}
		}
	}

	typePtr->SetAllocationTemplates(allocationTemplates);

	QByteArray typeId = typePtr->GetTypeId();
	if (typeId.isEmpty()){
		typeId = m_typeCatalogCompPtr->CreateType(*typePtr, error);
	}
	else if (!m_typeCatalogCompPtr->UpdateType(*typePtr, error)){
		typeId.clear();
	}

	if (typeId.isEmpty()){
		response.errorMessage = error;

		return response;
	}

	response.success = true;
	response.itemId = typeId;

	return response;
}


sdlauth::CCatalogCommandPayload CContractCatalogControllerComp::OnSetContractTypeArchived(
			const sdlauth::CSetContractTypeArchivedGqlRequest& setContractTypeArchivedRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CCatalogCommandPayload response;
	response.success = false;

	const sdlauth::SetContractTypeArchivedRequestArguments arguments = setContractTypeArchivedRequest.GetRequestedArguments();
	const sdlauth::CSetCatalogItemArchivedInput input = arguments.input ? *arguments.input : sdlauth::CSetCatalogItemArchivedInput();
	const QByteArray tenantId = input.tenantId.value_or(QByteArray());
	const QByteArray typeId = input.itemId.value_or(QByteArray());

	QString error;
	if (!HasTenantManageAccess(*m_tenantManagerCompPtr, m_membershipManagerCompPtr.GetPtr(), gqlRequest, tenantId, {s_editContractTypesPermission}, error)
				|| !m_typeCatalogCompPtr->SetTypeArchived(tenantId, typeId, input.isArchived.value_or(true), error)){
		response.errorMessage = error;

		return response;
	}

	response.success = true;
	response.itemId = typeId;

	return response;
}


// private methods

QByteArray CContractCatalogControllerComp::GetLanguageId(const ::imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();

	return (contextPtr != nullptr) ? contextPtr->GetLanguageId() : QByteArray();
}


} // namespace imtauthgql


