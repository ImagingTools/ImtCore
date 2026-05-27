// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CCollectionDocumentServiceControllerComp.h>


namespace imtauthgql
{


/**
 * Tenant-aware collection document service controller.
 *
 * Extends imtservergql::CCollectionDocumentServiceControllerComp with
 * permission checks that enforce:
 * - Creating a new tenant is only allowed when the session has NO active
 *   tenant (tenantId is empty).
 * - Opening/editing a tenant is only allowed when the session's active
 *   tenantId matches the tenant being accessed (i.e. while authorized in
 *   Tenant A the user cannot open Tenant B).
 */
class CTenantCollectionDocumentServiceControllerComp:
		public imtservergql::CCollectionDocumentServiceControllerComp
{
public:
	typedef imtservergql::CCollectionDocumentServiceControllerComp BaseClass;

	I_BEGIN_COMPONENT(CTenantCollectionDocumentServiceControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	// reimplemented (CGraphQlHandlerCompBase)
	virtual sdl::imtbase::CollectionDocumentService::CDocumentInfo OnCreateNewDocument(
		const sdl::imtbase::CollectionDocumentService::CCreateNewDocumentGqlRequest& createNewDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentInfo OnOpenDocument(
		const sdl::imtbase::CollectionDocumentService::COpenDocumentGqlRequest& openDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
};


} // namespace imtauthgql
