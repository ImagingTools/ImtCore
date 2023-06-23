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

	if (m_roleProviderPtr != nullptr){
		RoleIds roleIds;
		if (productId.isEmpty()){
			for (const QByteArray& id : m_rolesMap.keys()){
				RoleIds productRoleIds = m_rolesMap.value(id);
				roleIds += productRoleIds;
			}
		}
		else{
			if (m_rolesMap.contains(productId)){
				roleIds = m_rolesMap.value(productId);
			}
		}

		for (const QByteArray& roleId : roleIds){
			const IRole* rolePtr = m_roleProviderPtr->GetRole(roleId);
			if (rolePtr != nullptr){
				allPermissions += rolePtr->GetPermissions();
			}
		}
	}

	if (m_permissionsMap.contains(productId)){
		allPermissions += m_permissionsMap[productId];
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


void CUserBaseInfo::SetLocalPermissions(const QByteArray& productId, const FeatureIds &permissions)
{
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
	return m_rolesMap.keys();
}


bool CUserBaseInfo::RemoveProduct(const QByteArray& productId)
{
	bool ok = false;
	if (m_permissionsMap.contains(productId)){
		m_permissionsMap.remove(productId);

		ok = true;
	}

	if (m_rolesMap.contains(productId)){
		m_rolesMap.remove(productId);

		ok = true;
	}

	if (ok){
		istd::CChangeNotifier changeNotifier(this);
	}

	return false;
}


IUserBaseInfo::RoleIds CUserBaseInfo::GetRoles(const QByteArray& productId) const
{
	if (m_rolesMap.contains(productId)){
		return m_rolesMap.value(productId);
	}

	return IUserBaseInfo::RoleIds();
}


void CUserBaseInfo::SetRoles(const QByteArray& productId, const RoleIds &roles)
{
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

	static iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_restrictions, "Permissions", "Permission");
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_restrictions, "Restrictions", "Restriction");

//	if (imtCoreVersion < 6671){
//		retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_roles, "Roles", "Role");
//	}
//	else{
		QByteArrayList keys = m_rolesMap.keys();
		int count = keys.count();

		if (!archive.IsStoring()){
			m_rolesMap.clear();
			keys.clear();
			count = 0;
		}

		static iser::CArchiveTag productsTag("Products", "Products", iser::CArchiveTag::TT_MULTIPLE);
		static iser::CArchiveTag roleTag("Role", "Role", iser::CArchiveTag::TT_GROUP, &productsTag);

		retVal = retVal && archive.BeginMultiTag(productsTag, roleTag, count);

		for (int index = 0; index < count; index++){
			retVal = retVal && archive.BeginTag(roleTag);

			QByteArray key;
			QByteArrayList value;

			if (archive.IsStoring()){
				key = keys[index];
				value = m_rolesMap[key];
			}

			static iser::CArchiveTag keyTag("ProductId", "ProductId", iser::CArchiveTag::TT_LEAF, &roleTag);
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
//	}

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
		m_permissionsMap = sourcePtr->m_permissionsMap;
		m_restrictions = sourcePtr->m_restrictions;
		m_rolesMap = sourcePtr->m_rolesMap;
		m_roleProviderPtr = sourcePtr->m_roleProviderPtr;
		m_userGroupInfoProviderPtr = sourcePtr->m_userGroupInfoProviderPtr;

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

	m_id.clear();
	m_name.clear();
	m_permissionsMap.clear();
	m_restrictions.clear();
	m_rolesMap.clear();
	m_roleProviderPtr = nullptr;
	m_permissionProviderPtr = nullptr;
	m_userGroupInfoProviderPtr = nullptr;

	return true;
}


} // namespace imtauth


