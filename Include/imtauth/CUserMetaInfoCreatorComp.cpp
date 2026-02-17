// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/IUserInfo.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CUserMetaInfoCreatorComp::CreateMetaInfo(
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

	const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr);
	if (userInfoPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(imtauth::IUserInfo::MIT_ID, userInfoPtr->GetId());
	metaInfoPtr->SetMetaInfo(imtauth::IUserInfo::MIT_NAME, userInfoPtr->GetName());
	metaInfoPtr->SetMetaInfo(imtauth::IUserInfo::MIT_DESCRIPTION, userInfoPtr->GetDescription());
	metaInfoPtr->SetMetaInfo(imtauth::IUserInfo::MIT_EMAIL, userInfoPtr->GetMail());
	metaInfoPtr->SetMetaInfo(imtauth::IUserInfo::MIT_PASSWORD_HASH, userInfoPtr->GetPasswordHash());
	metaInfoPtr->SetMetaInfo(imtauth::IUserInfo::MIT_SYSTEM_ID, "");
	metaInfoPtr->SetMetaInfo(imtauth::IUserInfo::MIT_SYSTEM_NAME, "");

	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		if (systemInfo.enabled){
			metaInfoPtr->SetMetaInfo(imtauth::IUserInfo::MIT_SYSTEM_ID, systemInfo.systemId);
			metaInfoPtr->SetMetaInfo(imtauth::IUserInfo::MIT_SYSTEM_NAME, systemInfo.systemName);

			break;
		}
	}

	return true;
}


// public methods of embedded class MetaInfo

QString CUserMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace imtauth


