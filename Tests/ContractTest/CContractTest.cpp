// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CContractTest.h"


// STL includes
#include <algorithm>

// Qt includes
#include <QtCore/QTimeZone>

// ACF includes
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CContractEventInfo.h>
#include <imtauth/CContractInfo.h>
#include <imtauth/CContractTypeInfo.h>
#include <imtauth/ContractState.h>


namespace
{


const QDateTime s_start = QDateTime(QDate(2026, 1, 1), QTime(12, 0), QTimeZone::UTC);


QDateTime Day(int days)
{
	return s_start.addDays(days);
}


imtauth::IContractInfo::Party MakeParty(const QByteArray& tenantId, imtauth::IContractInfo::PartyState state)
{
	imtauth::IContractInfo::Party party;
	party.tenantId = tenantId;
	party.tenantName = QString::fromUtf8(tenantId);
	party.roleId = "Partner";
	party.state = state;

	return party;
}


imtauth::IContractInfo::Link MakeLink(const QByteArray& linkId, const QByteArray& tenantAId, const QByteArray& tenantBId)
{
	imtauth::IContractInfo::Link link;
	link.linkId = linkId;
	link.tenantAId = tenantAId;
	link.tenantBId = tenantBId;
	link.roleAId = "Partner";
	link.roleBId = "Partner";

	return link;
}


/**
	A contract of the parties A, B and C linked A-B and B-C, submitted at day 0; signed by all at day 0 when asked.
*/
void FillContract(imtauth::IContractInfo& contract, bool isSigned, int partyCount = 3)
{
	const imtauth::IContractInfo::PartyState state = isSigned ? imtauth::IContractInfo::PS_SIGNED : imtauth::IContractInfo::PS_REVIEWING;

	imtauth::IContractInfo::Parties parties = {MakeParty("A", state), MakeParty("B", state)};
	imtauth::IContractInfo::Links links = {MakeLink("AB", "A", "B")};
	if (partyCount > 2){
		parties.append(MakeParty("C", state));
		links.append(MakeLink("BC", "B", "C"));
	}

	contract.SetContractId("contract");
	contract.SetName("Supply");
	contract.SetTerms("Terms");
	contract.SetInitiatorTenantId("A");
	contract.SetParties(parties);
	contract.SetLinks(links);
	contract.SetSubmittedAt(s_start);
	if (isSigned){
		contract.SetSignedAt(s_start);
	}
}


void SetPartyState(imtauth::IContractInfo& contract, const QByteArray& tenantId, imtauth::IContractInfo::PartyState state, const QDateTime& withdrawAt = QDateTime())
{
	imtauth::IContractInfo::Parties parties = contract.GetParties();
	const int index = imtauth::FindContractParty(parties, tenantId);
	QVERIFY(index >= 0);
	parties[index].state = state;
	parties[index].withdrawAt = withdrawAt;
	contract.SetParties(parties);
}


} // anonymous namespace


// State of the contract

void CContractTest::testState_data()
{
	QTest::addColumn<QByteArray>("change");
	QTest::addColumn<int>("now");
	QTest::addColumn<int>("state");
	QTest::addColumn<int>("endedAt");

	QTest::newRow("draft") << QByteArray("draft") << 0 << int(imtauth::CS_DRAFT) << -1;
	QTest::newRow("negotiation") << QByteArray("negotiation") << 0 << int(imtauth::CS_NEGOTIATION) << -1;
	QTest::newRow("a party signed") << QByteArray("one signed") << 0 << int(imtauth::CS_SIGNING) << -1;
	QTest::newRow("a party declined") << QByteArray("declined") << 0 << int(imtauth::CS_REJECTED) << -1;
	QTest::newRow("cancelled") << QByteArray("cancelled") << 3 << int(imtauth::CS_CANCELLED) << 1;
	QTest::newRow("signed, not in force yet") << QByteArray("effective later") << 1 << int(imtauth::CS_SIGNED) << -1;
	QTest::newRow("in force") << QByteArray("signed") << 1 << int(imtauth::CS_ACTIVE) << -1;
	QTest::newRow("term ended") << QByteArray("term") << 31 << int(imtauth::CS_EXPIRED) << 30;
	QTest::newRow("terminated") << QByteArray("terminated") << 6 << int(imtauth::CS_TERMINATED) << 5;
	QTest::newRow("superseded") << QByteArray("superseded") << 4 << int(imtauth::CS_SUPERSEDED) << 3;
	QTest::newRow("replaced and terminated at once") << QByteArray("superseded and terminated") << 4 << int(imtauth::CS_SUPERSEDED) << 3;
	QTest::newRow("the other of two parties left") << QByteArray("two parties, one left") << 6 << int(imtauth::CS_TERMINATED) << 5;
	QTest::newRow("a third party left") << QByteArray("three parties, one left") << 6 << int(imtauth::CS_ACTIVE) << -1;
	QTest::newRow("the middle party left") << QByteArray("three parties, middle left") << 6 << int(imtauth::CS_TERMINATED) << 5;
}


