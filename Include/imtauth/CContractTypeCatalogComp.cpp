// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractTypeCatalogComp.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/CollectionSelection.h>


namespace imtauth
{


using namespace CollectionSelection;


// reimplemented (imtauth::IContractTypeCatalog)

ContractTypeInfoList CContractTypeCatalogComp::GetTypes(const QByteArray& tenantId, bool includeArchived, bool* isReadPtr) const
{
	ContractTypeInfoList retVal;
	for (int i = 0; i < m_systemTypesCompPtr.GetCount(); ++i){
		const IContractTypeInfo* systemTypePtr = m_systemTypesCompPtr[i];
		IContractTypeInfoUniquePtr typePtr = (systemTypePtr != nullptr) ? CopyType(*systemTypePtr) : nullptr;
		if (typePtr.IsValid()){
			retVal.append(IContractTypeInfoSharedPtr::CreateFromUnique(std::move(typePtr)));
		}
	}

	bool isRead = true;
	if (!tenantId.isEmpty()){
		for (const QByteArray& id : SelectIds(*m_typeCollectionCompPtr, ColumnEquals("OwnerTenantId", tenantId), QByteArrayList(), &isRead)){
			IContractTypeInfoUniquePtr typePtr = ReadStoredType(id);
			if (!typePtr.IsValid()){
				isRead = false;

				continue;
			}

			if (typePtr->GetOwnerTenantId() == tenantId && (includeArchived || !typePtr->IsArchived())){
				retVal.append(IContractTypeInfoSharedPtr::CreateFromUnique(std::move(typePtr)));
			}
		}
	}

	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
	}

	return retVal;
}


IContractTypeInfoUniquePtr CContractTypeCatalogComp::GetType(const QByteArray& typeId) const
{
	if (typeId.isEmpty()){
		return nullptr;
	}

	for (int i = 0; i < m_systemTypesCompPtr.GetCount(); ++i){
		const IContractTypeInfo* systemTypePtr = m_systemTypesCompPtr[i];
		if (systemTypePtr != nullptr && systemTypePtr->GetTypeId() == typeId){
			return CopyType(*systemTypePtr);
		}
	}

	return ReadStoredType(typeId);
}


QByteArray CContractTypeCatalogComp::CreateType(const IContractTypeInfo& type, QString& errorMessage)
{
	QMutexLocker locker(&m_changeMutex);

	IContractTypeInfoUniquePtr newTypePtr = CopyType(type);
	if (!newTypePtr.IsValid()){
		errorMessage = QStringLiteral("The type could not be created");

		return QByteArray();
	}

	newTypePtr->SetTypeId(QUuid::createUuid().toByteArray(QUuid::WithoutBraces));
	newTypePtr->SetName(type.GetName().trimmed());
	newTypePtr->SetArchived(false);
	if (!CheckType(*newTypePtr, errorMessage)){
		return QByteArray();
	}

	if (!StoreType(*newTypePtr, true)){
		errorMessage = QStringLiteral("The type could not be stored");

		return QByteArray();
	}

	return newTypePtr->GetTypeId();
}


bool CContractTypeCatalogComp::UpdateType(const IContractTypeInfo& type, QString& errorMessage)
{
	QMutexLocker locker(&m_changeMutex);

	IContractTypeInfoUniquePtr storedTypePtr = ReadStoredType(type.GetTypeId());
	IContractTypeInfoUniquePtr changedTypePtr = CopyType(type);
	if (!storedTypePtr.IsValid() || !changedTypePtr.IsValid() || storedTypePtr->GetOwnerTenantId() != type.GetOwnerTenantId()){
		errorMessage = QStringLiteral("The type is not a type of the organization");

		return false;
	}

	changedTypePtr->SetName(type.GetName().trimmed());
	changedTypePtr->SetArchived(storedTypePtr->IsArchived());
	if (!CheckType(*changedTypePtr, errorMessage)){
		return false;
	}

	if (!StoreType(*changedTypePtr, false)){
		errorMessage = QStringLiteral("The type could not be stored");

		return false;
	}

	return true;
}


bool CContractTypeCatalogComp::SetTypeArchived(const QByteArray& tenantId, const QByteArray& typeId, bool isArchived, QString& errorMessage)
{
	QMutexLocker locker(&m_changeMutex);

	IContractTypeInfoUniquePtr typePtr = ReadStoredType(typeId);
	if (!typePtr.IsValid() || typePtr->GetOwnerTenantId() != tenantId){
		errorMessage = QStringLiteral("The type is not a type of the organization");

		return false;
	}

	if (typePtr->IsArchived() == isArchived){
		return true;
	}

	typePtr->SetArchived(isArchived);
	if (!StoreType(*typePtr, false)){
		errorMessage = QStringLiteral("The type could not be stored");

		return false;
	}

	return true;
}


// private methods

