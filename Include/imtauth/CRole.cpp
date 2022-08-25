#include <imtauth/CRole.h>


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

CRole::CRole()
	:m_permissionProviderPtr(nullptr)
{
}


const imtlic::IFeatureInfoProvider *CRole::GetPermissionProvider() const
{
	return m_permissionProviderPtr;
}


QByteArray CRole::GetRoleId() const
{
	return m_roleId;
}


void CRole::SetRoleId(const QByteArray &id)
{
	if (m_roleId != id){
		istd::CChangeNotifier changeNotifier(this);

		m_roleId = id;
	}
}

QString CRole::GetRoleName() const
{
	return m_roleName;
}

void CRole::SetRoleName(const QString &name)
{
	if (m_roleName != name){
		istd::CChangeNotifier changeNotifier(this);

		m_roleName = name;
	}
}

IRole::FeatureIds CRole::GetPermissions() const
{
	IRole::FeatureIds allPermissions;
	for (const IRole* rolePtr: m_parents){
		allPermissions += rolePtr->GetPermissions();
	}
	allPermissions += m_rolePermissions;
	for (const QByteArray& prohibitionId : m_roleRestrictions){
		allPermissions.remove(prohibitionId);
	}
	return allPermissions;
}

IRole::FeatureIds CRole::GetLocalPermissions() const
{
	return m_rolePermissions;
}

void CRole:: SetLocalPermissions(const IRole::FeatureIds &permissions)
{
	if (m_rolePermissions != permissions){
		istd::CChangeNotifier changeNotifier(this);

		m_rolePermissions = permissions;
	}
}

IRole::FeatureIds CRole::GetProhibitions() const
{
	IRole::FeatureIds allProhibitions;
	for (const IRole* rolePtr: m_parents){
		allProhibitions += rolePtr->GetPermissions();
	}
	allProhibitions += m_roleRestrictions;

	return allProhibitions;
}

void CRole::SetProhibitions(const IRole::FeatureIds &prohibitions)
{
	if (m_roleRestrictions != prohibitions){
		istd::CChangeNotifier changeNotifier(this);

		m_roleRestrictions = prohibitions;
	}
}

const QList<const IRole *> CRole::GetParents() const
{
	return m_parents;
}

void CRole::SetParents(const QList<const IRole *> parents)
{
	if (m_parents != parents){
		istd::CChangeNotifier changeNotifier(this);

		m_parents = parents;
	}
}

bool CRole::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
	bool retVal = true;

	static iser::CArchiveTag roleIdTag("RoleId", "ID of the role", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleIdTag);
	retVal = retVal && archive.Process(m_roleId);
	retVal = retVal && archive.EndTag(roleIdTag);

	static iser::CArchiveTag roleNameTag("RoleName", "Name of the role", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleNameTag);
	retVal = retVal && archive.Process(m_roleName);
	retVal = retVal && archive.EndTag(roleNameTag);

	return retVal;
}

int CRole::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_RESET;
}

bool CRole::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	const CRole* sourcePtr = dynamic_cast<const CRole*>(&object);
	if (sourcePtr != nullptr){
		m_roleId = sourcePtr->m_roleId;
		m_roleName = sourcePtr->m_roleName;
		m_rolePermissions = sourcePtr->m_rolePermissions;
		m_roleRestrictions = sourcePtr->m_roleRestrictions;
		m_parents = sourcePtr->m_parents;
		return true;
	}

	return false;
}


istd::IChangeable *CRole::CloneMe(CompatibilityMode mode) const
{
	return nullptr;
}

bool CRole::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);
	m_roleId.clear();
	m_roleName.clear();
	m_rolePermissions.clear();
	m_roleRestrictions.clear();
	m_parents.clear();

	return true;
}



} // namespace imtauth


