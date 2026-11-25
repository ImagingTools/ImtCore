// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractTypeInfo.h>


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


bool SerializeRoleSlot(iser::IArchive& archive, IContractInfo::RoleSlot& item)
{
	bool retVal = af::Leaf(archive, "RoleId", item.roleId);
	retVal = retVal && af::Leaf(archive, "MinCount", item.minCount);
	retVal = retVal && af::Leaf(archive, "MaxCount", item.maxCount);

	return retVal;
}


bool SerializeAllocationTemplate(iser::IArchive& archive, IContractTypeInfo::AllocationTemplate& item)
{
	bool retVal = af::Leaf(archive, "GrantorRoleId", item.grantorRoleId);
	retVal = retVal && af::Leaf(archive, "GranteeRoleId", item.granteeRoleId);
	retVal = retVal && af::Leaf(archive, "ProductId", item.productId);
	retVal = retVal && af::Leaf(archive, "PermissionIds", item.permissionIds);

	return retVal;
}


bool SerializeNumber(iser::IArchive& archive, int& number)
{
	return af::Leaf(archive, "Value", number);
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
	m_signOrder(IContractInfo::SO_PARALLEL),
	m_requiredSignatures(1),
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


IContractInfo::SignOrder CContractTypeInfo::GetSignOrder() const
{
	return m_signOrder;
}


void CContractTypeInfo::SetSignOrder(IContractInfo::SignOrder value)
{
	AssignValue(*this, m_signOrder, value);
}


int CContractTypeInfo::GetRequiredSignatures() const
{
	return m_requiredSignatures;
}


void CContractTypeInfo::SetRequiredSignatures(int value)
{
	AssignValue(*this, m_requiredSignatures, value);
}


QList<int> CContractTypeInfo::GetRequiredAttachmentKinds() const
{
	return m_requiredAttachmentKinds;
}


void CContractTypeInfo::SetRequiredAttachmentKinds(const QList<int>& value)
{
	AssignValue(*this, m_requiredAttachmentKinds, value);
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

	bool retVal = af::Leaf(archive, "TypeId", m_typeId);
	retVal = retVal && af::Leaf(archive, "Name", m_name);
	retVal = retVal && af::Leaf(archive, "Description", m_description);
	retVal = retVal && af::Leaf(archive, "OwnerTenantId", m_ownerTenantId);
	retVal = retVal && af::Leaf(archive, "NumberPrefix", m_numberPrefix);
	retVal = retVal && af::List(archive, "RoleSlots", "RoleSlot", m_roleSlots, SerializeRoleSlot);
	retVal = retVal && af::EnumLeaf(archive, "LinkScheme", m_linkScheme);
	retVal = retVal && af::Leaf(archive, "StarRoleId", m_starRoleId);
	retVal = retVal && af::Leaf(archive, "TermDays", m_termDays);
	retVal = retVal && af::Leaf(archive, "AutoRenew", m_autoRenew);
	retVal = retVal && af::Leaf(archive, "RenewalPeriodDays", m_renewalPeriodDays);
	retVal = retVal && af::Leaf(archive, "RenewalNoticeDays", m_renewalNoticeDays);
	retVal = retVal && af::Leaf(archive, "TerminationNoticeDays", m_terminationNoticeDays);
	retVal = retVal && af::EnumLeaf(archive, "SignOrder", m_signOrder);
	retVal = retVal && af::Leaf(archive, "RequiredSignatures", m_requiredSignatures);
	retVal = retVal && af::List(archive, "RequiredAttachmentKinds", "Kind", m_requiredAttachmentKinds, SerializeNumber);
	retVal = retVal && af::List(archive, "AllocationTemplates", "Template", m_allocationTemplates, SerializeAllocationTemplate);
	retVal = retVal && af::Leaf(archive, "IsArchived", m_archived);

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
	m_signOrder = sourcePtr->GetSignOrder();
	m_requiredSignatures = sourcePtr->GetRequiredSignatures();
	m_requiredAttachmentKinds = sourcePtr->GetRequiredAttachmentKinds();
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
				&& m_signOrder == otherPtr->GetSignOrder()
				&& m_requiredSignatures == otherPtr->GetRequiredSignatures()
				&& m_requiredAttachmentKinds == otherPtr->GetRequiredAttachmentKinds()
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
	m_signOrder = IContractInfo::SO_PARALLEL;
	m_requiredSignatures = 1;
	m_requiredAttachmentKinds = QList<int>();
	m_allocationTemplates = IContractTypeInfo::AllocationTemplates();
	m_archived = false;

	return true;
}


} // namespace imtauth