void CContractTest::testState()
{
	QFETCH(QByteArray, change);
	QFETCH(int, now);
	QFETCH(int, state);
	QFETCH(int, endedAt);

	const bool isSigned = !change.startsWith("draft") && change != "negotiation" && change != "one signed" && change != "declined"
				&& change != "cancelled";

	imtauth::CContractInfo contract;
	FillContract(contract, isSigned, change == "two parties, one left" ? 2 : 3);

	if (change == "draft"){
		contract.SetSubmittedAt(QDateTime());
	}
	else if (change == "one signed"){
		SetPartyState(contract, "B", imtauth::IContractInfo::PS_SIGNED);
	}
	else if (change == "declined"){
		SetPartyState(contract, "B", imtauth::IContractInfo::PS_SIGNED);
		SetPartyState(contract, "C", imtauth::IContractInfo::PS_DECLINED);
	}
	else if (change == "cancelled"){
		contract.SetCancelledAt(Day(1));
	}
	else if (change == "effective later"){
		contract.SetEffectiveFrom(Day(10));
	}
	else if (change == "term"){
		contract.SetTermDays(30);
	}
	else if (change == "terminated"){
		contract.SetTerminatedAt(Day(5));
	}
	else if (change == "superseded"){
		contract.SetSupersededBy("next");
		contract.SetSupersededAt(Day(3));
	}
	else if (change == "superseded and terminated"){
		contract.SetSupersededBy("next");
		contract.SetSupersededAt(Day(3));
		contract.SetTerminatedAt(Day(3));
	}
	else if (change == "two parties, one left" || change == "three parties, one left"){
		SetPartyState(contract, change.startsWith("two") ? "B" : "C", imtauth::IContractInfo::PS_WITHDRAWN, Day(5));
	}
	else if (change == "three parties, middle left"){
		// A and C have no link of their own
		SetPartyState(contract, "B", imtauth::IContractInfo::PS_NOTICE_GIVEN, Day(5));
	}

	const imtauth::ContractStateInfo stateInfo = imtauth::ComputeContractState(contract, Day(now));
	QCOMPARE(int(stateInfo.state), state);
	QCOMPARE(stateInfo.endedAt, endedAt >= 0 ? Day(endedAt) : QDateTime());
}


void CContractTest::testState_PeriodEndOfARenewingContract()
{
	imtauth::CContractInfo contract;
	FillContract(contract, true);
	contract.SetTermDays(30);
	contract.SetAutoRenew(true);
	contract.SetRenewalPeriodDays(30);
	contract.SetRenewalNoticeDays(10);

	// Without a notice the contract renews: the period in progress ends
	imtauth::ContractStateInfo stateInfo = imtauth::ComputeContractState(contract, Day(45));
	QCOMPARE(int(stateInfo.state), int(imtauth::CS_ACTIVE));
	QCOMPARE(stateInfo.periodEnd, Day(60));

	// A notice in time ends it with the current period
	imtauth::IContractInfo::Parties parties = contract.GetParties();
	parties[1].nonRenewalNoticeAt = Day(15);
	contract.SetParties(parties);
	QCOMPARE(imtauth::ComputeContractState(contract, Day(20)).periodEnd, Day(30));
	stateInfo = imtauth::ComputeContractState(contract, Day(31));
	QCOMPARE(int(stateInfo.state), int(imtauth::CS_EXPIRED));
	QCOMPARE(stateInfo.endedAt, Day(30));

	// A late notice ends it with the next one
	parties[1].nonRenewalNoticeAt = Day(25);
	contract.SetParties(parties);
	QCOMPARE(imtauth::ComputeContractState(contract, Day(31)).periodEnd, Day(60));
	QCOMPARE(int(imtauth::ComputeContractState(contract, Day(61)).state), int(imtauth::CS_EXPIRED));
}


