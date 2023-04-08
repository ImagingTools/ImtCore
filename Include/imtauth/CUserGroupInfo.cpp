#include <imtauth/CUserGroupInfo.h>


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

CUserGroupInfo:: CUserGroupInfo():
	m_parentGroupInfoPtr(nullptr)
{
}


// reimplemented (IUserGroupInfo)

QString CUserGroupInfo::GetDescription() const
{
	return m_description;
}


void CUserGroupInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


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


const imtauth::IUserInfoProvider* CUserGroupInfo::GetUserProvider() const
{
	return nullptr;
}


const IUserGroupInfo* CUserGroupInfo::GetParentGroup() const
{
	return m_parentGroupInfoPtr;
}


const istd::TPointerVector<const IUserGroupInfo>& CUserGroupInfo::GetSubGroups() const
{
	return m_subGroups;
}


bool CUserGroupInfo::InsertSubGroup(const IUserGroupInfo* subGroupInfo)
{
	bool retVal = false;
	if (this != subGroupInfo){
		istd::CChangeNotifier notifier(this);

		m_subGroups.PushBack(subGroupInfo);

		retVal = true;
	}

	return retVal;
}


void CUserGroupInfo::DeleteSubGroup(const QByteArray& subGroupId)
{
	for (int i = 0; i < m_subGroups.GetCount(); i++){
		const IUserGroupInfo* subGroupInfoPtr = m_subGroups.GetAt(i);
		QByteArray groupId = subGroupInfoPtr->GetId();

		if (subGroupId == groupId){
			istd::CChangeNotifier notifier(this);

			m_subGroups.Remove(subGroupInfoPtr);

			break;
		}
	}
}


bool CUserGroupInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);

	static iser::CArchiveTag descriptionTag("Description", "Description of the group", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	QByteArrayList userIds = m_userIds.values();
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, userIds, "Users", "User");

//	static iser::CArchiveTag parentTag("ParentGroup", "Parent of this group", iser::CArchiveTag::TT_LEAF);
//	retVal = retVal && archive.BeginTag(parentTag);
//	retVal = retVal && const_cast<IUserGroupInfo*>(m_parentGroupInfoPtr)->Serialize(archive);
//	retVal = retVal && archive.EndTag(parentTag);

	static iser::CArchiveTag subGroupsTag("SubGroups", "Subgroups of the group", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag subGroupTag("Group", "Group item", iser::CArchiveTag::TT_GROUP, &subGroupsTag);

	int subGroupCount = m_subGroups.GetCount();

	retVal = retVal && archive.BeginMultiTag(subGroupsTag, subGroupTag, subGroupCount);

	if (!archive.IsStoring()){
		int oldSubGroupCount = m_subGroups.GetCount();
		m_subGroups.SetCount(subGroupCount);
		for (int i = oldSubGroupCount; i < subGroupCount; ++i){
			m_subGroups.SetElementAt(i, new imod::TModelWrap<CIdentifiableUserGroupInfo>());
		}
	}

	for (int i = 0; i < m_subGroups.GetCount(); i++){
		const IUserGroupInfo* groupInfoPtr = m_subGroups.GetAt(i);
		retVal = retVal && archive.BeginTag(subGroupTag);
		retVal = retVal && const_cast<IUserGroupInfo*>(groupInfoPtr)->Serialize(archive);
		retVal = retVal && archive.EndTag(subGroupTag);
	}

	retVal = retVal && archive.EndTag(subGroupsTag);

	return retVal;
}


bool CUserGroupInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CUserGroupInfo* sourcePtr = dynamic_cast<const CUserGroupInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		BaseClass::CopyFrom(object);

		m_description = sourcePtr->m_description;
		m_userIds = sourcePtr->m_userIds;

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

	m_description.clear();
	m_userIds.clear();

	return true;
}


} // namespace imtauth


