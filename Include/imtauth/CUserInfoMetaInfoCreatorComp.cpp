// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserInfoMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/CUserInfoMetaInfo.h>
#include <imtauth/IUserInfo.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CUserInfoMetaInfoCreatorComp::CreateMetaInfo(
		const istd::IChangeable* dataPtr,
		const QByteArray& typeId,
		idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<CUserInfoMetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const IUserInfo* userPtr = dynamic_cast<const IUserInfo*>(dataPtr);
	if (userPtr == nullptr){
		return false;
	}

	QByteArray username = userPtr->GetId();
	QString email =  userPtr->GetMail();
//	metaInfoPtr->SetMetaInfo(IUserInfo::MetaInfoTypes::MIT_EMAIL, userPtr->GetMail());
//	metaInfoPtr->SetMetaInfo(IUserInfo::MetaInfoTypes::MIT_NAME, userPtr->GetName());
//	metaInfoPtr->SetMetaInfo(IUserInfo::MetaInfoTypes::MIT_USERNAME, userPtr->GetId());

	return true;
}


} // namespace imtauth


