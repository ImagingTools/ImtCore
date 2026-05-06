// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantDocumentNameProviderComp.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>


namespace imtauth
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CTenantDocumentNameProviderComp::GetDefaultDocumentName(const QByteArray& /*objectId*/, const istd::IChangeable& document) const
{
	const ITenantInfo* tenantPtr = dynamic_cast<const ITenantInfo*>(&document);
	if (tenantPtr != nullptr){
		return tenantPtr->GetTenantName();
	}

	return QString();
}


} // namespace imtauth
