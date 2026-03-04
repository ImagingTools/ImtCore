// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRoleMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/IRole.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CRoleMetaInfoCreatorComp::CreateMetaInfo(
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

	const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr);
	if (roleInfoPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(imtauth::IRole::MIT_ROLE_ID, roleInfoPtr->GetRoleId());
	metaInfoPtr->SetMetaInfo(imtauth::IRole::MIT_ROLE_NAME, roleInfoPtr->GetRoleName());
	metaInfoPtr->SetMetaInfo(imtauth::IRole::MIT_ROLE_DESCRIPTION, roleInfoPtr->GetRoleDescription());
	metaInfoPtr->SetMetaInfo(imtauth::IRole::MIT_PRODUCT_ID, roleInfoPtr->GetProductId());

	return true;
}


// public methods of embedded class MetaInfo

QString CRoleMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace imtauth


