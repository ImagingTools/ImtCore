// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CPartyRoleCatalogComp.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/CollectionSelection.h>


namespace imtauth
{


using namespace CollectionSelection;


// reimplemented (imtauth::IPartyRoleCatalog)

PartyRoleInfoList CPartyRoleCatalogComp::GetRoles(const QByteArray& tenantId, bool includeArchived, bool* isReadPtr) const
{
	PartyRoleInfoList retVal;
	for (int i = 0; i < m_systemRolesCompPtr.GetCount(); ++i){
		const IPartyRoleInfo* systemRolePtr = m_systemRolesCompPtr[i];
		IPartyRoleInfoUniquePtr rolePtr = (systemRolePtr != nullptr) ? CopyRole(*systemRolePtr) : nullptr;
		if (rolePtr.IsValid()){
			retVal.append(IPartyRoleInfoSharedPtr::CreateFromUnique(std::move(rolePtr)));
		}
	}

	bool isRead = true;
	if (!tenantId.isEmpty()){
		for (const QByteArray& id : SelectIds(*m_roleCollectionCompPtr, ColumnEquals("OwnerTenantId", tenantId), QByteArrayList(), &isRead)){
			IPartyRoleInfoUniquePtr rolePtr = ReadStoredRole(id);
			if (!rolePtr.IsValid()){
				isRead = false;

				continue;
			}

			if (rolePtr->GetOwnerTenantId() == tenantId && (includeArchived || !rolePtr->IsArchived())){
				retVal.append(IPartyRoleInfoSharedPtr::CreateFromUnique(std::move(rolePtr)));
			}
		}
	}

	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
	}

	return retVal;
}


IPartyRoleInfoUniquePtr CPartyRoleCatalogComp::GetRole(const QByteArray& roleId) const
{
	if (roleId.isEmpty()){
		return nullptr;
	}

	for (int i = 0; i < m_systemRolesCompPtr.GetCount(); ++i){
		const IPartyRoleInfo* systemRolePtr = m_systemRolesCompPtr[i];
		if (systemRolePtr != nullptr && systemRolePtr->GetRoleId() == roleId){
			return CopyRole(*systemRolePtr);
		}
	}

	return ReadStoredRole(roleId);
}


QByteArray CPartyRoleCatalogComp::CreateRole(const IPartyRoleInfo& role, QString& errorMessage)
{
	QMutexLocker locker(&m_changeMutex);

	IPartyRoleInfoUniquePtr newRolePtr = CopyRole(role);
	if (!newRolePtr.IsValid()){
		errorMessage = QStringLiteral("The role could not be created");

		return QByteArray();
	}

	newRolePtr->SetRoleId(QUuid::createUuid().toByteArray(QUuid::WithoutBraces));
	newRolePtr->SetName(role.GetName().trimmed());
	newRolePtr->SetArchived(false);
	if (!CheckRole(*newRolePtr, errorMessage)){
		return QByteArray();
	}

	if (!StoreRole(*newRolePtr, true)){
		errorMessage = QStringLiteral("The role could not be stored");

		return QByteArray();
	}

	return newRolePtr->GetRoleId();
}


bool CPartyRoleCatalogComp::UpdateRole(const IPartyRoleInfo& role, QString& errorMessage)
{
	QMutexLocker locker(&m_changeMutex);

	IPartyRoleInfoUniquePtr storedRolePtr = ReadStoredRole(role.GetRoleId());
	IPartyRoleInfoUniquePtr changedRolePtr = CopyRole(role);
	if (!storedRolePtr.IsValid() || !changedRolePtr.IsValid() || storedRolePtr->GetOwnerTenantId() != role.GetOwnerTenantId()){
		errorMessage = QStringLiteral("The role is not a role of the organization");

		return false;
	}

	changedRolePtr->SetName(role.GetName().trimmed());
	changedRolePtr->SetArchived(storedRolePtr->IsArchived());
	if (!CheckRole(*changedRolePtr, errorMessage)){
		return false;
	}

	if (!StoreRole(*changedRolePtr, false)){
		errorMessage = QStringLiteral("The role could not be stored");

		return false;
	}

	return true;
}


