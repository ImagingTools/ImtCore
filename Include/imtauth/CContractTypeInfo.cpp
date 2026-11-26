// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractTypeInfo.h>


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


bool SerializeRoleSlot(iser::IArchive& archive, IContractInfo::RoleSlot& item)
{
	bool retVal = true;

	iser::CArchiveTag roleIdTag("RoleId", "Role ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleIdTag);
	retVal = retVal && archive.Process(item.roleId);
	retVal = retVal && archive.EndTag(roleIdTag);

	iser::CArchiveTag minCountTag("MinCount", "Min count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(minCountTag);
	retVal = retVal && archive.Process(item.minCount);
	retVal = retVal && archive.EndTag(minCountTag);

	iser::CArchiveTag maxCountTag("MaxCount", "Max count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(maxCountTag);
	retVal = retVal && archive.Process(item.maxCount);
	retVal = retVal && archive.EndTag(maxCountTag);

	return retVal;
}


bool SerializeAllocationTemplate(iser::IArchive& archive, IContractTypeInfo::AllocationTemplate& item)
{
	bool retVal = true;

	iser::CArchiveTag grantorRoleIdTag("GrantorRoleId", "Grantor role ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(grantorRoleIdTag);
	retVal = retVal && archive.Process(item.grantorRoleId);
	retVal = retVal && archive.EndTag(grantorRoleIdTag);

	iser::CArchiveTag granteeRoleIdTag("GranteeRoleId", "Grantee role ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(granteeRoleIdTag);
	retVal = retVal && archive.Process(item.granteeRoleId);
	retVal = retVal && archive.EndTag(granteeRoleIdTag);

	iser::CArchiveTag productIdTag("ProductId", "Product ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(item.productId);
	retVal = retVal && archive.EndTag(productIdTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, item.permissionIds, "PermissionIds", "PermissionId");

	return retVal;
}


} // anonymous namespace


// public methods of the types of IContractTypeInfo

bool IContractTypeInfo::AllocationTemplate::operator==(const AllocationTemplate& other) const
{
	return grantorRoleId == other.grantorRoleId
				&& granteeRoleId == other.granteeRoleId
				&& productId == other.productId
				&& permissionIds == other.permissionIds;
}


bool IContractTypeInfo::AllocationTemplate::operator!=(const AllocationTemplate& other) const
{
	return !(*this == other);
}


// public methods

CContractTypeInfo::CContractTypeInfo():
	m_linkScheme(LS_MANUAL),
	m_termDays(0),
	m_autoRenew(false),
	m_renewalPeriodDays(0),
	m_renewalNoticeDays(0),
	m_terminationNoticeDays(0),
	m_archived(false)
{
}


// reimplemented (imtauth::IContractTypeInfo)

QByteArray CContractTypeInfo::GetTypeId() const
{
	return m_typeId;
}


void CContractTypeInfo::SetTypeId(const QByteArray& value)
{
	AssignValue(*this, m_typeId, value);
}


QString CContractTypeInfo::GetName() const
{
	return m_name;
}


void CContractTypeInfo::SetName(const QString& value)
{
	AssignValue(*this, m_name, value);
}


QString CContractTypeInfo::GetDescription() const
{
	return m_description;
}


void CContractTypeInfo::SetDescription(const QString& value)
{
	AssignValue(*this, m_description, value);
}


QByteArray CContractTypeInfo::GetOwnerTenantId() const
{
	return m_ownerTenantId;
}


void CContractTypeInfo::SetOwnerTenantId(const QByteArray& value)
{
	AssignValue(*this, m_ownerTenantId, value);
}


QString CContractTypeInfo::GetNumberPrefix() const
{
	return m_numberPrefix;
}


void CContractTypeInfo::SetNumberPrefix(const QString& value)
{
	AssignValue(*this, m_numberPrefix, value);
}


IContractInfo::RoleSlots CContractTypeInfo::GetRoleSlots() const
{
	return m_roleSlots;
}


void CContractTypeInfo::SetRoleSlots(const IContractInfo::RoleSlots& value)
{
	AssignValue(*this, m_roleSlots, value);
}


IContractTypeInfo::LinkScheme CContractTypeInfo::GetLinkScheme() const
{
	return m_linkScheme;
}


void CContractTypeInfo::SetLinkScheme(IContractTypeInfo::LinkScheme value)
{
	AssignValue(*this, m_linkScheme, value);
}


QByteArray CContractTypeInfo::GetStarRoleId() const
{
	return m_starRoleId;
}


void CContractTypeInfo::SetStarRoleId(const QByteArray& value)
{
	AssignValue(*this, m_starRoleId, value);
}


int CContractTypeInfo::GetTermDays() const
{
	return m_termDays;
}


void CContractTypeInfo::SetTermDays(int value)
{
	AssignValue(*this, m_termDays, value);
}


bool CContractTypeInfo::IsAutoRenew() const
{
	return m_autoRenew;
}


void CContractTypeInfo::SetAutoRenew(bool value)
{
	AssignValue(*this, m_autoRenew, value);
}


int CContractTypeInfo::GetRenewalPeriodDays() const
{
	return m_renewalPeriodDays;
}


void CContractTypeInfo::SetRenewalPeriodDays(int value)
{
	AssignValue(*this, m_renewalPeriodDays, value);
}


int CContractTypeInfo::GetRenewalNoticeDays() const
{
	return m_renewalNoticeDays;
}


void CContractTypeInfo::SetRenewalNoticeDays(int value)
{
	AssignValue(*this, m_renewalNoticeDays, value);
}


int CContractTypeInfo::GetTerminationNoticeDays() const
{
	return m_terminationNoticeDays;
}


void CContractTypeInfo::SetTerminationNoticeDays(int value)
{
	AssignValue(*this, m_terminationNoticeDays, value);
}


IContractTypeInfo::AllocationTemplates CContractTypeInfo::GetAllocationTemplates() const
{
	return m_allocationTemplates;
}


void CContractTypeInfo::SetAllocationTemplates(const IContractTypeInfo::AllocationTemplates& value)
{
	AssignValue(*this, m_allocationTemplates, value);
}


bool CContractTypeInfo::IsArchived() const
{
	return m_archived;
}


void CContractTypeInfo::SetArchived(bool value)
{
	AssignValue(*this, m_archived, value);
}


// reimplemented (iser::ISerializable)

bool CContractTypeInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag typeIdTag("TypeId", "Type ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeIdTag);
	retVal = retVal && archive.Process(m_typeId);
	retVal = retVal && archive.EndTag(typeIdTag);

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

	iser::CArchiveTag numberPrefixTag("NumberPrefix", "Number prefix", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(numberPrefixTag);
	retVal = retVal && archive.Process(m_numberPrefix);
	retVal = retVal && archive.EndTag(numberPrefixTag);

	iser::CArchiveTag roleSlotsTag("RoleSlots", "Role slots", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag roleSlotTag("RoleSlot", "Role slot", iser::CArchiveTag::TT_GROUP, &roleSlotsTag);
	int roleSlotsCount = m_roleSlots.size();
	retVal = retVal && archive.BeginMultiTag(roleSlotsTag, roleSlotTag, roleSlotsCount);
	if (retVal && !archive.IsStoring()){
		m_roleSlots.clear();
		m_roleSlots.resize(roleSlotsCount);
	}

	for (int i = 0; retVal && i < roleSlotsCount; ++i){
		retVal = archive.BeginTag(roleSlotTag) && SerializeRoleSlot(archive, m_roleSlots[i]) && archive.EndTag(roleSlotTag);
	}

	retVal = retVal && archive.EndTag(roleSlotsTag);

	iser::CArchiveTag linkSchemeTag("LinkScheme", "Link scheme", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(linkSchemeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<LinkScheme, ToString, FromString>(archive, m_linkScheme);
	retVal = retVal && archive.EndTag(linkSchemeTag);

	iser::CArchiveTag starRoleIdTag("StarRoleId", "Star role ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(starRoleIdTag);
	retVal = retVal && archive.Process(m_starRoleId);
	retVal = retVal && archive.EndTag(starRoleIdTag);

	iser::CArchiveTag termDaysTag("TermDays", "Term days", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(termDaysTag);
	retVal = retVal && archive.Process(m_termDays);
	retVal = retVal && archive.EndTag(termDaysTag);

	iser::CArchiveTag autoRenewTag("AutoRenew", "Auto renew", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(autoRenewTag);
	retVal = retVal && archive.Process(m_autoRenew);
	retVal = retVal && archive.EndTag(autoRenewTag);

	iser::CArchiveTag renewalPeriodDaysTag("RenewalPeriodDays", "Renewal period days", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(renewalPeriodDaysTag);
	retVal = retVal && archive.Process(m_renewalPeriodDays);
	retVal = retVal && archive.EndTag(renewalPeriodDaysTag);

	iser::CArchiveTag renewalNoticeDaysTag("RenewalNoticeDays", "Renewal notice days", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(renewalNoticeDaysTag);
	retVal = retVal && archive.Process(m_renewalNoticeDays);
	retVal = retVal && archive.EndTag(renewalNoticeDaysTag);

	iser::CArchiveTag terminationNoticeDaysTag("TerminationNoticeDays", "Termination notice days", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(terminationNoticeDaysTag);
	retVal = retVal && archive.Process(m_terminationNoticeDays);
	retVal = retVal && archive.EndTag(terminationNoticeDaysTag);

	iser::CArchiveTag allocationTemplatesTag("AllocationTemplates", "Allocation templates", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag templateTag("Template", "Template", iser::CArchiveTag::TT_GROUP, &allocationTemplatesTag);
	int allocationTemplatesCount = m_allocationTemplates.size();
	retVal = retVal && archive.BeginMultiTag(allocationTemplatesTag, templateTag, allocationTemplatesCount);
	if (retVal && !archive.IsStoring()){
		m_allocationTemplates.clear();
		m_allocationTemplates.resize(allocationTemplatesCount);
	}

	for (int i = 0; retVal && i < allocationTemplatesCount; ++i){
		retVal = archive.BeginTag(templateTag) && SerializeAllocationTemplate(archive, m_allocationTemplates[i]) && archive.EndTag(templateTag);
	}

	retVal = retVal && archive.EndTag(allocationTemplatesTag);

	iser::CArchiveTag isArchivedTag("IsArchived", "Is archived", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isArchivedTag);
	retVal = retVal && archive.Process(m_archived);
	retVal = retVal && archive.EndTag(isArchivedTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CContractTypeInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IContractTypeInfo* sourcePtr = dynamic_cast<const IContractTypeInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_typeId = sourcePtr->GetTypeId();
	m_name = sourcePtr->GetName();
	m_description = sourcePtr->GetDescription();
	m_ownerTenantId = sourcePtr->GetOwnerTenantId();
	m_numberPrefix = sourcePtr->GetNumberPrefix();
	m_roleSlots = sourcePtr->GetRoleSlots();
	m_linkScheme = sourcePtr->GetLinkScheme();
	m_starRoleId = sourcePtr->GetStarRoleId();
	m_termDays = sourcePtr->GetTermDays();
	m_autoRenew = sourcePtr->IsAutoRenew();
	m_renewalPeriodDays = sourcePtr->GetRenewalPeriodDays();
	m_renewalNoticeDays = sourcePtr->GetRenewalNoticeDays();
	m_terminationNoticeDays = sourcePtr->GetTerminationNoticeDays();
	m_allocationTemplates = sourcePtr->GetAllocationTemplates();
	m_archived = sourcePtr->IsArchived();

	return true;
}


bool CContractTypeInfo::IsEqual(const IChangeable& object) const
{
	const IContractTypeInfo* otherPtr = dynamic_cast<const IContractTypeInfo*>(&object);
	if (otherPtr == nullptr){
		return false;
	}

	return m_typeId == otherPtr->GetTypeId()
				&& m_name == otherPtr->GetName()
				&& m_description == otherPtr->GetDescription()
				&& m_ownerTenantId == otherPtr->GetOwnerTenantId()
				&& m_numberPrefix == otherPtr->GetNumberPrefix()
				&& m_roleSlots == otherPtr->GetRoleSlots()
				&& m_linkScheme == otherPtr->GetLinkScheme()
				&& m_starRoleId == otherPtr->GetStarRoleId()
				&& m_termDays == otherPtr->GetTermDays()
				&& m_autoRenew == otherPtr->IsAutoRenew()
				&& m_renewalPeriodDays == otherPtr->GetRenewalPeriodDays()
				&& m_renewalNoticeDays == otherPtr->GetRenewalNoticeDays()
				&& m_terminationNoticeDays == otherPtr->GetTerminationNoticeDays()
				&& m_allocationTemplates == otherPtr->GetAllocationTemplates()
				&& m_archived == otherPtr->IsArchived();
}


istd::IChangeableUniquePtr CContractTypeInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CContractTypeInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CContractTypeInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_typeId = QByteArray();
	m_name = QString();
	m_description = QString();
	m_ownerTenantId = QByteArray();
	m_numberPrefix = QString();
	m_roleSlots = IContractInfo::RoleSlots();
	m_linkScheme = LS_MANUAL;
	m_starRoleId = QByteArray();
	m_termDays = 0;
	m_autoRenew = false;
	m_renewalPeriodDays = 0;
	m_renewalNoticeDays = 0;
	m_terminationNoticeDays = 0;
	m_allocationTemplates = IContractTypeInfo::AllocationTemplates();
	m_archived = false;

	return true;
}


} // namespace imtauth


