// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionDocumentServiceControllerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlParamObject.h>
#include <imtgql/IGqlContext.h>


namespace imtauthgql
{


bool CTenantCollectionDocumentServiceControllerComp::CheckPermissions(
		const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	// First run any base permission checks (feature-based etc.).
	if (!BaseClass::CheckPermissions(gqlRequest, errorMessage)){
		return false;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		// No context available - nothing more to check.
		return true;
	}

	QByteArray currentTenantId = gqlContextPtr->GetTenantId();
	if (currentTenantId.isEmpty()){
		errorMessage = QStringLiteral(
			"Access denied: no organization selected. Please switch to an organization first");
		return false;
	}

	// For OpenDocument the input contains "id" which equals the tenant document
	// objectId (and for tenant documents the objectId equals the tenant ID).
	// Reject attempts to open a tenant other than the one currently switched to.
	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
		if (!objectId.isEmpty() && objectId != currentTenantId){
			errorMessage = QStringLiteral(
				"Access denied: you must switch to this organization before accessing it");
			return false;
		}
	}

	return true;
}


} // namespace imtauthgql