void CContractTest::testState_ExpiringAndTerminatingFlags()
{
	imtauth::CContractInfo contract;
	FillContract(contract, true);
	contract.SetTermDays(90);

	QCOMPARE(imtauth::ComputeContractState(contract, Day(10)).flags & imtauth::CSF_EXPIRING, 0);
	QVERIFY(imtauth::ComputeContractState(contract, Day(70)).flags & imtauth::CSF_EXPIRING);

	// The third party leaves later: the two others still hold the contract, it does not end
	SetPartyState(contract, "C", imtauth::IContractInfo::PS_NOTICE_GIVEN, Day(20));
	const imtauth::ContractStateInfo goesOn = imtauth::ComputeContractState(contract, Day(10));
	QCOMPARE(int(goesOn.state), int(imtauth::CS_ACTIVE));
	QCOMPARE(goesOn.flags & imtauth::CSF_TERMINATING, 0);

	// One of two parties leaves later: the contract goes on until then and ends with it
	imtauth::CContractInfo pair;
	FillContract(pair, true, 2);
	SetPartyState(pair, "B", imtauth::IContractInfo::PS_NOTICE_GIVEN, Day(20));
	const imtauth::ContractStateInfo ending = imtauth::ComputeContractState(pair, Day(10));
	QCOMPARE(int(ending.state), int(imtauth::CS_ACTIVE));
	QVERIFY(ending.flags & imtauth::CSF_TERMINATING);
	QCOMPARE(ending.endsAt, Day(20));

	// Changes requested in the negotiation
	imtauth::CContractInfo negotiated;
	FillContract(negotiated, false);
	SetPartyState(negotiated, "B", imtauth::IContractInfo::PS_CHANGES_REQUESTED);
	QVERIFY(imtauth::ComputeContractState(negotiated, Day(0)).flags & imtauth::CSF_CHANGES_REQUESTED);
}


// What is in force

void CContractTest::testLink_InForceWithinItsPeriodAndBoundParties()
{
	imtauth::CContractInfo contract;
	FillContract(contract, true);

	imtauth::IContractInfo::Links links = contract.GetLinks();
	links[0].validFrom = Day(5);
	links[0].validUntil = Day(10);
	contract.SetLinks(links);

	QVERIFY(!imtauth::IsLinkInForce(contract, contract.GetLinks()[0], Day(4)));
	QVERIFY(imtauth::IsLinkInForce(contract, contract.GetLinks()[0], Day(5)));
	QVERIFY(!imtauth::IsLinkInForce(contract, contract.GetLinks()[0], Day(10)));

	// The link of a party that left ends with it, the other links stay
	SetPartyState(contract, "C", imtauth::IContractInfo::PS_WITHDRAWN, Day(6));
	QVERIFY(imtauth::IsLinkInForce(contract, contract.GetLinks()[0], Day(7)));
	QVERIFY(imtauth::IsLinkInForce(contract, contract.GetLinks()[1], Day(5)));
	QVERIFY(!imtauth::IsLinkInForce(contract, contract.GetLinks()[1], Day(7)));

	// Nothing of a contract that is not signed is in force
	imtauth::CContractInfo negotiated;
	FillContract(negotiated, false);
	QVERIFY(!imtauth::IsLinkInForce(negotiated, negotiated.GetLinks()[1], Day(7)));
}


void CContractTest::testAllocation_InForceOnItsLinkWhileTheGrantorIsBound()
{
	imtauth::CContractInfo contract;
	FillContract(contract, true);

	imtauth::IContractInfo::Allocation allocation;
	allocation.allocationId = "allocation";
	allocation.linkId = "AB";
	allocation.grantorTenantId = "A";
	allocation.granteeTenantId = "B";

	allocation.permissionIds = {"Read"};
	allocation.validUntil = Day(20);
	contract.SetAllocations({allocation});

	QVERIFY(imtauth::IsAllocationInForce(contract, allocation, Day(1)));
	QVERIFY(!imtauth::IsAllocationInForce(contract, allocation, Day(20)));

	// The link of the allocation connects its grantor and its grantee only
	imtauth::IContractInfo::Allocation foreign = allocation;
	foreign.granteeTenantId = "C";
	QVERIFY(!imtauth::IsAllocationInForce(contract, foreign, Day(1)));

	// The permissions end when the grantor leaves the contract
	imtauth::IContractInfo::Parties parties = contract.GetParties();
	imtauth::IContractInfo::Party& grantor = parties[imtauth::FindContractParty(parties, "A")];
	grantor.state = imtauth::IContractInfo::PS_WITHDRAWN;
	grantor.withdrawAt = Day(5);
	contract.SetParties(parties);
	QVERIFY(imtauth::IsAllocationInForce(contract, allocation, Day(4)));
	QVERIFY(!imtauth::IsAllocationInForce(contract, allocation, Day(6)));
}


