// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CPersonalAccessTokenMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/IPersonalAccessToken.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CPersonalAccessTokenMetaInfoCreatorComp::CreateMetaInfo(
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

	const imtauth::IPersonalAccessToken* tokenPtr = dynamic_cast<const imtauth::IPersonalAccessToken*>(dataPtr);
	if (tokenPtr == nullptr){
		return false;
	}

	return true;
}


QString CPersonalAccessTokenMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtauth
