// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtservergql/CCollectionDocumentServiceControllerComp.h>


namespace imtauthgql
{


/**
 * Tenant-aware collection document service controller.
 *
 * Extends imtservergql::CCollectionDocumentServiceControllerComp with
 * permission checks that enforce:
 * - A new tenant needs /OrganizationManagement/Add (a superuser excepted); created inside
 *   an active tenant it is its sub-organization and the tenant scope must contain the permission too.
 * - Opening/editing/saving a tenant is only allowed when the session's active
 *   tenantId matches the tenant being accessed (i.e. while authorized in
 *   Tenant A the user cannot open Tenant B).
 */
class CTenantCollectionDocumentServiceControllerComp:
		public imtservergql::CCollectionDocumentServiceControllerComp
{
public:
	typedef imtservergql::CCollectionDocumentServiceControllerComp BaseClass;

	I_BEGIN_COMPONENT(CTenantCollectionDocumentServiceControllerComp);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager, narrows the sub-organizations to the scope of a saved organization", false, "TenantManager");
	I_END_COMPONENT;

protected:
	// reimplemented (CTenantCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CDocumentList OnGetOpenedDocumentList(
		const sdl::V1_0::imtbase::CGetOpenedDocumentListGqlRequest& getOpenedDocumentListRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentInfo OnCreateNewDocument(
		const sdl::V1_0::imtbase::CCreateNewDocumentGqlRequest& createNewDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentInfo OnOpenDocument(
		const sdl::V1_0::imtbase::COpenDocumentGqlRequest& openDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnSaveDocument(
		const sdl::V1_0::imtbase::CSaveDocumentGqlRequest& saveDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;

private:
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauthgql
