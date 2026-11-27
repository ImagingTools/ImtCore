// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractEventInfo.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <istd/CChangeNotifier.h>


namespace imtauth
{


namespace
{


template <typename Value>
void AssignValue(istd::IChangeable& object, Value& member, const Value& value)
{
	if (member != value){
		istd::CChangeNotifier notifier(&object);

		member = value;
	}
}


} // anonymous namespace


// public methods

CContractEventInfo::CContractEventInfo():
	m_eventType(ET_UPDATED),
	m_revision(0),
	m_private(false)
{
}


// reimplemented (imtauth::IContractEventInfo)

QByteArray CContractEventInfo::GetEventId() const
{
	return m_eventId;
}


void CContractEventInfo::SetEventId(const QByteArray& value)
{
	AssignValue(*this, m_eventId, value);
}


QByteArray CContractEventInfo::GetContractId() const
{
	return m_contractId;
}


void CContractEventInfo::SetContractId(const QByteArray& value)
{
	AssignValue(*this, m_contractId, value);
}


IContractEventInfo::EventType CContractEventInfo::GetEventType() const
{
	return m_eventType;
}


void CContractEventInfo::SetEventType(IContractEventInfo::EventType value)
{
	AssignValue(*this, m_eventType, value);
}


QByteArray CContractEventInfo::GetTenantId() const
{
	return m_tenantId;
}


void CContractEventInfo::SetTenantId(const QByteArray& value)
{
	AssignValue(*this, m_tenantId, value);
}


QByteArray CContractEventInfo::GetUserId() const
{
	return m_userId;
}


void CContractEventInfo::SetUserId(const QByteArray& value)
{
	AssignValue(*this, m_userId, value);
}


QString CContractEventInfo::GetUserName() const
{
	return m_userName;
}


void CContractEventInfo::SetUserName(const QString& value)
{
	AssignValue(*this, m_userName, value);
}


QDateTime CContractEventInfo::GetTime() const
{
	return m_time;
}


void CContractEventInfo::SetTime(const QDateTime& value)
{
	AssignValue(*this, m_time, value);
}


int CContractEventInfo::GetRevision() const
{
	return m_revision;
}


void CContractEventInfo::SetRevision(int value)
{
	AssignValue(*this, m_revision, value);
}


QString CContractEventInfo::GetComment() const
{
	return m_comment;
}


void CContractEventInfo::SetComment(const QString& value)
{
	AssignValue(*this, m_comment, value);
}


QByteArray CContractEventInfo::GetSubjectId() const
{
	return m_subjectId;
}


void CContractEventInfo::SetSubjectId(const QByteArray& value)
{
	AssignValue(*this, m_subjectId, value);
}


bool CContractEventInfo::IsPrivate() const
{
	return m_private;
}


void CContractEventInfo::SetPrivate(bool value)
{
	AssignValue(*this, m_private, value);
}


// reimplemented (iser::ISerializable)

bool CContractEventInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag eventIdTag("EventId", "Event ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(eventIdTag);
	retVal = retVal && archive.Process(m_eventId);
	retVal = retVal && archive.EndTag(eventIdTag);

	iser::CArchiveTag contractIdTag("ContractId", "Contract ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contractIdTag);
	retVal = retVal && archive.Process(m_contractId);
	retVal = retVal && archive.EndTag(contractIdTag);

	iser::CArchiveTag typeTag("Type", "Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<EventType, ToString, FromString>(archive, m_eventType);
	retVal = retVal && archive.EndTag(typeTag);

	iser::CArchiveTag tenantIdTag("TenantId", "Tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantIdTag);
	retVal = retVal && archive.Process(m_tenantId);
	retVal = retVal && archive.EndTag(tenantIdTag);

	iser::CArchiveTag userIdTag("UserId", "User ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	iser::CArchiveTag userNameTag("UserName", "User name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userNameTag);
	retVal = retVal && archive.Process(m_userName);
	retVal = retVal && archive.EndTag(userNameTag);

	iser::CArchiveTag timeTag("Time", "Time", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(timeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_time);
	retVal = retVal && archive.EndTag(timeTag);

	iser::CArchiveTag revisionTag("Revision", "Revision", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(revisionTag);
	retVal = retVal && archive.Process(m_revision);
	retVal = retVal && archive.EndTag(revisionTag);

	iser::CArchiveTag commentTag("Comment", "Comment", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(commentTag);
	retVal = retVal && archive.Process(m_comment);
	retVal = retVal && archive.EndTag(commentTag);

	iser::CArchiveTag subjectIdTag("SubjectId", "Subject ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(subjectIdTag);
	retVal = retVal && archive.Process(m_subjectId);
	retVal = retVal && archive.EndTag(subjectIdTag);

	iser::CArchiveTag isPrivateTag("IsPrivate", "Is private", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isPrivateTag);
	retVal = retVal && archive.Process(m_private);
	retVal = retVal && archive.EndTag(isPrivateTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CContractEventInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IContractEventInfo* sourcePtr = dynamic_cast<const IContractEventInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_eventId = sourcePtr->GetEventId();
	m_contractId = sourcePtr->GetContractId();
	m_eventType = sourcePtr->GetEventType();
	m_tenantId = sourcePtr->GetTenantId();
	m_userId = sourcePtr->GetUserId();
	m_userName = sourcePtr->GetUserName();
	m_time = sourcePtr->GetTime();
	m_revision = sourcePtr->GetRevision();
	m_comment = sourcePtr->GetComment();
	m_subjectId = sourcePtr->GetSubjectId();
	m_private = sourcePtr->IsPrivate();

	return true;
}


bool CContractEventInfo::IsEqual(const IChangeable& object) const
{
	const IContractEventInfo* otherPtr = dynamic_cast<const IContractEventInfo*>(&object);
	if (otherPtr == nullptr){
		return false;
	}

	return m_eventId == otherPtr->GetEventId()
				&& m_contractId == otherPtr->GetContractId()
				&& m_eventType == otherPtr->GetEventType()
				&& m_tenantId == otherPtr->GetTenantId()
				&& m_userId == otherPtr->GetUserId()
				&& m_userName == otherPtr->GetUserName()
				&& m_time == otherPtr->GetTime()
				&& m_revision == otherPtr->GetRevision()
				&& m_comment == otherPtr->GetComment()
				&& m_subjectId == otherPtr->GetSubjectId()
				&& m_private == otherPtr->IsPrivate();
}


istd::IChangeableUniquePtr CContractEventInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CContractEventInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CContractEventInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_eventId = QByteArray();
	m_contractId = QByteArray();
	m_eventType = ET_UPDATED;
	m_tenantId = QByteArray();
	m_userId = QByteArray();
	m_userName = QString();
	m_time = QDateTime();
	m_revision = 0;
	m_comment = QString();
	m_subjectId = QByteArray();
	m_private = false;

	return true;
}


} // namespace imtauth


