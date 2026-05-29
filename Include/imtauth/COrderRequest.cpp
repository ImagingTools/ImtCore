// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/COrderRequest.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


COrderRequest::COrderRequest()
{
	m_info.status = ORS_RECEIVED;
	m_info.quantity = 0;
}


OrderRequestInfo COrderRequest::GetOrderRequestInfo() const
{
	return m_info;
}


void COrderRequest::SetOrderRequestInfo(const OrderRequestInfo& orderRequestInfo)
{
	if (m_info != orderRequestInfo){
		istd::CChangeNotifier notifier(this);
		m_info = orderRequestInfo;
	}
}


QByteArray COrderRequest::GetOrderRequestId() const
{
	return m_info.orderRequestId;
}


bool COrderRequest::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_info.orderRequestId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag messageTag("MessageId", "Message ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(messageTag);
	retVal = retVal && archive.Process(m_info.messageId);
	retVal = retVal && archive.EndTag(messageTag);

	iser::CArchiveTag sourceTag("SourceTenantId", "Source tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceTag);
	retVal = retVal && archive.Process(m_info.sourceTenantId);
	retVal = retVal && archive.EndTag(sourceTag);

	iser::CArchiveTag targetTag("TargetTenantId", "Target tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetTag);
	retVal = retVal && archive.Process(m_info.targetTenantId);
	retVal = retVal && archive.EndTag(targetTag);

	iser::CArchiveTag relationshipTag("RelationshipId", "Relationship ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(relationshipTag);
	retVal = retVal && archive.Process(m_info.relationshipId);
	retVal = retVal && archive.EndTag(relationshipTag);

	iser::CArchiveTag sourceOrderTag("SourceOrderId", "Source order ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceOrderTag);
	retVal = retVal && archive.Process(m_info.sourceOrderId);
	retVal = retVal && archive.EndTag(sourceOrderTag);

	iser::CArchiveTag articleTag("ArticleNumber", "Article number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(articleTag);
	retVal = retVal && archive.Process(m_info.articleNumber);
	retVal = retVal && archive.EndTag(articleTag);

	iser::CArchiveTag quantityTag("Quantity", "Quantity", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(quantityTag);
	retVal = retVal && archive.Process(m_info.quantity);
	retVal = retVal && archive.EndTag(quantityTag);

	iser::CArchiveTag noteTag("Note", "Note", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(noteTag);
	retVal = retVal && archive.Process(m_info.note);
	retVal = retVal && archive.EndTag(noteTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	int status = static_cast<int>(m_info.status);
	retVal = retVal && archive.Process(status);
	m_info.status = static_cast<OrderRequestStatus>(status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag statusNoteTag("StatusNote", "Status note", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusNoteTag);
	retVal = retVal && archive.Process(m_info.statusNote);
	retVal = retVal && archive.EndTag(statusNoteTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_info.createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_info.updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	return retVal;
}


bool COrderRequest::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const COrderRequest* sourcePtr = dynamic_cast<const COrderRequest*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_info = sourcePtr->m_info;

	return true;
}


istd::IChangeableUniquePtr COrderRequest::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new COrderRequest);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool COrderRequest::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_info = OrderRequestInfo();
	m_info.status = ORS_RECEIVED;
	m_info.quantity = 0;

	return true;
}


} // namespace imtauth
