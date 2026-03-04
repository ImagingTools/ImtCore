// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserInfo.h>


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

// reimplemented (iser::IUserInfo)

QByteArray CUserInfo::GetPasswordHash() const
{
	return m_passwordHash;
}


void CUserInfo::SetPasswordHash(const QByteArray& passwordHash)
{
	if (m_passwordHash != passwordHash){
		istd::CChangeNotifier changeNotifier(this);

		m_passwordHash = passwordHash;
	}
}


QString CUserInfo::GetMail() const
{
	return m_mail;
}


void CUserInfo::SetMail(const QString& mail)
{
	if (m_mail != mail){
		istd::CChangeNotifier changeNotifier(this);

		m_mail = mail;
	}
}


bool CUserInfo::IsAdmin() const
{
	return false;
}


IUserGroupInfo::GroupIds CUserInfo::GetGroups() const
{
	return m_groupIds;
}


void CUserInfo::AddToGroup(const QByteArray& groupId)
{
	if (!m_groupIds.contains(groupId)){
		istd::CChangeNotifier changeNotifier(this);

		m_groupIds << groupId;
	}
}


bool CUserInfo::RemoveFromGroup(const QByteArray& groupId)
{
	qsizetype removedCount = m_groupIds.removeAll(groupId);
	if (removedCount != 0){
		istd::CChangeNotifier changeNotifier(this);
	}

	return removedCount != 0;
}


IUserBaseInfo::RoleIds CUserInfo::GetRoles(const QByteArray& productId) const
{
	IUserBaseInfo::RoleIds retVal = BaseClass::GetRoles(productId);

	if (m_userGroupInfoProviderPtr != nullptr){
		for (const QByteArray& parentGroupId : m_groupIds){
			IUserGroupInfoSharedPtr parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(parentGroupId);
			if (parentGroupPtr.IsValid()){
				QByteArrayList groupRoleIds = parentGroupPtr->GetRoles(productId);
				for (const QByteArray& roleId : groupRoleIds){
					if (!retVal.contains(roleId)){
						retVal << roleId;
					}
				}
			}
		}
	}

	return retVal;
}


IUserBaseInfo::FeatureIds CUserInfo::GetPermissions(const QByteArray& productId) const
{
	IUserBaseInfo::FeatureIds allPermissions = BaseClass::GetPermissions(productId);

	if (m_userGroupInfoProviderPtr != nullptr){
		for (const QByteArray& parentGroupId : m_groupIds){
			IUserGroupInfoSharedPtr parentGroupPtr = m_userGroupInfoProviderPtr->GetUserGroup(parentGroupId);
			if (parentGroupPtr.IsValid()){
				QByteArrayList groupPermissions = parentGroupPtr->GetPermissions(productId);
				for (const QByteArray& perm : std::as_const(groupPermissions)){
					if (!allPermissions.contains(perm)){
						allPermissions << perm;
					}
				}
			}
		}
	}

	return allPermissions;
}


IUserInfo::SystemInfoList CUserInfo::GetSystemInfos() const
{
	return m_systemInfos;
}


bool CUserInfo::AddToSystem(SystemInfo systemInfo)
{
	if (!m_systemInfos.contains(systemInfo)){
		istd::CChangeNotifier changeNotifier(this);

		m_systemInfos << systemInfo;

		return true;
	}

	return false;
}


bool CUserInfo::RemoveFromSystem(const QByteArray& systemId)
{
	for (const SystemInfo& systemInfo : m_systemInfos){
		if (systemInfo.systemId == systemId){
			istd::CChangeNotifier changeNotifier(this);
			m_systemInfos.removeOne(systemInfo);

			return true;
		}
	}

	return false;
}


// reimplemented (iser::ISerializable)

bool CUserInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();
	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	bool retVal = true;

	retVal = retVal && BaseClass::Serialize(archive);

	iser::CArchiveTag mailTag("Mail", "Mail of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(mailTag);
	retVal = retVal && archive.Process(m_mail);
	retVal = retVal && archive.EndTag(mailTag);

	iser::CArchiveTag passwordTag("PasswordHash", "Password hash", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(passwordTag);
	retVal = retVal && archive.Process(m_passwordHash);
	retVal = retVal && archive.EndTag(passwordTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_groupIds, "Groups", "Group");

	if (imtCoreVersion > 9867 && imtCoreVersion <= 13561){
		iser::CArchiveTag lastConnectionTag("LastConnection", "LastConnection", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(lastConnectionTag);
		retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_lastConnection);
		retVal = retVal && archive.EndTag(lastConnectionTag);
	}

	if (imtCoreVersion >= 10734){
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
	else{
		if (!archive.IsStoring()){
			SystemInfo systemInfo;
			m_systemInfos << systemInfo;
		}
	}

	return retVal;
}


// reimplemented (iser::IChangeable)

bool CUserInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CUserInfo* sourcePtr = dynamic_cast<const CUserInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		BaseClass::CopyFrom(object);

		m_passwordHash = sourcePtr->m_passwordHash;
		m_mail = sourcePtr->m_mail;
		m_groupIds = sourcePtr->m_groupIds;
		m_systemInfos = sourcePtr->m_systemInfos;

		return true;
	}

	return false;
}


bool CUserInfo::IsEqual(const IChangeable& object) const
{
	bool retVal = BaseClass::IsEqual(object);

	const CUserInfo* sourcePtr = dynamic_cast<const CUserInfo*>(&object);
	if (retVal && sourcePtr != nullptr){
		retVal = retVal && m_passwordHash == sourcePtr->m_passwordHash;
		retVal = retVal && m_mail == sourcePtr->m_mail;
		retVal = retVal && m_groupIds == sourcePtr->m_groupIds;
		retVal = retVal && m_lastConnection == sourcePtr->m_lastConnection;
		retVal = retVal && m_systemInfos == sourcePtr->m_systemInfos;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CUserInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CUserInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CUserInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	BaseClass::ResetData(mode);

	m_mail.clear();
	m_passwordHash.clear();
	m_groupIds.clear();

	return true;
}


} // namespace imtauth


