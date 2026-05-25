// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionDocumentServiceControllerComp.h>


namespace imtauthgql
{


bool CTenantCollectionDocumentServiceControllerComp::CheckPermissions(
		const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return BaseClass::CheckPermissions(gqlRequest, errorMessage);
}


} // namespace imtauthgql
