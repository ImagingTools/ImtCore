// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CGroupMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/IUserGroupInfo.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CGroupMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr);
	if (userGroupInfoPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(imtauth::IUserGroupInfo::MIT_ID, userGroupInfoPtr->GetId());
	metaInfoPtr->SetMetaInfo(imtauth::IUserGroupInfo::MIT_NAME, userGroupInfoPtr->GetName());
	metaInfoPtr->SetMetaInfo(imtauth::IUserGroupInfo::MIT_DESCRIPTION, userGroupInfoPtr->GetDescription());

	return true;
}


// public methods of embedded class MetaInfo

QString CGroupMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace imtauth


