#include <imtauth/CUserInfo.h>


// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods


const imtlic::IFeatureInfoProvider *CUserInfo::GetPermissionProvider() const
{
    return m_permissionProviderPtr;
}


const IRoleInfoProvider *CUserInfo::GetRoleProvider() const
{
    return m_roleProviderPtr;
}


QByteArray CUserInfo::GetId() const
{
	return m_userId;
}


void CUserInfo::SetId(QByteArray id)
{
	if (m_userId != id){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = id;
	}
}


QString CUserInfo::GetUsername() const
{
	return m_username;
}


void CUserInfo::SetUsername(QString username)
{
	if (m_username != username){
		istd::CChangeNotifier changeNotifier(this);

		m_username = username;
	}
}


QString CUserInfo::GetName() const
{
	return m_name;
}


void CUserInfo::SetName(QString name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


QByteArray CUserInfo::GetPasswordHash() const
{
	return m_passwordHash;
}


void CUserInfo::SetPasswordHash(QByteArray passwordHash)
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


void CUserInfo::SetMail(QString mail)
{
	if (m_mail != mail){
		istd::CChangeNotifier changeNotifier(this);

		m_mail = mail;
    }
}


IUserInfo::FeatureIds CUserInfo::GetPermissions() const
{
    IUserInfo::FeatureIds allPermissions;

    for (QByteArray roleId : m_userRoles){
        const IRole* rolePtr = m_roleProviderPtr->GetRole(roleId);
        allPermissions += rolePtr->GetPermissions();
    }

    allPermissions += m_userPermissions;

    for (QByteArray roleId : m_userRoles){
        const IRole* rolePtr = m_roleProviderPtr->GetRole(roleId);
        IRole::FeatureIds prohibitions = rolePtr->GetProhibitions();
        for (const QByteArray& prohibitionId : prohibitions){
            allPermissions.remove(prohibitionId);
        }
    }

    return allPermissions;
}


IUserInfo::FeatureIds CUserInfo::GetLocalPermissions() const
{
    return m_userPermissions;
}


void CUserInfo::SetLocalPermissions(const FeatureIds &permissions)
{
    if (m_userPermissions != permissions){
        istd::CChangeNotifier changeNotifier(this);

        m_userPermissions = permissions;
    }
}


IUserInfo::FeatureIds CUserInfo::GetProhibitions() const
{
    return m_userRestrictions;
}


void CUserInfo::SetProhibitions(const FeatureIds &prohibitions)
{
    if (m_userRestrictions != prohibitions){
        istd::CChangeNotifier changeNotifier(this);

        m_userRestrictions = prohibitions;
    }
}


IUserInfo::RoleIds CUserInfo::GetRoles() const
{
    return m_userRoles;
}


void CUserInfo::SetRoles(const RoleIds &roles)
{
    if (m_userRoles != roles){
        istd::CChangeNotifier changeNotifier(this);

        m_userRoles = roles;
    }
}


bool CUserInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
    bool retVal = true;

	static iser::CArchiveTag userIdTag("UserId", "ID of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	static iser::CArchiveTag usernameTag("Username", "Username of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(usernameTag);
	retVal = retVal && archive.Process(m_username);
	retVal = retVal && archive.EndTag(usernameTag);

	static iser::CArchiveTag nameTag("Name", "Name of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag passwordHashTag("PasswordHash", "Password hash of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(passwordHashTag);
	retVal = retVal && archive.Process(m_passwordHash);
	retVal = retVal && archive.EndTag(passwordHashTag);

	static iser::CArchiveTag mailTag("Mail", "Mail of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(mailTag);
	retVal = retVal && archive.Process(m_mail);
	retVal = retVal && archive.EndTag(mailTag);

    QByteArray permissionsTag = "Permissions";
    QByteArray permissionTag = "Permission";
    QByteArrayList permissions = m_userPermissions.toList();
    retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, permissions, permissionsTag, permissionTag);

    if (!archive.IsStoring()){
        m_userPermissions.fromList(permissions);
    }

    QByteArray restrictionsTag = "Restrictions";
    QByteArray restrictionTag = "Restriction";
    QByteArrayList restrictions = m_userRestrictions.toList();
    retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, restrictions, restrictionsTag, restrictionTag);

    if (!archive.IsStoring()){
        m_userRestrictions.fromList(restrictions);
    }

    QByteArray rolesTag = "Roles";
    QByteArray roleTag = "Role";
    QByteArrayList roles = m_userRoles.toList();
    retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, roles, rolesTag, roleTag);

    if (!archive.IsStoring()){
        m_userRoles.fromList(roles);
    }

	return retVal;
}


int CUserInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_RESET;
}


bool CUserInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CUserInfo* sourcePtr = dynamic_cast<const CUserInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);
        m_userId = sourcePtr->m_userId;
        m_username = sourcePtr->m_username;
        m_name = sourcePtr->m_name;
        m_passwordHash = sourcePtr->m_passwordHash;
        m_mail = sourcePtr->m_mail;
        m_userPermissions = sourcePtr->m_userPermissions;
        m_userRestrictions = sourcePtr->m_userRestrictions;
        m_userRoles = sourcePtr->m_userRoles;
        return true;
	}

	return false;
}


istd::IChangeable *CUserInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CUserInfo> clonePtr(new CUserInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CUserInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);
    m_userId.clear();
    m_username.clear();
    m_name.clear();
    m_passwordHash.clear();
    m_mail.clear();
    m_userPermissions.clear();
    m_userRestrictions.clear();
    m_userRoles.clear();
    return true;
}



} // namespace imtauth


