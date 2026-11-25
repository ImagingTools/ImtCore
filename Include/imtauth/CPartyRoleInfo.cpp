// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CPartyRoleInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/ArchiveFields.h>


namespace imtauth
{


namespace
{


namespace af = ArchiveFields;


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

	bool retVal = af::Leaf(archive, "RoleId", m_roleId);
	retVal = retVal && af::Leaf(archive, "Name", m_name);
	retVal = retVal && af::Leaf(archive, "Description", m_description);
	retVal = retVal && af::Leaf(archive, "OwnerTenantId", m_ownerTenantId);
	retVal = retVal && af::Leaf(archive, "Applicability", m_applicability);
	retVal = retVal && af::Leaf(archive, "PairedRoleId", m_pairedRoleId);
	retVal = retVal && af::Leaf(archive, "TemplateProductId", m_templateProductId);
	retVal = retVal && af::Leaf(archive, "TemplatePermissionIds", m_templatePermissionIds);
	retVal = retVal && af::Leaf(archive, "IsArchived", m_archived);

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


