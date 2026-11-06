// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserBaseInfo.h>


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QSet>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtcore/Version.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{

namespace
{
const QByteArray DirectoryProductPrefix("urn:imt:directory-roles:");
}


QMutex& CUserBaseInfo::GetUserUpdateMutex(const QByteArray& userId)
{
	static QMutex mutexes[64];
	return mutexes[qHash(userId) % 64];
}


IUserBaseInfo::RoleIds CUserBaseInfo::GetDirectoryRoles(const QByteArray& productId) const
{
	if (IsReservedProductId(productId)){ return RoleIds(); }
	return m_rolesMap.value(DirectoryProductPrefix + productId);
}


void CUserBaseInfo::SetDirectoryRoles(const QByteArray& productId, const RoleIds& roles)
{
	if (productId.isEmpty() || IsReservedProductId(productId)){ return; }
	istd::CChangeNotifier notifier(this);
	if (roles.isEmpty()){
		m_rolesMap.remove(DirectoryProductPrefix + productId);
	}
	else{
		m_rolesMap.insert(DirectoryProductPrefix + productId, roles);
	}
}


bool CUserBaseInfo::HasManualProduct(const QByteArray& productId) const
{
	if (IsReservedProductId(productId)){
		return false;
	}

	return m_rolesMap.contains(productId);
}


bool CUserBaseInfo::IsReservedProductId(const QByteArray& productId)
{
	return productId.startsWith(DirectoryProductPrefix);
}

bool CUserBaseInfo::ResolveDirectoryRoles(const QByteArray& productId, RoleIds& roles, FeatureIds& permissions) const
{
	roles.clear();
	permissions.clear();
	RoleIds roots = GetDirectoryRoles(productId);
	QByteArrayList pending = roots;
	QSet<QByteArray> visited;
	FeatureIds resolved;
	while (!pending.isEmpty()){
		QByteArray roleId = pending.takeFirst();
		if (visited.contains(roleId)){ continue; }
		visited.insert(roleId);
		if (m_roleProviderPtr == nullptr || visited.size() > 10000){
			return false;
		}
		imtauth::IRoleUniquePtr role = m_roleProviderPtr->GetRole(roleId);
		if (!role.IsValid() || role->GetProductId() != productId){
			return false;
		}
		for (const QByteArray& permission : role->GetLocalPermissions()){
			if (!resolved.contains(permission)){ resolved.append(permission); }
		}
		pending += role->GetIncludedRoles();
	}
	roles = roots;
	permissions = resolved;
	return true;
}


// public methods

CUserBaseInfo::CUserBaseInfo():
	m_permissionProviderPtr(nullptr),
	m_roleProviderPtr(nullptr),
	m_userGroupInfoProviderPtr(nullptr)
{

}


const imtlic::IFeatureInfoProvider *CUserBaseInfo::GetPermissionProvider() const
{
	return m_permissionProviderPtr;
}


const imtauth::IRoleInfoProvider* CUserBaseInfo::GetRoleProvider() const
{
	return m_roleProviderPtr;
}


QByteArray CUserBaseInfo::GetId() const
{
	return m_id;
}


void CUserBaseInfo::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier changeNotifier(this);

		m_id = id;
	}
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


QString CUserBaseInfo::GetDescription() const
{
	return m_description;
}


void CUserBaseInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


IUserBaseInfo::FeatureIds CUserBaseInfo::GetPermissions(const QByteArray& productId) const
{
	IUserBaseInfo::FeatureIds allPermissions;
	if (IsReservedProductId(productId)){
		return allPermissions;
	}

	if (m_roleProviderPtr != nullptr){
		RoleIds roleIds;
		if (productId.isEmpty()){
			for (auto it = m_rolesMap.cbegin(); it != m_rolesMap.cend(); ++it){
				if (!IsReservedProductId(it.key())){
					roleIds += it.value();
				}
			}
		}
		else{
			auto it = m_rolesMap.constFind(productId);
			if (it != m_rolesMap.cend()){
				roleIds = it.value();
			}
		}

		for (const QByteArray& roleId : std::as_const(roleIds)){
			imtauth::IRoleUniquePtr roleInfoPtr = m_roleProviderPtr->GetRole(roleId);
			if (roleInfoPtr.IsValid()){
				const IUserBaseInfo::FeatureIds rolePerms = roleInfoPtr->GetPermissions();
				for (const QByteArray& perm : std::as_const(rolePerms)){
					if (!allPermissions.contains(perm)){
						allPermissions << perm;
					}
				}
			}
		}

		for (auto it = m_rolesMap.cbegin(); it != m_rolesMap.cend(); ++it){
			if (!IsReservedProductId(it.key())){
				continue;
			}
			QByteArray directoryProduct = it.key().mid(DirectoryProductPrefix.size());
			if (!productId.isEmpty() && productId != directoryProduct){
				continue;
			}
			RoleIds directoryRoles;
			FeatureIds directoryPermissions;
			if (ResolveDirectoryRoles(directoryProduct, directoryRoles, directoryPermissions)){
				for (const QByteArray& permission : directoryPermissions){
					if (!allPermissions.contains(permission)){
						allPermissions.append(permission);
					}
				}
			}
		}
	}

	// Local (directly assigned, not role-based) permissions are always part of the
	// effective permission set - independent of whether a role provider is available.
	FeatureIds localPermissions;
	if (productId.isEmpty()){
		for (auto it = m_permissionsMap.cbegin(); it != m_permissionsMap.cend(); ++it){
			localPermissions += it.value();
		}
	}
	else{
		auto it = m_permissionsMap.constFind(productId);
		if (it != m_permissionsMap.cend()){
			localPermissions = it.value();
		}
	}

	for (const QByteArray& perm : std::as_const(localPermissions)){
		if (!allPermissions.contains(perm)){
			allPermissions << perm;
		}
	}

	return allPermissions;
}



