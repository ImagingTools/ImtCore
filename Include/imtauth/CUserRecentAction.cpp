#include <imtauth/CUserRecentAction.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

CUserRecentAction::CUserRecentAction()
	:m_actionType(AT_UNKNOWN)
{
}


// reimplemented (imtauth::IUserRecentAction)

QByteArray CUserRecentAction::GetUserId() const
{
	return m_userId;
}


void CUserRecentAction::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}



imtauth::IUserRecentAction::ActionType CUserRecentAction::GetActionType() const
{
	return m_actionType;
}


void CUserRecentAction::SetActionType(ActionType actionType)
{
	if (m_actionType != actionType){
		istd::CChangeNotifier changeNotifier(this);

		m_actionType = actionType;
	}
}


imtauth::IUserRecentAction::TargetInfo CUserRecentAction::GetTargetInfo() const
{
	return m_targetInfo;
}


void CUserRecentAction::SetTargetInfo(TargetInfo targetInfo)
{
	if (m_targetInfo != targetInfo){
		istd::CChangeNotifier changeNotifier(this);

		m_targetInfo = targetInfo;
	}
}


QDateTime CUserRecentAction::GetTimestamp() const
{
	return m_timestamp;
}


void CUserRecentAction::SetTimestamp(const QDateTime& timestamp)
{
	if (m_timestamp != timestamp){
		istd::CChangeNotifier changeNotifier(this);

		m_timestamp = timestamp;
	}
}


// reimplemented (iser::ISerializable)

bool CUserRecentAction::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag userIdTag("UserId", "User-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	iser::CArchiveTag actionTypeTag("ActionType", "Action Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(actionTypeTag);
	retVal = retVal && I_SERIALIZE_ENUM(ActionType, archive, m_actionType);
	retVal = retVal && archive.EndTag(actionTypeTag);

	TargetInfo targetInfo;
	if (archive.IsStoring()){
		targetInfo = m_targetInfo;
	}

	iser::CArchiveTag targetIdTag("TargetId", "Target-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetIdTag);
	retVal = retVal && archive.Process(targetInfo.id);
	retVal = retVal && archive.EndTag(targetIdTag);

	iser::CArchiveTag targetNameTag("TargetName", "Target Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetNameTag);
	retVal = retVal && archive.Process(targetInfo.name);
	retVal = retVal && archive.EndTag(targetNameTag);

	iser::CArchiveTag targetTypeIdTag("TargetTypeId", "Target Type-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetTypeIdTag);
	retVal = retVal && archive.Process(targetInfo.typeId);
	retVal = retVal && archive.EndTag(targetTypeIdTag);

	iser::CArchiveTag targetSourceTag("TargetSource", "Target Source", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetSourceTag);
	retVal = retVal && archive.Process(targetInfo.source);
	retVal = retVal && archive.EndTag(targetSourceTag);

	iser::CArchiveTag timestampTag("Timestamp", "Timestamp", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(timestampTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_timestamp);
	retVal = retVal && archive.EndTag(timestampTag);

	if (retVal && !archive.IsStoring()){
		m_targetInfo = targetInfo;
	}

	return retVal;
}


// reimplemented (iser::IChangeable)

bool CUserRecentAction::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CUserRecentAction* sourcePtr = dynamic_cast<const CUserRecentAction*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = sourcePtr->m_userId;
		m_actionType = sourcePtr->m_actionType;
		m_targetInfo = sourcePtr->m_targetInfo;
		m_timestamp = sourcePtr->m_timestamp;

		return true;
	}

	return false;
}


bool CUserRecentAction::IsEqual(const IChangeable& object) const
{
	bool retVal = true;
	const CUserRecentAction* sourcePtr = dynamic_cast<const CUserRecentAction*>(&object);
	if (retVal && sourcePtr != nullptr){
		retVal = retVal && m_userId == sourcePtr->m_userId;
		retVal = retVal && m_actionType == sourcePtr->m_actionType;
		retVal = retVal && m_targetInfo == sourcePtr->m_targetInfo;
		retVal = retVal && m_timestamp == sourcePtr->m_timestamp;

		return retVal;
	}

	return false;
}


istd::IChangeable *CUserRecentAction::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CUserRecentAction> clonePtr(new CUserRecentAction);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CUserRecentAction::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_userId.clear();
	m_targetInfo.id.clear();
	m_targetInfo.name.clear();
	m_targetInfo.typeId.clear();
	m_targetInfo.source.clear();
	m_actionType = AT_UNKNOWN;

	return true;
}


} // namespace imtauth


