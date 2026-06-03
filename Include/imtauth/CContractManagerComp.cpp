// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtauth
{


QByteArray CContractManagerComp::CreateContract(
		const QByteArray& relationshipId,
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QString& scope,
		const QString& validFrom,
		const QString& validUntil,
		const QString& description,
		const QString& terms)
{
	if (!m_contractCollectionCompPtr.IsValid() || !m_contractFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Contract collection or factory not configured", "CContractManagerComp");
		return QByteArray();
	}

	if (relationshipId.isEmpty() || sourceTenantId.isEmpty() || targetTenantId.isEmpty()){
		SendErrorMessage(0, "Relationship, source tenant and target tenant are required", "CContractManagerComp");
		return QByteArray();
	}

	if (sourceTenantId == targetTenantId){
		SendErrorMessage(0, "Contract cannot reference the same tenant as source and target", "CContractManagerComp");
		return QByteArray();
	}

	IContractDataUniquePtr contractPtr = m_contractFactoryCompPtr.CreateInstance();
	if (!contractPtr.IsValid()){
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	QByteArray contractId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	ContractInfo info;
	info.contractId = contractId;
	info.relationshipId = relationshipId;
	info.sourceTenantId = sourceTenantId;
	info.targetTenantId = targetTenantId;
	info.status = CTS_DRAFT;
	info.scope = scope;
	info.validFrom = validFrom;
	info.validUntil = validUntil;
	info.description = description;
	info.terms = terms;
	info.createdAt = now;
	info.updatedAt = now;

	contractPtr->SetContractInfo(info);

	QByteArray storedId = m_contractCollectionCompPtr->InsertNewObject("Contract", QString(), QString(), contractPtr.GetPtr(), contractId);
	return storedId.isEmpty() ? QByteArray() : contractId;
}


bool CContractManagerComp::SetContractStatus(const QByteArray& contractId, ContractStatus status)
{
	if (!m_contractCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_contractCollectionCompPtr->GetObjectData(contractId, dataPtr)){
		return false;
	}

	IContractData* contractPtr = dynamic_cast<IContractData*>(dataPtr.GetPtr());
	if (contractPtr == nullptr){
		return false;
	}

	ContractInfo info = contractPtr->GetContractInfo();
	if (info.status == status){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	info.status = status;
	info.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	contractPtr->SetContractInfo(info);

	return m_contractCollectionCompPtr->SetObjectData(contractId, *contractPtr);
}


bool CContractManagerComp::UpdateContractStatus(const QByteArray& contractId, ContractStatus status)
{
	return SetContractStatus(contractId, status);
}


bool CContractManagerComp::TerminateContract(const QByteArray& contractId)
{
	return SetContractStatus(contractId, CTS_TERMINATED);
}


ContractInfo CContractManagerComp::GetContract(const QByteArray& contractId) const
{
	if (!m_contractCollectionCompPtr.IsValid()){
		return ContractInfo();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_contractCollectionCompPtr->GetObjectData(contractId, dataPtr)){
		return ContractInfo();
	}

	const IContractData* contractPtr = dynamic_cast<const IContractData*>(dataPtr.GetPtr());
	if (contractPtr == nullptr){
		return ContractInfo();
	}

	return contractPtr->GetContractInfo();
}


Contracts CContractManagerComp::CollectContracts(const std::function<bool(const ContractInfo&)>& predicate) const
{
	Contracts result;
	if (!m_contractCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_contractCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_contractCollectionCompPtr->GetObjectData(id, dataPtr)){
			const IContractData* contractPtr = dynamic_cast<const IContractData*>(dataPtr.GetPtr());
			if (contractPtr != nullptr){
				ContractInfo info = contractPtr->GetContractInfo();
				if (predicate(info)){
					result.append(info);
				}
			}
		}
	}

	return result;
}


Contracts CContractManagerComp::GetContractsBySourceTenant(const QByteArray& sourceTenantId) const
{
	return CollectContracts([&sourceTenantId](const ContractInfo& info){
		return info.sourceTenantId == sourceTenantId;
	});
}


Contracts CContractManagerComp::GetContractsByTargetTenant(const QByteArray& targetTenantId) const
{
	return CollectContracts([&targetTenantId](const ContractInfo& info){
		return info.targetTenantId == targetTenantId;
	});
}


Contracts CContractManagerComp::GetContractsByRelationship(const QByteArray& relationshipId) const
{
	return CollectContracts([&relationshipId](const ContractInfo& info){
		return info.relationshipId == relationshipId;
	});
}


} // namespace imtauth
