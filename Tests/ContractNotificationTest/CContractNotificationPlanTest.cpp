// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CContractNotificationPlanTest.h"


// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CContractInfo.h>
#include <imtauth/TenantEvents.h>
#include <imtauthgql/ContractNotificationPlan.h>
#include <imtauthgql/TenantNotifications.h>


namespace
{


namespace te = imtauth::TenantEvent;
namespace tn = imtauthgql::TenantNotification;
namespace plan = imtauthgql::ContractNotificationPlan;


const QByteArray s_pending = "contract-pending:contract";
const QByteArray s_signed = "contract-signed:contract";


/**
	A contract of the parties A (the initiator), B, C and D.
*/
void FillContract(imtauth::IContractInfo& contract)
{
	imtauth::IContractInfo::Parties parties;
	for (const QByteArray& tenantId : {"A", "B", "C", "D"}){
		imtauth::IContractInfo::Party party;
		party.tenantId = tenantId;
		party.tenantName = QString::fromUtf8(tenantId);
		party.roleId = "Partner";
		party.state = imtauth::IContractInfo::PS_REVIEWING;
		parties.append(party);
	}

	contract.SetContractId("contract");
	contract.SetName("Supply");
	contract.SetInitiatorTenantId("A");
	contract.SetParties(parties);
}


QVariantMap Event(const QString& type, const QByteArray& actorTenantId, const QStringList& recipientTenantIds, const QByteArray& reminderKey = QByteArray())
{
	QVariantMap retVal = {
		{te::s_type, type},
		{te::s_contractId, QByteArray("contract")},
		{te::s_sourceTenantId, actorTenantId},
		{te::s_recipientTenantIds, recipientTenantIds}};
	if (!reminderKey.isEmpty()){
		retVal.insert(te::s_reminderKey, reminderKey);
	}

	return retVal;
}


plan::Plan CreatePlan(const QVariantMap& event)
{
	imtauth::CContractInfo contract;
	FillContract(contract);

	return plan::CreatePlan(event, contract);
}


// "<tenant> <group key>"
QStringList Withdrawals(const plan::Plan& result)
{
	QStringList retVal;
	for (const plan::Withdrawal& withdrawal : result.withdrawals){
		retVal.append(QString::fromUtf8(withdrawal.tenantId + " " + withdrawal.groupKey));
	}

	return retVal;
}


// "<tenant> <type> group=<key> partner=<tenant>", with " unlinked" and " date=<time>" where they apply
QStringList Deliveries(const plan::Plan& result)
{
	QStringList retVal;
	for (const plan::Delivery& delivery : result.deliveries){
		QString text = QString::fromUtf8(delivery.tenantId + " " + delivery.typeId + " group=" + delivery.groupKey + " partner=" + delivery.partnerTenantId);
		if (!delivery.isSubjectLinked){
			text += QStringLiteral(" unlinked");
		}

		if (delivery.date.isValid()){
			text += QStringLiteral(" date=") + delivery.date.toUTC().toString(Qt::ISODate);
		}

		retVal.append(text);
	}

	return retVal;
}


QString Delivery(const QByteArray& tenantId, const QByteArray& typeId, const QByteArray& groupKey, const QByteArray& partnerTenantId)
{
	return QString::fromUtf8(tenantId + " " + typeId + " group=" + groupKey + " partner=" + partnerTenantId);
}


} // anonymous namespace


// Negotiation

void CContractNotificationPlanTest::testInvitation_PendingForEveryInvitedParty()
{
	const plan::Plan result = CreatePlan(Event(te::s_contractInvitationReceived, "A", {"B", "C"}));

	QCOMPARE(Withdrawals(result), QStringList());
	QCOMPARE(Deliveries(result), QStringList({
				Delivery("B", tn::s_contractInvitationReceived, s_pending, "A"),
				Delivery("C", tn::s_contractInvitationReceived, s_pending, "A")}));
	QCOMPARE(result.deliveries[0].severity, istd::IInformationProvider::IC_INFO);
}


