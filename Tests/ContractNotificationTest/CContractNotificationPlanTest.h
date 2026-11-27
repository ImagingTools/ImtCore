// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	What an event of a contract tells its parties and what it withdraws from their inboxes
	(imtauthgql::ContractNotificationPlan), and who of a party is told.
*/
class CContractNotificationPlanTest: public QObject
{
	Q_OBJECT

private slots:
	// Negotiation
	void testInvitation_PendingForEveryInvitedParty();
	void testSigned_TellsTheInitiatorInOneGroup();
	void testSigned_AnswersTheInvitationOfTheSigner();
	void testSigned_ByTheInitiatorKeepsTheChangesAskedOfIt();
	void testChangesRequested_AnswersTheInvitationAndAsksTheInitiator();
	void testTermsChanged_ReplacesWhatWasPendingAndTheSignatures();
	void testPartyRemoved_WithdrawsItsInvitationAndHidesTheContract();
	void testNegotiationEnd_WithdrawsAllPendingAndTheSignatures_data();
	void testNegotiationEnd_WithdrawsAllPendingAndTheSignatures();

	// A concluded contract
	void testNotice_GroupedPerLeavingParty();
	void testNoticeRevoked_WithdrawsTheNoticeEverywhere();
	void testInformation_NeitherGroupedNorWithdrawing_data();
	void testInformation_NeitherGroupedNorWithdrawing();

	// Reminders
	void testReminder_NamesItsDate_data();
	void testReminder_NamesItsDate();
	void testReminder_WithoutAKnownDateTellsNothing_data();
	void testReminder_WithoutAKnownDateTellsNothing();

	// Events that tell nothing
	void testNothingTold_data();
	void testNothingTold();
	void testEmptyRecipientsAreSkipped();

	// Recipients of a party
	void testSelectRecipients_data();
	void testSelectRecipients();
};


