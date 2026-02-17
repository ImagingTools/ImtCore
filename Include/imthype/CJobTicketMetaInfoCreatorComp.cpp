// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CJobTicketMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// Acula includes
#include <imthype/CJobTicket.h>


namespace imthype
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CJobTicketMetaInfoCreatorComp::CreateMetaInfo(
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

	const imthype::CJobTicket* jobTicketInfoPtr = dynamic_cast<const imthype::CJobTicket*>(dataPtr);
	if (jobTicketInfoPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(imthype::IJobTicket::MIT_UUID, jobTicketInfoPtr->GetUuid());
	metaInfoPtr->SetMetaInfo(imthype::IJobTicket::MIT_JOB_NAME, jobTicketInfoPtr->GetJobName());
	metaInfoPtr->SetMetaInfo(imthype::IJobTicket::MIT_CONTEXT_ID, jobTicketInfoPtr->GetContextId());
	metaInfoPtr->SetMetaInfo(imthype::IJobTicket::MIT_PROGRESS, jobTicketInfoPtr->GetProgress());
	metaInfoPtr->SetMetaInfo(imthype::IJobTicket::MIT_PROCESSING_STATUS, jobTicketInfoPtr->GetProcessingStatus());

	return true;
}


// public methods of embedded class MetaInfo

idoc::IDocumentMetaInfo::MetaInfoTypes CJobTicketMetaInfoCreatorComp::MetaInfo::GetMetaInfoTypes(bool /*allowReadOnly*/) const
{
	return {
		imthype::IJobTicket::MIT_UUID,
		imthype::IJobTicket::MIT_JOB_NAME,
		imthype::IJobTicket::MIT_CONTEXT_ID,
		imthype::IJobTicket::MIT_PROGRESS,
		imthype::IJobTicket::MIT_PROCESSING_STATUS
	};
}


QByteArray CJobTicketMetaInfoCreatorComp::MetaInfo::GetMetaInfoId(int metaInfoType) const
{
	return BaseClass::GetMetaInfoId(metaInfoType);
}


QString CJobTicketMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imthype