void CContractNotificationPlanTest::testSigned_TellsTheInitiatorInOneGroup()
{
	const plan::Plan result = CreatePlan(Event(te::s_contractSigned, "B", {"A"}));

	QCOMPARE(Deliveries(result), QStringList({Delivery("A", tn::s_contractSigned, s_signed, "B")}));
}


void CContractNotificationPlanTest::testSigned_AnswersTheInvitationOfTheSigner()
{
	const plan::Plan result = CreatePlan(Event(te::s_contractSigned, "B", {"A"}));

	QCOMPARE(Withdrawals(result), QStringList({"B " + QString::fromUtf8(s_pending)}));
}


void CContractNotificationPlanTest::testSigned_ByTheInitiatorKeepsTheChangesAskedOfIt()
{
	// The initiator signs while a party waits for changes: what it was asked stays
	const plan::Plan result = CreatePlan(Event(te::s_contractSigned, "A", {}));

	QCOMPARE(Withdrawals(result), QStringList());
	QCOMPARE(Deliveries(result), QStringList());
}


void CContractNotificationPlanTest::testChangesRequested_AnswersTheInvitationAndAsksTheInitiator()
{
	const plan::Plan result = CreatePlan(Event(te::s_contractChangesRequested, "C", {"A"}));

	QCOMPARE(Withdrawals(result), QStringList({"C " + QString::fromUtf8(s_pending)}));
	QCOMPARE(Deliveries(result), QStringList({Delivery("A", tn::s_contractChangesRequested, s_pending, "C")}));
	QCOMPARE(result.deliveries[0].severity, istd::IInformationProvider::IC_WARNING);
}


void CContractNotificationPlanTest::testTermsChanged_ReplacesWhatWasPendingAndTheSignatures()
{
	const plan::Plan result = CreatePlan(Event(te::s_contractTermsChanged, "A", {"B", "C"}));

	// The initiator answered and lost its signatures; an unread invitation of a party is replaced, not counted twice
	QCOMPARE(Withdrawals(result), QStringList({
				"A " + QString::fromUtf8(s_pending),
				"A " + QString::fromUtf8(s_signed),
				"B " + QString::fromUtf8(s_pending),
				"C " + QString::fromUtf8(s_pending)}));
	QCOMPARE(Deliveries(result), QStringList({
				Delivery("B", tn::s_contractTermsChanged, s_pending, "A"),
				Delivery("C", tn::s_contractTermsChanged, s_pending, "A")}));
}


void CContractNotificationPlanTest::testPartyRemoved_WithdrawsItsInvitationAndHidesTheContract()
{
	const plan::Plan result = CreatePlan(Event(te::s_contractPartyRemoved, "A", {"D"}));

	QCOMPARE(Withdrawals(result), QStringList({"D " + QString::fromUtf8(s_pending)}));
	QCOMPARE(Deliveries(result), QStringList({Delivery("D", tn::s_contractPartyRemoved, QByteArray(), "A") + " unlinked"}));
}


void CContractNotificationPlanTest::testNegotiationEnd_WithdrawsAllPendingAndTheSignatures_data()
{
	QTest::addColumn<QString>("eventType");
	QTest::addColumn<QByteArray>("actorTenantId");
	QTest::addColumn<QByteArray>("typeId");

	QTest::newRow("declined") << te::s_contractDeclined << QByteArray("B") << tn::s_contractDeclined;
	QTest::newRow("cancelled") << te::s_contractCancelled << QByteArray("A") << tn::s_contractCancelled;
	QTest::newRow("concluded") << te::s_contractConcluded << QByteArray() << tn::s_contractConcluded;
}


void CContractNotificationPlanTest::testNegotiationEnd_WithdrawsAllPendingAndTheSignatures()
{
	QFETCH(QString, eventType);
	QFETCH(QByteArray, actorTenantId);
	QFETCH(QByteArray, typeId);

	const plan::Plan result = CreatePlan(Event(eventType, actorTenantId, {"C", "D"}));

	QCOMPARE(Withdrawals(result), QStringList({
				"A " + QString::fromUtf8(s_pending),
				"B " + QString::fromUtf8(s_pending),
				"C " + QString::fromUtf8(s_pending),
				"D " + QString::fromUtf8(s_pending),
				"A " + QString::fromUtf8(s_signed)}));
	QCOMPARE(Deliveries(result), QStringList({
				Delivery("C", typeId, QByteArray(), actorTenantId),
				Delivery("D", typeId, QByteArray(), actorTenantId)}));
}


