// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtauth/IContract.h>


namespace imtauth
{


/**
	Self-contained mock replicating the lifecycle semantics of
	CContractManagerComp, used to verify the expected behaviour of
	cooperation contracts without bringing up the component framework.
*/
class CMockContractManager
{
public:
	QByteArray CreateContract(
				const QByteArray& relationshipId,
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QString& scope = QString(),
				const QString& validFrom = QString(),
				const QString& validUntil = QString(),
				const QString& description = QString(),
				const QString& terms = QString())
	{
		if (relationshipId.isEmpty() || sourceTenantId.isEmpty() || targetTenantId.isEmpty()){
			return QByteArray();
		}
		if (sourceTenantId == targetTenantId){
			return QByteArray();
		}

		ContractInfo info;
		info.contractId = QByteArray::number(++m_counter);
		info.relationshipId = relationshipId;
		info.sourceTenantId = sourceTenantId;
		info.targetTenantId = targetTenantId;
		info.status = CTS_DRAFT;
		info.scope = scope;
		info.validFrom = validFrom;
		info.validUntil = validUntil;
		info.description = description;
		info.terms = terms;
		info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		info.updatedAt = info.createdAt;

		m_contracts.append(info);
		return info.contractId;
	}

	bool UpdateContractStatus(const QByteArray& contractId, ContractStatus status)
	{
		for (ContractInfo& info : m_contracts){
			if (info.contractId == contractId){
				info.status = status;
				info.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				return true;
			}
		}
		return false;
	}

	bool TerminateContract(const QByteArray& contractId)
	{
		return UpdateContractStatus(contractId, CTS_TERMINATED);
	}

	ContractInfo GetContract(const QByteArray& contractId) const
	{
		for (const ContractInfo& info : m_contracts){
			if (info.contractId == contractId){
				return info;
			}
		}
		return ContractInfo();
	}

	Contracts GetContractsBySourceTenant(const QByteArray& sourceTenantId) const
	{
		Contracts result;
		for (const ContractInfo& info : m_contracts){
			if (info.sourceTenantId == sourceTenantId){
				result.append(info);
			}
		}
		return result;
	}

	Contracts GetContractsByTargetTenant(const QByteArray& targetTenantId) const
	{
		Contracts result;
		for (const ContractInfo& info : m_contracts){
			if (info.targetTenantId == targetTenantId){
				result.append(info);
			}
		}
		return result;
	}

	Contracts GetContractsByRelationship(const QByteArray& relationshipId) const
	{
		Contracts result;
		for (const ContractInfo& info : m_contracts){
			if (info.relationshipId == relationshipId){
				result.append(info);
			}
		}
		return result;
	}

	QList<ContractInfo> m_contracts;
	int m_counter = 0;
};


} // namespace imtauth


class CContractTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Data object tests
	void testContractInfo_RoundTrip();
	void testContractInfo_CopyFromAndClone();
	void testContractInfo_ResetDataDefaults();

	// Manager semantics
	void testCreateContract_Success();
	void testCreateContract_MissingFields_Fails();
	void testCreateContract_SameTenant_Fails();
	void testUpdateContractStatus_ChangesStatus();
	void testUpdateContractStatus_Unknown_ReturnsFalse();
	void testTerminateContract_SetsTerminated();

	// Queries
	void testGetContractsBySourceTenant();
	void testGetContractsByTargetTenant();
	void testGetContractsByRelationship();

private:
	imtauth::CMockContractManager* m_managerPtr = nullptr;
};
