// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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

// reimplemented (imtauth::IUserRecentAction)

imtauth::IUserRecentAction::UserInfo CUserRecentAction::GetUserInfo() const
{
	return m_userInfo;
}


void CUserRecentAction::SetUserInfo(imtauth::IUserRecentAction::UserInfo userInfo)
{
	if (m_userInfo != userInfo){
		istd::CChangeNotifier changeNotifier(this);

		m_userInfo = userInfo;
	}
}


imtauth::IUserRecentAction::ActionTypeInfo CUserRecentAction::GetActionTypeInfo() const
{
	return m_actionTypeInfo;
}


void CUserRecentAction::SetActionTypeInfo(imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo)
{
	if (m_actionTypeInfo != actionTypeInfo){
		istd::CChangeNotifier changeNotifier(this);

		m_actionTypeInfo = actionTypeInfo;
	}
}


imtauth::IUserRecentAction::TargetInfo CUserRecentAction::GetTargetInfo() const
{
	return m_targetInfo;
}


void CUserRecentAction::SetTargetInfo(imtauth::IUserRecentAction::TargetInfo targetInfo)
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


const iprm::IParamsSet* CUserRecentAction::GetParams() const
{
	return m_paramsPtr.GetPtr();
}


void CUserRecentAction::SetParams(iprm::IParamsSet* paramsPtr)
{
	m_paramsPtr.SetPtr(paramsPtr);
}


// reimplemented (iser::ISerializable)

bool CUserRecentAction::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	UserInfo userInfo;
	if (archive.IsStoring()){
		userInfo = m_userInfo;
	}

	iser::CArchiveTag userIdTag("UserId", "User-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(userInfo.id);
	retVal = retVal && archive.EndTag(userIdTag);

	iser::CArchiveTag userNameTag("UserName", "User Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userNameTag);
	retVal = retVal && archive.Process(userInfo.name);
	retVal = retVal && archive.EndTag(userNameTag);

	if (retVal && !archive.IsStoring()){
		m_userInfo = userInfo;
	}

	ActionTypeInfo actionTypeInfo;
	if (archive.IsStoring()){
		actionTypeInfo = m_actionTypeInfo;
	}

	iser::CArchiveTag actionTypeIdTag("ActionTypeId", "Action Type-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(actionTypeIdTag);
	retVal = retVal && archive.Process(actionTypeInfo.id);
	retVal = retVal && archive.EndTag(actionTypeIdTag);

	iser::CArchiveTag actionTypeNameTag("ActionTypeName", "Action Type Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(actionTypeNameTag);
	retVal = retVal && archive.Process(actionTypeInfo.name);
	retVal = retVal && archive.EndTag(actionTypeNameTag);

	iser::CArchiveTag actionTypeDescriptionTag("ActionTypeDescription", "Action Type Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(actionTypeDescriptionTag);
	retVal = retVal && archive.Process(actionTypeInfo.description);
	retVal = retVal && archive.EndTag(actionTypeDescriptionTag);

	if (retVal && !archive.IsStoring()){
		m_actionTypeInfo = actionTypeInfo;
	}

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

	iser::CArchiveTag targetTypeNameTag("TargetTypeName", "Target Type Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetTypeNameTag);
	retVal = retVal && archive.Process(targetInfo.typeName);
	retVal = retVal && archive.EndTag(targetTypeNameTag);

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

		m_userInfo = sourcePtr->m_userInfo;
		m_actionTypeInfo = sourcePtr->m_actionTypeInfo;
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
		retVal = retVal && m_userInfo == sourcePtr->m_userInfo;
		retVal = retVal && m_actionTypeInfo == sourcePtr->m_actionTypeInfo;
		retVal = retVal && m_targetInfo == sourcePtr->m_targetInfo;
		retVal = retVal && m_timestamp == sourcePtr->m_timestamp;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CUserRecentAction::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CUserRecentAction);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CUserRecentAction::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_userInfo.id.clear();
	m_userInfo.name.clear();
	m_targetInfo.id.clear();
	m_targetInfo.name.clear();
	m_targetInfo.typeId.clear();
	m_targetInfo.typeName.clear();
	m_targetInfo.source.clear();
	m_actionTypeInfo.id.clear();
	m_actionTypeInfo.name.clear();
	m_actionTypeInfo.description.clear();

	return true;
}


} // namespace imtauth