// A concluded contract

void CContractNotificationPlanTest::testNotice_GroupedPerLeavingParty()
{
	const plan::Plan result = CreatePlan(Event(te::s_contractNoticeGiven, "C", {"A", "B"}));

	QCOMPARE(Withdrawals(result), QStringList());
	QCOMPARE(Deliveries(result), QStringList({
				Delivery("A", tn::s_contractNoticeGiven, "contract-notice:contract:C", "C"),
				Delivery("B", tn::s_contractNoticeGiven, "contract-notice:contract:C", "C")}));
	QCOMPARE(plan::GetNoticeGroupKey("contract", "C"), QByteArray("contract-notice:contract:C"));
	QVERIFY(plan::GetNoticeGroupKey("contract", "B") != plan::GetNoticeGroupKey("contract", "C"));
}


void CContractNotificationPlanTest::testNoticeRevoked_WithdrawsTheNoticeEverywhere()
{
	const plan::Plan result = CreatePlan(Event(te::s_contractNoticeRevoked, "C", {"A", "B"}));

	QCOMPARE(Withdrawals(result), QStringList({
				"A contract-notice:contract:C",
				"B contract-notice:contract:C",
				"C contract-notice:contract:C",
				"D contract-notice:contract:C"}));
	QCOMPARE(Deliveries(result), QStringList({
				Delivery("A", tn::s_contractNoticeRevoked, QByteArray(), "C"),
				Delivery("B", tn::s_contractNoticeRevoked, QByteArray(), "C")}));
}


void CContractNotificationPlanTest::testInformation_NeitherGroupedNorWithdrawing_data()
{
	QTest::addColumn<QString>("eventType");
	QTest::addColumn<QByteArray>("typeId");
	QTest::addColumn<int>("severity");

	QTest::newRow("left") << te::s_contractWithdrawn << tn::s_contractWithdrawn << int(istd::IInformationProvider::IC_WARNING);
	QTest::newRow("renewal stopped") << te::s_contractRenewalStopped << tn::s_contractRenewalStopped << int(istd::IInformationProvider::IC_WARNING);
	QTest::newRow("permissions received") << te::s_contractPermissionsReceived << tn::s_contractPermissionsReceived << int(istd::IInformationProvider::IC_INFO);
	QTest::newRow("permissions taken back") << te::s_contractPermissionsWithdrawn << tn::s_contractPermissionsWithdrawn << int(istd::IInformationProvider::IC_WARNING);
	QTest::newRow("superseded") << te::s_contractSuperseded << tn::s_contractSuperseded << int(istd::IInformationProvider::IC_INFO);
}


void CContractNotificationPlanTest::testInformation_NeitherGroupedNorWithdrawing()
{
	QFETCH(QString, eventType);
	QFETCH(QByteArray, typeId);
	QFETCH(int, severity);

	const plan::Plan result = CreatePlan(Event(eventType, "B", {"A", "C"}));

	QCOMPARE(Withdrawals(result), QStringList());
	QCOMPARE(Deliveries(result), QStringList({
				Delivery("A", typeId, QByteArray(), "B"),
				Delivery("C", typeId, QByteArray(), "B")}));
	QCOMPARE(int(result.deliveries[0].severity), severity);
}


// Reminders

void CContractNotificationPlanTest::testReminder_NamesItsDate_data()
{
	QTest::addColumn<QByteArray>("reminderKey");
	QTest::addColumn<QString>("expected");

	QTest::newRow("end") << QByteArray("end:2027-03-01T10:00:00Z")
				<< Delivery("B", tn::s_contractEndsSoon, QByteArray(), QByteArray()) + " date=2027-03-01T10:00:00Z";
	QTest::newRow("renewal") << QByteArray("renewal:2027-02-01T10:00:00Z")
				<< Delivery("B", tn::s_contractRenewalDeadline, QByteArray(), QByteArray()) + " date=2027-02-01T10:00:00Z";
}


