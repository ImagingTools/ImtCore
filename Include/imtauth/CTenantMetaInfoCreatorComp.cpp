// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/ITenantInfo.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CTenantMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (m_objectTypeIdsAttrPtr.FindValue(typeId) < 0){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const imtauth::ITenantInfo* tenantInfoPtr = dynamic_cast<const imtauth::ITenantInfo*>(dataPtr);
	if (tenantInfoPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_ID, tenantInfoPtr->GetTenantId());
	metaInfoPtr->SetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_NAME, tenantInfoPtr->GetTenantName());
	metaInfoPtr->SetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_DESCRIPTION, tenantInfoPtr->GetTenantDescription());
	metaInfoPtr->SetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_OWNER_ID, tenantInfoPtr->GetOwnerId());

	return true;
}


// public methods of embedded class MetaInfo

QString CTenantMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace imtauth

