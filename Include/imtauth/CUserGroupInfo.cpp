// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserGroupInfo.h>


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtcore/Version.h>
#include <imtauth/IUserGroupInfoProvider.h>


namespace imtauth
{


// public methods


CUserGroupInfo::CUserGroupInfo():
	m_userInfoProviderPtr(nullptr)
{

}


IUserGroupInfo::UserIds CUserGroupInfo::GetOwnUsers() const
{
	return m_userIds;
}


QRecursiveMutex& CUserGroupInfo::GetUpdateMutex()
{
	static QRecursiveMutex s_updateMutex;

	return s_updateMutex;
}


// reimplemented (IUserGroupInfo)

IUserGroupInfo::UserIds CUserGroupInfo::GetUsers() const
{
	QByteArrayList retVal = m_userIds;

	if (m_userGroupInfoProviderPtr != nullptr){
		// Only the own users of each ancestor: GetUsers() of a parent would recurse forever on a cycle.
		for (const QByteArray& parentGroupId : GetAncestorGroups(m_parentGroupIds)){
			imtauth::IUserGroupInfoSharedPtr parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(parentGroupId);
			const CUserGroupInfo* parentInfoPtr = dynamic_cast<const CUserGroupInfo*>(parentGroupPtr.GetPtr());
			if (parentInfoPtr != nullptr){
				for (const QByteArray& userId : parentInfoPtr->m_userIds){
					if (!retVal.contains(userId)){
						retVal << userId;
					}
				}
			}
		}
	}

	return retVal;
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
	qsizetype removedCount = m_userIds.removeAll(userId);
	if (removedCount != 0){
		istd::CChangeNotifier changeNotifier(this);
	}

	return removedCount != 0;
}


imtauth::IUserGroupInfo::GroupIds CUserGroupInfo::GetParentGroups() const
{
	return m_parentGroupIds;
}


bool CUserGroupInfo::AddParentGroup(const QByteArray& parentGroupId)
{
	if (m_parentGroupIds.contains(parentGroupId)){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_parentGroupIds.append(parentGroupId);

	return true;
}


bool CUserGroupInfo::RemoveParentGroup(const QByteArray& parentGroupId)
{
	qsizetype removedCount = m_parentGroupIds.removeAll(parentGroupId);
	if (removedCount != 0){
		istd::CChangeNotifier changeNotifier(this);
	}

	return removedCount != 0;
}


const imtauth::IUserInfoProvider* CUserGroupInfo::GetUserProvider() const
{
	return m_userInfoProviderPtr;
}


IUserBaseInfo::SystemInfoList CUserGroupInfo::GetSystemInfos() const
{
	return m_systemInfos;
}


bool CUserGroupInfo::AddToSystem(SystemInfo systemInfo)
{
	if (m_systemInfos.contains(systemInfo)){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_systemInfos << systemInfo;

	return true;
}


bool CUserGroupInfo::RemoveFromSystem(const QByteArray& systemId)
{
	for (qsizetype index = 0; index < m_systemInfos.size(); ++index){
		if (m_systemInfos[index].systemId == systemId){
			istd::CChangeNotifier changeNotifier(this);

			m_systemInfos.removeAt(index);

			return true;
		}
	}

	return false;
}


// reimplemented (IUserBaseInfo)

IUserBaseInfo::FeatureIds CUserGroupInfo::GetPermissions(const QByteArray& productId) const
{
	IUserBaseInfo::FeatureIds allPermissions = BaseClass::GetPermissions(productId);

	if (m_userGroupInfoProviderPtr != nullptr){
		// Walked iteratively with a visited set: a cycle in the parent groups must not recurse forever.
		QByteArrayList pendingGroupIds = m_parentGroupIds;
		QSet<QByteArray> visitedGroupIds;
		while (!pendingGroupIds.isEmpty()){
			const QByteArray groupId = pendingGroupIds.takeFirst();
			if (visitedGroupIds.contains(groupId)){
				continue;
			}
			visitedGroupIds.insert(groupId);

			IUserGroupInfoSharedPtr parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(groupId);
			if (!parentGroupPtr.IsValid()){
				continue;
			}

			// Only the group's own roles here, its parents are walked by this loop.
			const CUserBaseInfo* parentBasePtr = dynamic_cast<const CUserBaseInfo*>(parentGroupPtr.GetPtr());
			const FeatureIds parentPermissions = (parentBasePtr != nullptr)
						? parentBasePtr->CUserBaseInfo::GetPermissions(productId)
						: parentGroupPtr->GetPermissions(productId);
			for (const QByteArray& permissionId : parentPermissions){
				if (!allPermissions.contains(permissionId)){
					allPermissions << permissionId;
				}
			}

			pendingGroupIds += parentGroupPtr->GetParentGroups();
		}
	}

	return allPermissions;
}


bool CUserGroupInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_userIds, "Users", "User");
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_parentGroupIds, "ParentGroups", "ParentGroup");

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();
	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	if (imtCoreVersion >= 23950){
		int count = m_systemInfos.size();

		if (!archive.IsStoring()){
			m_systemInfos.clear();
			count = 0;
		}

		iser::CArchiveTag infosTag("SystemInfos", "System infos", iser::CArchiveTag::TT_MULTIPLE);
		iser::CArchiveTag infoTag("SystemInfo", "System info", iser::CArchiveTag::TT_GROUP, &infosTag);

		retVal = retVal && archive.BeginMultiTag(infosTag, infoTag, count);
		for (int index = 0; index < count; index++){
			retVal = retVal && archive.BeginTag(infoTag);

			SystemInfo systemInfo;

			if (archive.IsStoring()){
				systemInfo = m_systemInfos[index];
			}

			iser::CArchiveTag systemIdTag("SystemId", "System-ID", iser::CArchiveTag::TT_LEAF);
			retVal = retVal && archive.BeginTag(systemIdTag);
			retVal = retVal && archive.Process(systemInfo.systemId);
			retVal = retVal && archive.EndTag(systemIdTag);

			iser::CArchiveTag systemNameTag("SystemName", "System Name", iser::CArchiveTag::TT_LEAF);
			retVal = retVal && archive.BeginTag(systemNameTag);
			retVal = retVal && archive.Process(systemInfo.systemName);
			retVal = retVal && archive.EndTag(systemNameTag);

			iser::CArchiveTag systemEnabledTag("Enabled", "System enabled", iser::CArchiveTag::TT_LEAF);
			retVal = retVal && archive.BeginTag(systemEnabledTag);
			retVal = retVal && archive.Process(systemInfo.enabled);
			retVal = retVal && archive.EndTag(systemEnabledTag);

			retVal = retVal && archive.EndTag(infoTag);

			if (retVal && !archive.IsStoring()){
				m_systemInfos << systemInfo;
			}
		}

		retVal = retVal && archive.EndTag(infosTag);
	}
	else if (!archive.IsStoring()){
		// Groups stored before the system flag existed are internal groups.
		m_systemInfos.clear();
		m_systemInfos << SystemInfo();
	}

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
		m_systemInfos = sourcePtr->m_systemInfos;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CUserGroupInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CUserGroupInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CUserGroupInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	BaseClass::ResetData(mode);

	m_userIds.clear();
	m_parentGroupIds.clear();
	m_systemInfos.clear();
	m_userGroupInfoProviderPtr = nullptr;
	m_userInfoProviderPtr = nullptr;

	return true;
}


// protected methods

QByteArrayList CUserGroupInfo::GetAncestorGroups(const QByteArrayList& groupIds) const
{
	QByteArrayList retVal;

	// Walked iteratively with a visited set: a cycle in the parent groups must not recurse forever.
	QByteArrayList pendingGroupIds = groupIds;
	while (!pendingGroupIds.isEmpty()){
		const QByteArray currentGroupId = pendingGroupIds.takeFirst();
		if (retVal.contains(currentGroupId)){
			continue;
		}
		retVal << currentGroupId;

		if (m_userGroupInfoProviderPtr != nullptr){
			IUserGroupInfoSharedPtr groupPtr(m_userGroupInfoProviderPtr->GetUserGroup(currentGroupId));
			if (groupPtr.IsValid()){
				pendingGroupIds += groupPtr->GetParentGroups();
			}
		}
	}

	return retVal;
}


} // namespace imtauth