void CContractNotificationPlanTest::testReminder_NamesItsDate()
{
	QFETCH(QByteArray, reminderKey);
	QFETCH(QString, expected);

	const plan::Plan result = CreatePlan(Event(te::s_contractReminder, QByteArray(), {"B"}, reminderKey));

	QCOMPARE(Withdrawals(result), QStringList());
	QCOMPARE(Deliveries(result), QStringList({expected}));
}


void CContractNotificationPlanTest::testReminder_WithoutAKnownDateTellsNothing_data()
{
	QTest::addColumn<QByteArray>("reminderKey");

	QTest::newRow("no key") << QByteArray();
	QTest::newRow("no date") << QByteArray("end");
	QTest::newRow("bad date") << QByteArray("end:tomorrow");
	QTest::newRow("unknown date") << QByteArray("signing:2027-03-01T10:00:00Z");
}


void CContractNotificationPlanTest::testReminder_WithoutAKnownDateTellsNothing()
{
	QFETCH(QByteArray, reminderKey);

	const plan::Plan result = CreatePlan(Event(te::s_contractReminder, QByteArray(), {"B"}, reminderKey));

	QCOMPARE(Deliveries(result), QStringList());
}


// Events that tell nothing

void CContractNotificationPlanTest::testNothingTold_data()
{
	QTest::addColumn<QString>("eventType");

	// The change of a contract only refreshes the open views
	QTest::newRow("changed") << te::s_contractChanged;
	QTest::newRow("unknown") << QStringLiteral("ContractSomethingElse");
}


void CContractNotificationPlanTest::testNothingTold()
{
	QFETCH(QString, eventType);

	const plan::Plan result = CreatePlan(Event(eventType, "A", {"B", "C"}));

	QCOMPARE(Withdrawals(result), QStringList());
	QCOMPARE(Deliveries(result), QStringList());
}


void CContractNotificationPlanTest::testEmptyRecipientsAreSkipped()
{
	const plan::Plan result = CreatePlan(Event(te::s_contractTermsChanged, "A", {"", "B"}));

	QCOMPARE(Deliveries(result), QStringList({Delivery("B", tn::s_contractTermsChanged, s_pending, "A")}));
	QVERIFY(!Withdrawals(result).contains(" " + QString::fromUtf8(s_pending)));
}


// Recipients of a party

void CContractNotificationPlanTest::testSelectRecipients_data()
{
	QTest::addColumn<QByteArray>("responsibleUserId");
	QTest::addColumn<bool>("isResponsibleMember");
	QTest::addColumn<QByteArray>("actorUserId");
	QTest::addColumn<QByteArrayList>("expected");

	QTest::newRow("no responsible user: the owner") << QByteArray() << false << QByteArray("x") << QByteArrayList({"owner"});
	QTest::newRow("the responsible user instead of the owner") << QByteArray("resp") << true << QByteArray("x") << QByteArrayList({"resp"});
	QTest::newRow("a responsible user who left: the owner") << QByteArray("resp") << false << QByteArray("x") << QByteArrayList({"owner"});
	QTest::newRow("the responsible user acted") << QByteArray("resp") << true << QByteArray("resp") << QByteArrayList();
	QTest::newRow("the owner acted") << QByteArray() << false << QByteArray("owner") << QByteArrayList();
	QTest::newRow("the owner acted, the responsible user is told") << QByteArray("resp") << true << QByteArray("owner") << QByteArrayList({"resp"});
	QTest::newRow("the contract acted by itself") << QByteArray() << false << QByteArray() << QByteArrayList({"owner"});
}


void CContractNotificationPlanTest::testSelectRecipients()
{
	QFETCH(QByteArray, responsibleUserId);
	QFETCH(bool, isResponsibleMember);
	QFETCH(QByteArray, actorUserId);
	QFETCH(QByteArrayList, expected);

	QCOMPARE(plan::SelectRecipients("owner", responsibleUserId, isResponsibleMember, actorUserId), expected);
	QCOMPARE(plan::SelectRecipients(QByteArray(), QByteArray(), false, QByteArray()), QByteArrayList());
}


I_ADD_TEST(CContractNotificationPlanTest);