IContractTypeInfoUniquePtr CContractTypeCatalogComp::CopyType(const IContractTypeInfo& type) const
{
	IContractTypeInfoUniquePtr retVal = m_typeFactoryCompPtr.CreateInstance();
	if (!retVal.IsValid() || !retVal->CopyFrom(type)){
		return nullptr;
	}

	return retVal;
}


IContractTypeInfoUniquePtr CContractTypeCatalogComp::ReadStoredType(const QByteArray& typeId) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (typeId.isEmpty() || !m_typeCollectionCompPtr->GetObjectData(typeId, dataPtr)){
		return nullptr;
	}

	const IContractTypeInfo* typePtr = dynamic_cast<const IContractTypeInfo*>(dataPtr.GetPtr());
	if (typePtr == nullptr || typePtr->GetTypeId() != typeId){
		return nullptr;
	}

	return CopyType(*typePtr);
}


bool CContractTypeCatalogComp::StoreType(const IContractTypeInfo& type, bool isNew)
{
	istd::CChangeNotifier changeNotifier(this);

	if (isNew){
		return !m_typeCollectionCompPtr->InsertNewObject("ContractType", type.GetName(), type.GetDescription(), &type, type.GetTypeId()).isEmpty();
	}

	return m_typeCollectionCompPtr->SetObjectData(type.GetTypeId(), type);
}


bool CContractTypeCatalogComp::CheckType(const IContractTypeInfo& type, QString& errorMessage) const
{
	const QByteArray ownerTenantId = type.GetOwnerTenantId();
	if (ownerTenantId.isEmpty()){
		errorMessage = QStringLiteral("A type belongs to an organization");

		return false;
	}

	if (type.GetName().isEmpty()){
		errorMessage = QStringLiteral("A type needs a name");

		return false;
	}

	if (type.GetTermDays() < 0 || type.GetRenewalPeriodDays() < 0 || type.GetRenewalNoticeDays() < 0
				|| type.GetTerminationNoticeDays() < 0 || type.GetRequiredSignatures() < 1){
		errorMessage = QStringLiteral("The periods of a type are not negative, and a party gives at least one signature");

		return false;
	}

	if (type.IsAutoRenew() && type.GetRenewalPeriodDays() <= 0){
		errorMessage = QStringLiteral("A renewing type needs the length of a renewal");

		return false;
	}

	QByteArrayList slotRoleIds;
	for (const IContractInfo::RoleSlot& slot : type.GetRoleSlots()){
		if (slotRoleIds.contains(slot.roleId)){
			errorMessage = QStringLiteral("A role has one slot");

			return false;
		}

		slotRoleIds.append(slot.roleId);

		if (!IsRoleAvailable(slot.roleId, ownerTenantId)){
			errorMessage = QStringLiteral("The role '%1' is not available to the organization").arg(QString::fromUtf8(slot.roleId));

			return false;
		}

		if (slot.minCount < 0 || slot.maxCount < 0 || (slot.maxCount > 0 && slot.maxCount < slot.minCount)){
			errorMessage = QStringLiteral("The slot of the role '%1' has wrong limits").arg(QString::fromUtf8(slot.roleId));

			return false;
		}
	}

	if (type.GetLinkScheme() == IContractTypeInfo::LS_STAR && !slotRoleIds.contains(type.GetStarRoleId())){
		errorMessage = QStringLiteral("The center of the links is one of the roles of the type");

		return false;
	}

	for (const IContractTypeInfo::AllocationTemplate& allocationTemplate : type.GetAllocationTemplates()){
		if (!slotRoleIds.contains(allocationTemplate.grantorRoleId) || !slotRoleIds.contains(allocationTemplate.granteeRoleId)
					|| allocationTemplate.productId.isEmpty() || allocationTemplate.permissionIds.isEmpty()){
			errorMessage = QStringLiteral("A template of permissions links two roles of the type and names a product and permissions");

			return false;
		}
	}

	bool isRead = false;
	const ContractTypeInfoList types = GetTypes(ownerTenantId, true, &isRead);
	if (!isRead){
		errorMessage = QStringLiteral("The types of the organization could not be read");

		return false;
	}

	for (const IContractTypeInfoSharedPtr& otherTypePtr : types){
		if (otherTypePtr->GetTypeId() != type.GetTypeId() && otherTypePtr->GetName().compare(type.GetName(), Qt::CaseInsensitive) == 0){
			errorMessage = QStringLiteral("A type with this name already exists");

			return false;
		}
	}

	return true;
}


bool CContractTypeCatalogComp::IsRoleAvailable(const QByteArray& roleId, const QByteArray& tenantId) const
{
	IPartyRoleInfoUniquePtr rolePtr = m_roleCatalogCompPtr->GetRole(roleId);
	if (!rolePtr.IsValid()){
		return false;
	}

	return (rolePtr->GetOwnerTenantId().isEmpty() || rolePtr->GetOwnerTenantId() == tenantId) && (rolePtr->GetApplicability() & IPartyRoleInfo::RA_PARTY) != 0;
}


} // namespace imtauth