void CContractTest::testPartyBound_UntilItLeaves()
{
	imtauth::CContractInfo contract;
	FillContract(contract, true);
	SetPartyState(contract, "C", imtauth::IContractInfo::PS_NOTICE_GIVEN, Day(5));

	const imtauth::IContractInfo::Party leaving = contract.GetParties()[2];
	QVERIFY(imtauth::IsPartyBoundAt(contract, leaving, Day(4)));
	QVERIFY(!imtauth::IsPartyBoundAt(contract, leaving, Day(5)));

	// A party is bound by a signed contract only
	imtauth::CContractInfo negotiated;
	FillContract(negotiated, false);
	QVERIFY(!imtauth::IsPartyBoundAt(negotiated, negotiated.GetParties()[0], Day(1)));
}


// Content hash

void CContractTest::testContentHash_IgnoresOrderAndProgress()
{
	imtauth::CContractInfo contract;
	FillContract(contract, false);
	const QByteArray hash = imtauth::ComputeContractContentHash(contract);
	QCOMPARE(hash.size(), 64);

	// The parties in another order, their states and signatures: the same terms
	imtauth::CContractInfo reordered;
	FillContract(reordered, true);
	imtauth::IContractInfo::Parties parties = reordered.GetParties();
	std::reverse(parties.begin(), parties.end());
	imtauth::IContractInfo::Signature signature;
	signature.userId = "user";
	signature.signedAt = s_start;
	parties[0].signatures = {signature};
	reordered.SetParties(parties);
	QCOMPARE(imtauth::ComputeContractContentHash(reordered), hash);

	// A replaced file and a file attached after the conclusion are no terms
	imtauth::IContractInfo::Attachment copy;
	copy.attachmentId = "copy";
	copy.addedAt = Day(1);
	copy.contentHash = "1111";
	imtauth::IContractInfo::Attachment replaced;
	replaced.attachmentId = "old";
	replaced.addedAt = Day(-1);
	replaced.contentHash = "2222";
	replaced.isReplaced = true;
	reordered.SetAttachments({copy, replaced});
	QCOMPARE(imtauth::ComputeContractContentHash(reordered), hash);

	// The permissions a party gives are its own decision, not a term
	imtauth::IContractInfo::Allocation allocation;
	allocation.allocationId = "allocation";
	allocation.linkId = "AB";
	allocation.grantorTenantId = "A";
	allocation.granteeTenantId = "B";

	allocation.permissionIds = {"Read"};
	reordered.SetAllocations({allocation});
	QCOMPARE(imtauth::ComputeContractContentHash(reordered), hash);
}


void CContractTest::testContentHash_FollowsTheTerms()
{
	imtauth::CContractInfo contract;
	FillContract(contract, false);
	const QByteArray hash = imtauth::ComputeContractContentHash(contract);

	imtauth::CContractInfo changed;
	FillContract(changed, false);
	changed.SetTerms("Other terms");
	QVERIFY(imtauth::ComputeContractContentHash(changed) != hash);

	FillContract(changed, false);
	imtauth::IContractInfo::Links links = changed.GetLinks();
	links[0].isSharingAllowed = !links[0].isSharingAllowed;
	changed.SetLinks(links);
	QVERIFY(imtauth::ComputeContractContentHash(changed) != hash);

	FillContract(changed, false);
	imtauth::IContractInfo::Attachment text;
	text.attachmentId = "text";
	text.contentHash = "3333";
	changed.SetAttachments({text});
	const QByteArray textHash = imtauth::ComputeContractContentHash(changed);
	QVERIFY(textHash != hash);
	text.contentHash = "4444";
	changed.SetAttachments({text});
	QVERIFY(imtauth::ComputeContractContentHash(changed) != textHash);
}


// Serialization

