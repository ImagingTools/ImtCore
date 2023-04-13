#include <imtauth/CUserGroupInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods


CUserGroupInfo::CUserGroupInfo():
	m_userGroupInfoProviderPtr(nullptr)
{

}


// reimplemented (IUserGroupInfo)

IUserGroupInfo::UserIds CUserGroupInfo::GetUsers() const
{
	return m_userIds;
}


void CUserGroupInfo::SetUsers(const UserIds& users)
{
	if (m_userIds != users){
		istd::CChangeNotifier changeNotifier(this);

		m_userIds = users;
	}
}


void CUserGroupInfo::AddUser(const QByteArray& userId)
{
	if (!m_userIds.contains(userId)){
		istd::CChangeNotifier changeNotifier(this);

		m_userIds << userId;
	}
}


bool CUserGroupInfo::RemoveUser(const QByteArray& userId)
{
	bool result = m_userIds.removeAll(userId);
	if (result){
		istd::CChangeNotifier changeNotifier(this);
	}

	return result;
}


imtauth::IUserGroupInfo::GroupIds CUserGroupInfo::GetParentGroups() const
{
	return m_parentGroupIds;
}


void CUserGroupInfo::AddParentGroup(const QByteArray& parentGroupId)
{
	if (!m_parentGroupIds.contains(parentGroupId)){
		istd::CChangeNotifier changeNotifier(this);

		m_parentGroupIds << parentGroupId;
	}
}


bool CUserGroupInfo::RemoveParentGroup(const QByteArray& parentGroupId)
{
	bool result = m_parentGroupIds.removeAll(parentGroupId);
	if (result){
		istd::CChangeNotifier changeNotifier(this);
	}

	return result;
}


const imtauth::IUserInfoProvider* CUserGroupInfo::GetUserProvider() const
{
	return nullptr;
}


const imtauth::IUserGroupInfoProvider* CUserGroupInfo::GetUserGroupProvider() const
{
	return m_userGroupInfoProviderPtr;
}


bool CUserGroupInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_userIds, "Users", "User");
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_parentGroupIds, "ParentGroups", "ParentGroup");

	return retVal;
}


bool CUserGroupInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CUserGroupInfo* sourcePtr = dynamic_cast<const CUserGroupInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		BaseClass::CopyFrom(object);

		m_userIds = sourcePtr->m_userIds;
		m_parentGroupIds = sourcePtr->m_parentGroupIds;

		return true;
	}

	return false;
}


istd::IChangeable *CUserGroupInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CUserGroupInfo> clonePtr(new CUserGroupInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CUserGroupInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	BaseClass::ResetData(mode);

	m_userIds.clear();
	m_parentGroupIds.clear();
	m_userGroupInfoProviderPtr = nullptr;

	return true;
}


} // namespace imtauth


