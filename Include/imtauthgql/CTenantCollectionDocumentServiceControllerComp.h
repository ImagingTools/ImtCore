// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CCollectionDocumentServiceControllerComp.h>


namespace imtauthgql
{


/**
 * Tenant-aware collection document service controller.
 *
 * Extends imtservergql::CCollectionDocumentServiceControllerComp with a
 * permission check that ensures the user has switched to a tenant before
 * accessing tenant documents and that the document being opened belongs to
 * that same tenant (i.e. while authorized in Tenant A the user cannot open
 * Tenant B).
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
};


} // namespace imtauthgql
