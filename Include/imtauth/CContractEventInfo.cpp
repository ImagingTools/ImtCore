// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractEventInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/ArchiveFields.h>


namespace imtauth
{


namespace
{


namespace af = ArchiveFields;


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

	bool retVal = af::Leaf(archive, "EventId", m_eventId);
	retVal = retVal && af::Leaf(archive, "ContractId", m_contractId);
	retVal = retVal && af::EnumLeaf(archive, "Type", m_eventType);
	retVal = retVal && af::Leaf(archive, "TenantId", m_tenantId);
	retVal = retVal && af::Leaf(archive, "UserId", m_userId);
	retVal = retVal && af::Leaf(archive, "UserName", m_userName);
	retVal = retVal && af::Leaf(archive, "Time", m_time);
	retVal = retVal && af::Leaf(archive, "Revision", m_revision);
	retVal = retVal && af::Leaf(archive, "Comment", m_comment);
	retVal = retVal && af::Leaf(archive, "SubjectId", m_subjectId);
	retVal = retVal && af::Leaf(archive, "IsPrivate", m_private);

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


