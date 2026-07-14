// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantRelationshipDocumentNameProviderComp.h>


// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtbase/ICollectionInfo.h>


namespace imtauth
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CTenantRelationshipDocumentNameProviderComp::GetDefaultDocumentName(const QByteArray& /*objectId*/, const istd::IChangeable& document) const
{
	const ITenantRelationshipInfo* relationshipPtr = dynamic_cast<const ITenantRelationshipInfo*>(&document);
	if (relationshipPtr != nullptr){
		const QByteArray targetTenantId = relationshipPtr->GetTargetTenantId();
		if (m_tenantCollectionCompPtr.IsValid() && !targetTenantId.isEmpty()){
			const QString targetTenantName = m_tenantCollectionCompPtr->GetElementInfo(targetTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
			if (!targetTenantName.isEmpty()){
				return targetTenantName;
			}
		}

		return QString::fromUtf8(targetTenantId);
	}

	return QString();
}


} // namespace imtauth