void CContractTest::testContractInfo_SerializeRoundTrip()
{
	imtauth::CContractInfo source;
	FillContract(source, true);
	source.SetProductId("ProLife");
	source.SetTermDays(365);
	source.SetAutoRenew(true);
	source.SetRenewalPeriodDays(365);

	imtauth::IContractInfo::Allocation allocation;
	allocation.allocationId = "allocation";
	allocation.linkId = "AB";
	allocation.grantorTenantId = "A";
	allocation.granteeTenantId = "B";
	allocation.permissionIds = {"Read", "Write"};
	source.SetAllocations({allocation});

	imtauth::IContractInfo::Reference reference;
	reference.contractId = "framework";
	reference.type = imtauth::IContractInfo::RT_FRAMEWORK;
	source.SetReferences({reference});

	iser::CMemoryWriteArchive writeArchive;
	QVERIFY(source.Serialize(writeArchive));

	imtauth::CContractInfo target;
	iser::CMemoryReadArchive readArchive(writeArchive.GetBuffer(), writeArchive.GetBufferSize());
	QVERIFY(target.Serialize(readArchive));
	QVERIFY(target.IsEqual(source));
	QCOMPARE(target.GetProductId(), QByteArray("ProLife"));
	imtauth::CContractInfo copied;
	QVERIFY(copied.CopyFrom(source));
	QCOMPARE(copied.GetProductId(), QByteArray("ProLife"));
	copied.SetProductId("Puma");
	QVERIFY(!copied.IsEqual(source));
	QCOMPARE(imtauth::ComputeContractContentHash(copied), imtauth::ComputeContractContentHash(source));
	QCOMPARE(target.GetAllocations().first().permissionIds, QByteArrayList({"Read", "Write"}));
	QCOMPARE(int(target.GetReferences().first().type), int(imtauth::IContractInfo::RT_FRAMEWORK));
	QCOMPARE(imtauth::ComputeContractContentHash(target), imtauth::ComputeContractContentHash(source));
}


void CContractTest::testContractTypeInfo_SerializeRoundTrip()
{
	imtauth::CContractTypeInfo source;
	source.SetTypeId("supply");
	source.SetName("Supply");
	source.SetLinkScheme(imtauth::IContractTypeInfo::LS_STAR);
	source.SetStarRoleId("customer");
	source.SetTermDays(365);

	imtauth::IContractInfo::RoleSlot slot;
	slot.roleId = "supplier";
	slot.minCount = 1;
	slot.maxCount = 2;
	source.SetRoleSlots({slot});

	imtauth::IContractTypeInfo::AllocationTemplate allocationTemplate;
	allocationTemplate.grantorRoleId = "supplier";
	allocationTemplate.granteeRoleId = "customer";

	allocationTemplate.permissionIds = {"Read", "Write"};
	source.SetAllocationTemplates({allocationTemplate});

	iser::CMemoryWriteArchive writeArchive;
	QVERIFY(source.Serialize(writeArchive));

	imtauth::CContractTypeInfo target;
	iser::CMemoryReadArchive readArchive(writeArchive.GetBuffer(), writeArchive.GetBufferSize());
	QVERIFY(target.Serialize(readArchive));
	QVERIFY(target.IsEqual(source));
	QCOMPARE(int(target.GetLinkScheme()), int(imtauth::IContractTypeInfo::LS_STAR));
	QCOMPARE(target.GetAllocationTemplates().first().permissionIds, QByteArrayList({"Read", "Write"}));
}


void CContractTest::testContractEventInfo_SerializeRoundTrip()
{
	imtauth::CContractEventInfo source;
	source.SetEventId("event");
	source.SetContractId("contract");
	source.SetEventType(imtauth::IContractEventInfo::ET_SIGNED);
	source.SetUserName("Ann");
	source.SetTime(s_start);
	source.SetRevision(3);
	source.SetPrivate(true);

	iser::CMemoryWriteArchive writeArchive;
	QVERIFY(source.Serialize(writeArchive));

	imtauth::CContractEventInfo target;
	iser::CMemoryReadArchive readArchive(writeArchive.GetBuffer(), writeArchive.GetBufferSize());
	QVERIFY(target.Serialize(readArchive));
	QVERIFY(target.IsEqual(source));
	QCOMPARE(int(target.GetEventType()), int(imtauth::IContractEventInfo::ET_SIGNED));
	QCOMPARE(target.GetTime(), s_start);
}


I_ADD_TEST(CContractTest);


