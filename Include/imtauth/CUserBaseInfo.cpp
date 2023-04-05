#include <imtauth/CUserBaseInfo.h>


// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


// public methods


const imtlic::IFeatureInfoProvider *CUserBaseInfo::GetPermissionProvider() const
{
	return m_permissionProviderPtr;
}


const imtauth::IRoleInfoProvider* CUserBaseInfo::GetRoleProvider() const
{
	return m_roleProviderPtr;
}


QString CUserBaseInfo::GetName() const
{
	return m_name;
}


void CUserBaseInfo::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


IUserBaseInfo::FeatureIds CUserBaseInfo::GetPermissions() const
{
	IUserBaseInfo::FeatureIds allPermissions;

	if (m_roleProviderPtr != nullptr){
		for (QByteArray roleId : m_roles){
			const IRole* rolePtr = m_roleProviderPtr->GetRole(roleId);
			if (rolePtr != nullptr){
				allPermissions += rolePtr->GetPermissions();
			}
		}
	}

	allPermissions += m_permissions;

	return allPermissions;
}


IUserBaseInfo::FeatureIds CUserBaseInfo::GetLocalPermissions() const
{
	return m_permissions;
}


void CUserBaseInfo::SetLocalPermissions(const FeatureIds &permissions)
{
	if (m_permissions != permissions){
		istd::CChangeNotifier changeNotifier(this);

		m_permissions = permissions;
	}
}


IUserBaseInfo::FeatureIds CUserBaseInfo::GetProhibitions() const
{
	return m_restrictions;
}


void CUserBaseInfo::SetProhibitions(const FeatureIds &prohibitions)
{
	if (m_restrictions != prohibitions){
		istd::CChangeNotifier changeNotifier(this);

		m_restrictions = prohibitions;
	}
}


IUserBaseInfo::RoleIds CUserBaseInfo::GetRoles() const
{
	return m_roles;
}


void CUserBaseInfo::SetRoles(const RoleIds &roles)
{
	if (m_roles != roles){
		istd::CChangeNotifier changeNotifier(this);

		m_roles = roles;
	}
}


bool CUserBaseInfo::Serialize(iser::IArchive &archive)
{
	bool retVal = true;

	static iser::CArchiveTag nameTag("Name", "Name of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	QByteArray permissionsTag = "Permissions";
	QByteArray permissionTag = "Permission";
	QByteArrayList userPermissions = m_permissions.values();
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, userPermissions, permissionsTag, permissionTag);

	QByteArray restrictionsTag = "Restrictions";
	QByteArray restrictionTag = "Restriction";
	QByteArrayList userRestrictions = m_restrictions.values();
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, userRestrictions, restrictionsTag, restrictionTag);

	QByteArray rolesTag = "Roles";
	QByteArray roleTag = "Role";
	QByteArrayList userRoles = m_roles.values();
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, userRoles, rolesTag, roleTag);

	return retVal;
}


int CUserBaseInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_RESET;
}


bool CUserBaseInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CUserBaseInfo* sourcePtr = dynamic_cast<const CUserBaseInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_name = sourcePtr->m_name;
		m_permissions = sourcePtr->m_permissions;
		m_restrictions = sourcePtr->m_restrictions;
		m_roles = sourcePtr->m_roles;
		m_roleProviderPtr = sourcePtr->m_roleProviderPtr;

		return true;
	}

	return false;
}


istd::IChangeable *CUserBaseInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CUserBaseInfo> clonePtr(new CUserBaseInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CUserBaseInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_name.clear();
	m_permissions.clear();
	m_restrictions.clear();
	m_roles.clear();
	m_roleProviderPtr = nullptr;
	m_permissionProviderPtr = nullptr;

	return true;
}


} // namespace imtauth


