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
	// reimplemented (CGraphQlHandlerCompBase)
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
};


} // namespace imtauthgql
