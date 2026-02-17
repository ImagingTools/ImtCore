// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CSessionMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/ISession.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CSessionMetaInfoCreatorComp::CreateMetaInfo(
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

	const ISession* sessionPtr = dynamic_cast<const ISession*>(dataPtr);
	if (sessionPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(ISession::MIT_TOKEN, sessionPtr->GetToken());
	metaInfoPtr->SetMetaInfo(ISession::MIT_USER_ID, sessionPtr->GetUserId());

	return true;
}


// public methods of embedded class MetaInfo

QString CSessionMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case ISession::MIT_TOKEN:
		return QObject::tr("Token");
	case ISession::MIT_USER_ID:
		return QObject::tr("User-ID");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}



} // namespace imtauth