bool CPartyRoleCatalogComp::SetRoleArchived(const QByteArray& tenantId, const QByteArray& roleId, bool isArchived, QString& errorMessage)
{
	QMutexLocker locker(&m_changeMutex);

	IPartyRoleInfoUniquePtr rolePtr = ReadStoredRole(roleId);
	if (!rolePtr.IsValid() || rolePtr->GetOwnerTenantId() != tenantId){
		errorMessage = QStringLiteral("The role is not a role of the organization");

		return false;
	}

	if (rolePtr->IsArchived() == isArchived){
		return true;
	}

	rolePtr->SetArchived(isArchived);
	if (!StoreRole(*rolePtr, false)){
		errorMessage = QStringLiteral("The role could not be stored");

		return false;
	}

	return true;
}


// private methods

IPartyRoleInfoUniquePtr CPartyRoleCatalogComp::CopyRole(const IPartyRoleInfo& role) const
{
	IPartyRoleInfoUniquePtr retVal = m_roleFactoryCompPtr.CreateInstance();
	if (!retVal.IsValid() || !retVal->CopyFrom(role)){
		return nullptr;
	}

	return retVal;
}


IPartyRoleInfoUniquePtr CPartyRoleCatalogComp::ReadStoredRole(const QByteArray& roleId) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (roleId.isEmpty() || !m_roleCollectionCompPtr->GetObjectData(roleId, dataPtr)){
		return nullptr;
	}

	const IPartyRoleInfo* rolePtr = dynamic_cast<const IPartyRoleInfo*>(dataPtr.GetPtr());
	if (rolePtr == nullptr || rolePtr->GetRoleId() != roleId){
		return nullptr;
	}

	return CopyRole(*rolePtr);
}


bool CPartyRoleCatalogComp::StoreRole(const IPartyRoleInfo& role, bool isNew)
{
	istd::CChangeNotifier changeNotifier(this);

	if (isNew){
		return !m_roleCollectionCompPtr->InsertNewObject("PartyRole", role.GetName(), role.GetDescription(), &role, role.GetRoleId()).isEmpty();
	}

	return m_roleCollectionCompPtr->SetObjectData(role.GetRoleId(), role);
}


bool CPartyRoleCatalogComp::CheckRole(const IPartyRoleInfo& role, QString& errorMessage) const
{
	const QByteArray ownerTenantId = role.GetOwnerTenantId();
	if (ownerTenantId.isEmpty()){
		errorMessage = QStringLiteral("A role belongs to an organization");

		return false;
	}

	if (role.GetName().isEmpty()){
		errorMessage = QStringLiteral("A role needs a name");

		return false;
	}

	if ((role.GetApplicability() & IPartyRoleInfo::RA_ANY) == 0){
		errorMessage = QStringLiteral("A role is used for a party, for a link or for both");

		return false;
	}

	const QByteArray pairedRoleId = role.GetPairedRoleId();
	if (!pairedRoleId.isEmpty() && pairedRoleId != role.GetRoleId()){
		IPartyRoleInfoUniquePtr pairedRolePtr = GetRole(pairedRoleId);
		if (!pairedRolePtr.IsValid() || (!pairedRolePtr->GetOwnerTenantId().isEmpty() && pairedRolePtr->GetOwnerTenantId() != ownerTenantId)){
			errorMessage = QStringLiteral("The paired role is not available to the organization");

			return false;
		}
	}

	// The names of the roles an organization sees are unique, ignoring the case
	bool isRead = false;
	const PartyRoleInfoList roles = GetRoles(ownerTenantId, true, &isRead);
	if (!isRead){
		errorMessage = QStringLiteral("The roles of the organization could not be read");

		return false;
	}

	for (const IPartyRoleInfoSharedPtr& otherRolePtr : roles){
		if (otherRolePtr->GetRoleId() != role.GetRoleId() && otherRolePtr->GetName().compare(role.GetName(), Qt::CaseInsensitive) == 0){
			errorMessage = QStringLiteral("A role with this name already exists");

			return false;
		}
	}

	return true;
}


} // namespace imtauth


