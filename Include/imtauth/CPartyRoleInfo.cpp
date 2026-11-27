// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CPartyRoleInfo.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <istd/CChangeNotifier.h>


namespace imtauth
{


namespace
{


template <typename Value>
void AssignValue(istd::IChangeable& object, Value& member, const Value& value)
{
	if (member != value){
		istd::CChangeNotifier notifier(&object);

		member = value;
	}
}


} // anonymous namespace


// public methods

CPartyRoleInfo::CPartyRoleInfo():
	m_applicability(RA_ANY),
	m_archived(false)
{
}


// reimplemented (imtauth::IPartyRoleInfo)

QByteArray CPartyRoleInfo::GetRoleId() const
{
	return m_roleId;
}


void CPartyRoleInfo::SetRoleId(const QByteArray& value)
{
	AssignValue(*this, m_roleId, value);
}


QString CPartyRoleInfo::GetName() const
{
	return m_name;
}


void CPartyRoleInfo::SetName(const QString& value)
{
	AssignValue(*this, m_name, value);
}


QString CPartyRoleInfo::GetDescription() const
{
	return m_description;
}


void CPartyRoleInfo::SetDescription(const QString& value)
{
	AssignValue(*this, m_description, value);
}


QByteArray CPartyRoleInfo::GetOwnerTenantId() const
{
	return m_ownerTenantId;
}


void CPartyRoleInfo::SetOwnerTenantId(const QByteArray& value)
{
	AssignValue(*this, m_ownerTenantId, value);
}


int CPartyRoleInfo::GetApplicability() const
{
	return m_applicability;
}


void CPartyRoleInfo::SetApplicability(int value)
{
	AssignValue(*this, m_applicability, value);
}


QByteArray CPartyRoleInfo::GetPairedRoleId() const
{
	return m_pairedRoleId;
}


void CPartyRoleInfo::SetPairedRoleId(const QByteArray& value)
{
	AssignValue(*this, m_pairedRoleId, value);
}


QByteArray CPartyRoleInfo::GetTemplateProductId() const
{
	return m_templateProductId;
}


void CPartyRoleInfo::SetTemplateProductId(const QByteArray& value)
{
	AssignValue(*this, m_templateProductId, value);
}


QByteArrayList CPartyRoleInfo::GetTemplatePermissionIds() const
{
	return m_templatePermissionIds;
}


void CPartyRoleInfo::SetTemplatePermissionIds(const QByteArrayList& value)
{
	AssignValue(*this, m_templatePermissionIds, value);
}


bool CPartyRoleInfo::IsArchived() const
{
	return m_archived;
}


void CPartyRoleInfo::SetArchived(bool value)
{
	AssignValue(*this, m_archived, value);
}


// reimplemented (iser::ISerializable)

bool CPartyRoleInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag roleIdTag("RoleId", "Role ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleIdTag);
	retVal = retVal && archive.Process(m_roleId);
	retVal = retVal && archive.EndTag(roleIdTag);

	iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	iser::CArchiveTag ownerTenantIdTag("OwnerTenantId", "Owner tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(ownerTenantIdTag);
	retVal = retVal && archive.Process(m_ownerTenantId);
	retVal = retVal && archive.EndTag(ownerTenantIdTag);

	iser::CArchiveTag applicabilityTag("Applicability", "Applicability", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(applicabilityTag);
	retVal = retVal && archive.Process(m_applicability);
	retVal = retVal && archive.EndTag(applicabilityTag);

	iser::CArchiveTag pairedRoleIdTag("PairedRoleId", "Paired role ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(pairedRoleIdTag);
	retVal = retVal && archive.Process(m_pairedRoleId);
	retVal = retVal && archive.EndTag(pairedRoleIdTag);

	iser::CArchiveTag templateProductIdTag("TemplateProductId", "Template product ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(templateProductIdTag);
	retVal = retVal && archive.Process(m_templateProductId);
	retVal = retVal && archive.EndTag(templateProductIdTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_templatePermissionIds, "TemplatePermissionIds", "PermissionId");

	iser::CArchiveTag isArchivedTag("IsArchived", "Is archived", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isArchivedTag);
	retVal = retVal && archive.Process(m_archived);
	retVal = retVal && archive.EndTag(isArchivedTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CPartyRoleInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IPartyRoleInfo* sourcePtr = dynamic_cast<const IPartyRoleInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_roleId = sourcePtr->GetRoleId();
	m_name = sourcePtr->GetName();
	m_description = sourcePtr->GetDescription();
	m_ownerTenantId = sourcePtr->GetOwnerTenantId();
	m_applicability = sourcePtr->GetApplicability();
	m_pairedRoleId = sourcePtr->GetPairedRoleId();
	m_templateProductId = sourcePtr->GetTemplateProductId();
	m_templatePermissionIds = sourcePtr->GetTemplatePermissionIds();
	m_archived = sourcePtr->IsArchived();

	return true;
}


bool CPartyRoleInfo::IsEqual(const IChangeable& object) const
{
	const IPartyRoleInfo* otherPtr = dynamic_cast<const IPartyRoleInfo*>(&object);
	if (otherPtr == nullptr){
		return false;
	}

	return m_roleId == otherPtr->GetRoleId()
				&& m_name == otherPtr->GetName()
				&& m_description == otherPtr->GetDescription()
				&& m_ownerTenantId == otherPtr->GetOwnerTenantId()
				&& m_applicability == otherPtr->GetApplicability()
				&& m_pairedRoleId == otherPtr->GetPairedRoleId()
				&& m_templateProductId == otherPtr->GetTemplateProductId()
				&& m_templatePermissionIds == otherPtr->GetTemplatePermissionIds()
				&& m_archived == otherPtr->IsArchived();
}


istd::IChangeableUniquePtr CPartyRoleInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CPartyRoleInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CPartyRoleInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_roleId = QByteArray();
	m_name = QString();
	m_description = QString();
	m_ownerTenantId = QByteArray();
	m_applicability = RA_ANY;
	m_pairedRoleId = QByteArray();
	m_templateProductId = QByteArray();
	m_templatePermissionIds = QByteArrayList();
	m_archived = false;

	return true;
}


} // namespace imtauth


