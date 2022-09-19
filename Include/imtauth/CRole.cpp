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
	: m_permissionProviderPtr(nullptr),
	  m_roleInfoProviderPtr(nullptr)
{
}


void CRole::SetParentRoleProvider(const IRoleInfoProvider *parentRoleProvider)
{
	m_roleInfoProviderPtr = parentRoleProvider;
}


const IRoleInfoProvider *CRole::GetParentRolesProvider() const
{
	return m_roleInfoProviderPtr;
}


const imtlic::IFeatureInfoProvider* CRole::GetPermissionProvider() const
{
	return m_permissionProviderPtr;
}


QByteArray CRole::GetRoleId() const
{
	return m_roleId;
}


void CRole::SetRoleId(const QByteArray& id)
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


void CRole::SetRoleName(const QString& name)
{
	if (m_roleName != name){
		istd::CChangeNotifier changeNotifier(this);

		m_roleName = name;
	}
}


IRole::FeatureIds CRole::GetPermissions() const
{
	IRole::FeatureIds allPermissions;
//	for (const QByteArray& roleId : m_parents){
//		const IRole* role = m_roleInfoProviderPtr->GetRole(roleId);
//		allPermissions += role->GetPermissions();
//	}

//	allPermissions += m_rolePermissions;

//	for (const QByteArray& prohibitionId : m_roleRestrictions){
//		allPermissions.remove(prohibitionId);
//	}

	return allPermissions;
}


IRole::FeatureIds CRole::GetLocalPermissions() const
{
	return m_rolePermissions;
}


void CRole:: SetLocalPermissions(const IRole::FeatureIds& permissions)
{
	if (m_rolePermissions != permissions){
		istd::CChangeNotifier changeNotifier(this);

		m_rolePermissions = permissions;
	}
}


IRole::FeatureIds CRole::GetProhibitions() const
{
	IRole::FeatureIds allProhibitions;

	return allProhibitions;
}


void CRole::SetProhibitions(const IRole::FeatureIds& prohibitions)
{
	if (m_roleRestrictions != prohibitions){
		istd::CChangeNotifier changeNotifier(this);

		m_roleRestrictions = prohibitions;
	}
}


QByteArrayList CRole::GetIncludedRoles() const
{
	return m_parents;
}


QByteArray CRole::GetProductId() const
{
	return m_productId;
}


void CRole::SetProductId(const QByteArray& productId)
{
	if (m_productId != productId){
		istd::CChangeNotifier changeNotifier(this);

		m_productId = productId;
	}
}


bool CRole::IncludeRole(const QByteArray &roleId)
{
	if (m_roleId == roleId){
		return false;
	}

	QByteArrayList parentRoles = GetParentRoles(roleId);

	if (parentRoles.contains(m_roleId)){
		return false;
	}

	m_parents.append(roleId);

	return true;
}


void CRole::ExcludeRole(const QByteArray &roleId)
{
	istd::CChangeNotifier changeNotifier(this);

	m_parents.removeOne(roleId);
}


// reimplemented (iser::ISerializable)

bool CRole::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag roleIdTag("RoleId", "ID of the role", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleIdTag);
	retVal = retVal && archive.Process(m_roleId);
	retVal = retVal && archive.EndTag(roleIdTag);

	static iser::CArchiveTag productIdTag("ProductId", "ID of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	static iser::CArchiveTag roleNameTag("RoleName", "Name of the role", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleNameTag);
	retVal = retVal && archive.Process(m_roleName);
	retVal = retVal && archive.EndTag(roleNameTag);

	QByteArray parentsRolesTag = "ParentsRoles";
	QByteArray parentRoleTag = "ParentRole";
	QByteArrayList parentsRoles(m_parents.begin(), m_parents.end());
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, parentsRoles, parentsRolesTag, parentRoleTag);

	if (!archive.IsStoring()){
		m_parents = QByteArrayList(parentsRoles.begin(), parentsRoles.end());
	}

	QByteArray permissionsTag = "Permissions";
	QByteArray permissionTag = "Permission";
	QByteArrayList permissions(m_rolePermissions.begin(),m_rolePermissions.end());
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, permissions, permissionsTag, permissionTag);

	if (!archive.IsStoring()){
		m_rolePermissions = IRole::FeatureIds(permissions.begin(), permissions.end());
	}

	QByteArray restrictionsTag = "Restrictions";
	QByteArray restrictionTag = "Restriction";
	QByteArrayList restrictions(m_roleRestrictions.begin(),m_roleRestrictions.end());
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, restrictions, restrictionsTag, restrictionTag);

	if (!archive.IsStoring()){
		m_roleRestrictions = IRole::FeatureIds(restrictions.begin(), restrictions.end());
	}

	return retVal;
}


// reimplemented (iser::IChangeable)

int CRole::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_RESET | SO_CLONE;
}


bool CRole::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	const CRole* sourcePtr = dynamic_cast<const CRole*>(&object);
	if (sourcePtr != nullptr){
		m_roleId = sourcePtr->m_roleId;
		m_productId = sourcePtr->m_productId;
		m_roleName = sourcePtr->m_roleName;
		m_rolePermissions = sourcePtr->m_rolePermissions;
		m_roleRestrictions = sourcePtr->m_roleRestrictions;
		m_parents = sourcePtr->m_parents;

		if (mode == CM_WITH_REFS){
			m_permissionProviderPtr = sourcePtr->m_permissionProviderPtr;
			m_roleInfoProviderPtr = sourcePtr->m_roleInfoProviderPtr;
		}

		return true;
	}

	return false;
}


istd::IChangeable* CRole::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<imtauth::CRole> clonePtr(new CRole());

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CRole::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_roleId.clear();
	m_productId.clear();
	m_roleName.clear();
	m_rolePermissions.clear();
	m_roleRestrictions.clear();
	m_parents.clear();

	return true;
}


QByteArrayList CRole::GetParentRoles(const QByteArray &roleId) const
{
	QByteArrayList retVal;
	if (m_roleInfoProviderPtr != nullptr){
		const istd::TDelPtr<IRole> rolePtr(const_cast<IRole*>(m_roleInfoProviderPtr->GetRole(roleId, m_productId)));
		if (rolePtr != nullptr){
			GetParentRoleList(*rolePtr, retVal);
		}
	}

	return retVal;
}


void CRole::GetParentRoleList(const IRole &role, QByteArrayList &roleList) const
{
	QByteArrayList includedRoles = role.GetIncludedRoles();

	roleList += includedRoles;
	if (m_roleInfoProviderPtr != nullptr){
		for (const QByteArray& roleId : includedRoles){
			const istd::TDelPtr<IRole> rolePtr(const_cast<IRole*>(m_roleInfoProviderPtr->GetRole(roleId, m_productId)));
			if (rolePtr != nullptr){
				GetParentRoleList(*rolePtr, roleList);
			}
		}
	}
}


} // namespace imtauth


