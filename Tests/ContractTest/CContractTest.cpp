// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CContractTest.h"

// ImtCore includes
#include <imtauth/CContract.h>


namespace
{


imtauth::ContractInfo MakeSampleInfo()
{
	imtauth::ContractInfo info;
	info.contractId = "contract-1";
	info.relationshipId = "rel-1";
	info.sourceTenantId = "tenantA";
	info.targetTenantId = "tenantB";
	info.status = imtauth::CTS_ACTIVE;
	info.scope = "documents";
	info.validFrom = "2026-01-01T00:00:00.000Z";
	info.validUntil = "2030-01-01T00:00:00.000Z";
	info.description = "Sample contract";
	info.terms = "{\"payment\":\"monthly\"}";
	info.createdAt = "2026-01-01T00:00:00.000Z";
	info.updatedAt = "2026-01-02T00:00:00.000Z";
	return info;
}


} // anonymous namespace


void CContractTest::init()
{
	m_managerPtr = new imtauth::CMockContractManager();
}


void CContractTest::cleanup()
{
	delete m_managerPtr;
	m_managerPtr = nullptr;
}


void CContractTest::testContractInfo_RoundTrip()
{
	imtauth::CContract contract;
	imtauth::ContractInfo info = MakeSampleInfo();
	contract.SetContractInfo(info);

	QCOMPARE(contract.GetContractId(), QByteArray("contract-1"));
	QVERIFY(contract.GetContractInfo() == info);
}


void CContractTest::testContractInfo_CopyFromAndClone()
{
	imtauth::CContract source;
	source.SetContractInfo(MakeSampleInfo());

	imtauth::CContract target;
	QVERIFY(target.CopyFrom(source));
	QVERIFY(target.GetContractInfo() == source.GetContractInfo());

	istd::IChangeableUniquePtr clonePtr = source.CloneMe();
	QVERIFY(clonePtr.IsValid());
	const imtauth::IContractData* clonedPtr = dynamic_cast<const imtauth::IContractData*>(clonePtr.GetPtr());
	QVERIFY(clonedPtr != nullptr);
	QVERIFY(clonedPtr->GetContractInfo() == source.GetContractInfo());
}


void CContractTest::testContractInfo_ResetDataDefaults()
{
	imtauth::CContract contract;
	contract.SetContractInfo(MakeSampleInfo());

	QVERIFY(contract.ResetData());

	imtauth::ContractInfo info = contract.GetContractInfo();
	QVERIFY(info.contractId.isEmpty());
	QVERIFY(info.relationshipId.isEmpty());
	QVERIFY(info.sourceTenantId.isEmpty());
	QVERIFY(info.targetTenantId.isEmpty());
	QCOMPARE(info.status, imtauth::CTS_DRAFT);
}


void CContractTest::testCreateContract_Success()
{
	QByteArray contractId = m_managerPtr->CreateContract("rel-1", "tenantA", "tenantB");
	QVERIFY(!contractId.isEmpty());
	QCOMPARE(m_managerPtr->m_contracts.size(), 1);
	QCOMPARE(m_managerPtr->GetContract(contractId).status, imtauth::CTS_DRAFT);
}


void CContractTest::testCreateContract_MissingFields_Fails()
{
	QVERIFY(m_managerPtr->CreateContract("", "tenantA", "tenantB").isEmpty());
	QVERIFY(m_managerPtr->CreateContract("rel-1", "", "tenantB").isEmpty());
	QVERIFY(m_managerPtr->CreateContract("rel-1", "tenantA", "").isEmpty());
	QCOMPARE(m_managerPtr->m_contracts.size(), 0);
}


void CContractTest::testCreateContract_SameTenant_Fails()
{
	QVERIFY(m_managerPtr->CreateContract("rel-1", "tenantA", "tenantA").isEmpty());
	QCOMPARE(m_managerPtr->m_contracts.size(), 0);
}


void CContractTest::testUpdateContractStatus_ChangesStatus()
{
	QByteArray contractId = m_managerPtr->CreateContract("rel-1", "tenantA", "tenantB");
	QVERIFY(m_managerPtr->UpdateContractStatus(contractId, imtauth::CTS_ACTIVE));
	QCOMPARE(m_managerPtr->GetContract(contractId).status, imtauth::CTS_ACTIVE);
}


void CContractTest::testUpdateContractStatus_Unknown_ReturnsFalse()
{
	QVERIFY(!m_managerPtr->UpdateContractStatus("does-not-exist", imtauth::CTS_ACTIVE));
}


void CContractTest::testTerminateContract_SetsTerminated()
{
	QByteArray contractId = m_managerPtr->CreateContract("rel-1", "tenantA", "tenantB");
	QVERIFY(m_managerPtr->TerminateContract(contractId));
	QCOMPARE(m_managerPtr->GetContract(contractId).status, imtauth::CTS_TERMINATED);
}


void CContractTest::testGetContractsBySourceTenant()
{
	m_managerPtr->CreateContract("rel-1", "tenantA", "tenantB");
	m_managerPtr->CreateContract("rel-2", "tenantA", "tenantC");
	m_managerPtr->CreateContract("rel-3", "tenantX", "tenantB");

	QCOMPARE(m_managerPtr->GetContractsBySourceTenant("tenantA").size(), 2);
	QCOMPARE(m_managerPtr->GetContractsBySourceTenant("tenantX").size(), 1);
	QCOMPARE(m_managerPtr->GetContractsBySourceTenant("unknown").size(), 0);
}


void CContractTest::testGetContractsByTargetTenant()
{
	m_managerPtr->CreateContract("rel-1", "tenantA", "tenantB");
	m_managerPtr->CreateContract("rel-2", "tenantX", "tenantB");
	m_managerPtr->CreateContract("rel-3", "tenantA", "tenantC");

	QCOMPARE(m_managerPtr->GetContractsByTargetTenant("tenantB").size(), 2);
	QCOMPARE(m_managerPtr->GetContractsByTargetTenant("tenantC").size(), 1);
	QCOMPARE(m_managerPtr->GetContractsByTargetTenant("unknown").size(), 0);
}


void CContractTest::testGetContractsByRelationship()
{
	m_managerPtr->CreateContract("rel-1", "tenantA", "tenantB");
	m_managerPtr->CreateContract("rel-1", "tenantA", "tenantC");
	m_managerPtr->CreateContract("rel-2", "tenantA", "tenantD");

	QCOMPARE(m_managerPtr->GetContractsByRelationship("rel-1").size(), 2);
	QCOMPARE(m_managerPtr->GetContractsByRelationship("rel-2").size(), 1);
	QCOMPARE(m_managerPtr->GetContractsByRelationship("unknown").size(), 0);
}