IUserBaseInfo::FeatureIds CUserBaseInfo::GetLocalPermissions(const QByteArray& productId) const
{
	if (m_permissionsMap.contains(productId)){
		return m_permissionsMap[productId];
	}

	return IUserBaseInfo::FeatureIds();
}


void CUserBaseInfo::SetLocalPermissions(const QByteArray& productId, const FeatureIds& permissions)
{
	if (IsReservedProductId(productId)){ return; }
	bool ok = true;
	if (m_permissionsMap.contains(productId)){
		FeatureIds currentFeatureIds = m_permissionsMap.value(productId);
		if (currentFeatureIds == permissions){
			ok = false;
		}
	}

	if (ok){
		istd::CChangeNotifier changeNotifier(this);

		m_permissionsMap.insert(productId, permissions);
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


QByteArrayList CUserBaseInfo::GetProducts() const
{
	QByteArrayList products;
	for (auto it = m_rolesMap.cbegin(); it != m_rolesMap.cend(); ++it){
		QByteArray product = it.key();
		if (IsReservedProductId(product)){
			product = product.mid(DirectoryProductPrefix.size());
		}
		if (!products.contains(product)){
			products.append(product);
		}
	}
	return products;
}


bool CUserBaseInfo::RemoveProduct(const QByteArray& productId)
{
	if (IsReservedProductId(productId)){ return false; }
	if (!m_permissionsMap.contains(productId) && !m_rolesMap.contains(productId) &&
		!m_rolesMap.contains(DirectoryProductPrefix + productId)){
		return false;
	}
	istd::CChangeNotifier changeNotifier(this);
	m_permissionsMap.remove(productId);
	m_rolesMap.remove(productId);
	m_rolesMap.remove(DirectoryProductPrefix + productId);
	return true;
}


IUserBaseInfo::RoleIds CUserBaseInfo::GetRoles(const QByteArray& productId) const
{
	if (IsReservedProductId(productId)){ return RoleIds(); }
	if (m_rolesMap.contains(productId)){
		return m_rolesMap.value(productId);
	}

	return IUserBaseInfo::RoleIds();
}


void CUserBaseInfo::SetRoles(const QByteArray& productId, const RoleIds &roles)
{
	if (IsReservedProductId(productId)){ return; }
	bool ok = true;
	if (m_rolesMap.contains(productId)){
		RoleIds currentRoleIds = m_rolesMap.value(productId);
		if (currentRoleIds == roles){
			ok = false;
		}
	}

	if (ok){
		istd::CChangeNotifier changeNotifier(this);

		m_rolesMap.insert(productId, roles);
	}
}


void CUserBaseInfo::AddRole(const QByteArray& productId, const QByteArray& roleId)
{
	if (IsReservedProductId(productId)){ return; }
	bool isChanged = false;
	if (!m_rolesMap.contains(productId)){
		m_rolesMap.insert(productId, RoleIds());

		isChanged = true;
	}

	RoleIds roleIds = m_rolesMap.value(productId);
	if (!roleIds.contains(roleId)){
		roleIds << roleId;

		isChanged = true;
	}

	if (isChanged){
		istd::CChangeNotifier changeNotifier(this);

		m_rolesMap.insert(productId, roleIds);
	}
}


bool CUserBaseInfo::RemoveRole(const QByteArray& productId, const QByteArray& roleId)
{
	if (IsReservedProductId(productId)){ return false; }
	if (m_rolesMap.contains(productId)){
		RoleIds roleIds = m_rolesMap.value(productId);
		if (roleIds.contains(roleId)){
			istd::CChangeNotifier changeNotifier(this);

			roleIds.removeOne(roleId);

			if (roleIds.isEmpty()){
				m_rolesMap.remove(productId);
			}
			else{
				m_rolesMap.insert(productId, roleIds);
			}

			return true;
		}
	}

	return false;
}


const imtauth::IUserGroupInfoProvider* CUserBaseInfo::GetUserGroupProvider() const
{
	return m_userGroupInfoProviderPtr;
}


bool CUserBaseInfo::Serialize(iser::IArchive &archive)
{
	// Get ImtCore version
	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();
	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	if (imtCoreVersion >= 6940){
		QByteArrayList keys = m_permissionsMap.keys();
		int count = keys.size();

		if (!archive.IsStoring()){
			m_permissionsMap.clear();
			keys.clear();
			count = 0;
		}

		iser::CArchiveTag permissionsTag("Permissions", "Permissions", iser::CArchiveTag::TT_MULTIPLE);
		iser::CArchiveTag permissionTag("Permission", "Permission", iser::CArchiveTag::TT_GROUP, &permissionsTag);

		retVal = retVal && archive.BeginMultiTag(permissionsTag, permissionTag, count);

		for (int index = 0; index < count; index++){
			retVal = retVal && archive.BeginTag(permissionTag);

			QByteArray key;
			QByteArrayList value;

			if (archive.IsStoring()){
				key = keys[index];
				value = m_permissionsMap[key];
			}

			iser::CArchiveTag keyTag("ProductId", "ProductId", iser::CArchiveTag::TT_LEAF, &permissionTag);
			retVal = retVal && archive.BeginTag(keyTag);
			retVal = retVal && archive.Process(key);
			retVal = retVal && archive.EndTag(keyTag);

			retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, value, "Permissions", "PermissionId");

			if (!archive.IsStoring()){
				m_permissionsMap[key] = value;
			}

			retVal = retVal && archive.EndTag(permissionTag);
		}

		retVal = retVal && archive.EndTag(permissionsTag);
	}
	else{
		retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_restrictions, "Permissions", "Permission");
	}

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_restrictions, "Restrictions", "Restriction");

	QByteArrayList keys = m_rolesMap.keys();
	int count = keys.size();

	if (!archive.IsStoring()){
		m_rolesMap.clear();
		keys.clear();
		count = 0;
	}

	iser::CArchiveTag productsTag("Products", "Products", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag roleTag("Role", "Role", iser::CArchiveTag::TT_GROUP, &productsTag);

	retVal = retVal && archive.BeginMultiTag(productsTag, roleTag, count);

	for (int index = 0; index < count; index++){
		retVal = retVal && archive.BeginTag(roleTag);

		QByteArray key;
		QByteArrayList value;

		if (archive.IsStoring()){
			key = keys[index];
			value = m_rolesMap[key];
		}

		iser::CArchiveTag keyTag("ProductId", "ProductId", iser::CArchiveTag::TT_LEAF, &roleTag);
		retVal = retVal && archive.BeginTag(keyTag);
		retVal = retVal && archive.Process(key);
		retVal = retVal && archive.EndTag(keyTag);

		retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, value, "Roles", "RoleId");

		if (!archive.IsStoring()){
			m_rolesMap[key] = value;
		}

		retVal = retVal && archive.EndTag(roleTag);
	}

	retVal = retVal && archive.EndTag(productsTag);

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

		m_id = sourcePtr->m_id;
		m_name = sourcePtr->m_name;
		m_description = sourcePtr->m_description;
		m_permissionsMap = sourcePtr->m_permissionsMap;
		m_restrictions = sourcePtr->m_restrictions;
		m_rolesMap = sourcePtr->m_rolesMap;
		m_roleProviderPtr = sourcePtr->m_roleProviderPtr;
		m_userGroupInfoProviderPtr = sourcePtr->m_userGroupInfoProviderPtr;

		return true;
	}

	return false;
}


bool CUserBaseInfo::IsEqual(const IChangeable& object) const
{
	const CUserBaseInfo* sourcePtr = dynamic_cast<const CUserBaseInfo*>(&object);
	if (sourcePtr != nullptr){
		bool retVal = m_restrictions == sourcePtr->m_restrictions;
		retVal = retVal && m_permissionsMap == sourcePtr->m_permissionsMap;
		retVal = retVal && m_rolesMap == sourcePtr->m_rolesMap;
		retVal = retVal && m_name == sourcePtr->m_name;
		retVal = retVal && m_id == sourcePtr->m_id;
		retVal = retVal && m_description == sourcePtr->m_description;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CUserBaseInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CUserBaseInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CUserBaseInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_id.clear();
	m_name.clear();
	m_description.clear();
	m_permissionsMap.clear();
	m_restrictions.clear();
	m_rolesMap.clear();
	m_roleProviderPtr = nullptr;
	m_permissionProviderPtr = nullptr;
	m_userGroupInfoProviderPtr = nullptr;

	return true;
}


} // namespace imtauth
