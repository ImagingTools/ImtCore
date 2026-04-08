// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesk/CTicketActionComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtdesk
{


// reimplemented (imtdesk::ITicketAction)

QByteArray CTicketActionComp::GetId() const
{
	return m_id;
}


void CTicketActionComp::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
	}
}


QByteArray CTicketActionComp::GetTicketId() const
{
	return m_ticketId;
}


void CTicketActionComp::SetTicketId(const QByteArray& ticketId)
{
	if (m_ticketId != ticketId){
		istd::CChangeNotifier notifier(this);

		m_ticketId = ticketId;
	}
}


QByteArray CTicketActionComp::GetUserId() const
{
	return m_userId;
}


void CTicketActionComp::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier notifier(this);

		m_userId = userId;
	}
}


QString CTicketActionComp::GetUserName() const
{
	return m_userName;
}


void CTicketActionComp::SetUserName(const QString& userName)
{
	if (m_userName != userName){
		istd::CChangeNotifier notifier(this);

		m_userName = userName;
	}
}


QByteArray CTicketActionComp::GetActionTypeId() const
{
	return m_actionTypeId;
}


void CTicketActionComp::SetActionTypeId(const QByteArray& actionTypeId)
{
	if (m_actionTypeId != actionTypeId){
		istd::CChangeNotifier notifier(this);

		m_actionTypeId = actionTypeId;
	}
}


QString CTicketActionComp::GetActionTypeName() const
{
	return m_actionTypeName;
}


void CTicketActionComp::SetActionTypeName(const QString& actionTypeName)
{
	if (m_actionTypeName != actionTypeName){
		istd::CChangeNotifier notifier(this);

		m_actionTypeName = actionTypeName;
	}
}


QString CTicketActionComp::GetActionTypeDescription() const
{
	return m_actionTypeDescription;
}


void CTicketActionComp::SetActionTypeDescription(const QString& actionTypeDescription)
{
	if (m_actionTypeDescription != actionTypeDescription){
		istd::CChangeNotifier notifier(this);

		m_actionTypeDescription = actionTypeDescription;
	}
}


QString CTicketActionComp::GetTimestamp() const
{
	return m_timestamp;
}


void CTicketActionComp::SetTimestamp(const QString& timestamp)
{
	if (m_timestamp != timestamp){
		istd::CChangeNotifier notifier(this);

		m_timestamp = timestamp;
	}
}


QString CTicketActionComp::GetActionData() const
{
	return m_actionData;
}


void CTicketActionComp::SetActionData(const QString& actionData)
{
	if (m_actionData != actionData){
		istd::CChangeNotifier notifier(this);

		m_actionData = actionData;
	}
}


// reimplemented (iser::ISerializable)

bool CTicketActionComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag ticketIdTag("TicketId", "Ticket ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(ticketIdTag);
	retVal = retVal && archive.Process(m_ticketId);
	retVal = retVal && archive.EndTag(ticketIdTag);

	static iser::CArchiveTag userIdTag("UserId", "User ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	static iser::CArchiveTag userNameTag("UserName", "User name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userNameTag);
	retVal = retVal && archive.Process(m_userName);
	retVal = retVal && archive.EndTag(userNameTag);

	static iser::CArchiveTag actionTypeIdTag("ActionTypeId", "Action type ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(actionTypeIdTag);
	retVal = retVal && archive.Process(m_actionTypeId);
	retVal = retVal && archive.EndTag(actionTypeIdTag);

	static iser::CArchiveTag actionTypeNameTag("ActionTypeName", "Action type name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(actionTypeNameTag);
	retVal = retVal && archive.Process(m_actionTypeName);
	retVal = retVal && archive.EndTag(actionTypeNameTag);

	static iser::CArchiveTag actionTypeDescTag("ActionTypeDescription", "Action type description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(actionTypeDescTag);
	retVal = retVal && archive.Process(m_actionTypeDescription);
	retVal = retVal && archive.EndTag(actionTypeDescTag);

	static iser::CArchiveTag timestampTag("Timestamp", "Timestamp", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(timestampTag);
	retVal = retVal && archive.Process(m_timestamp);
	retVal = retVal && archive.EndTag(timestampTag);

	static iser::CArchiveTag actionDataTag("ActionData", "Action data", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(actionDataTag);
	retVal = retVal && archive.Process(m_actionData);
	retVal = retVal && archive.EndTag(actionDataTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CTicketActionComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const ITicketAction* srcPtr = dynamic_cast<const ITicketAction*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_id = srcPtr->GetId();
	m_ticketId = srcPtr->GetTicketId();
	m_userId = srcPtr->GetUserId();
	m_userName = srcPtr->GetUserName();
	m_actionTypeId = srcPtr->GetActionTypeId();
	m_actionTypeName = srcPtr->GetActionTypeName();
	m_actionTypeDescription = srcPtr->GetActionTypeDescription();
	m_timestamp = srcPtr->GetTimestamp();
	m_actionData = srcPtr->GetActionData();

	return true;
}


bool CTicketActionComp::IsEqual(const IChangeable& object) const
{
	const ITicketAction* srcPtr = dynamic_cast<const ITicketAction*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	return m_id == srcPtr->GetId()
		&& m_ticketId == srcPtr->GetTicketId()
		&& m_userId == srcPtr->GetUserId()
		&& m_userName == srcPtr->GetUserName()
		&& m_actionTypeId == srcPtr->GetActionTypeId()
		&& m_actionTypeName == srcPtr->GetActionTypeName()
		&& m_actionTypeDescription == srcPtr->GetActionTypeDescription()
		&& m_timestamp == srcPtr->GetTimestamp()
		&& m_actionData == srcPtr->GetActionData();
}


istd::IChangeableUniquePtr CTicketActionComp::CloneMe(CompatibilityMode mode) const
{
	istd::TUniqueInterfacePtr<CTicketActionComp> clonePtr(new CTicketActionComp());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTicketActionComp::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_id.clear();
	m_ticketId.clear();
	m_userId.clear();
	m_userName.clear();
	m_actionTypeId.clear();
	m_actionTypeName.clear();
	m_actionTypeDescription.clear();
	m_timestamp.clear();
	m_actionData.clear();

	return true;
}


} // namespace imtdesk
