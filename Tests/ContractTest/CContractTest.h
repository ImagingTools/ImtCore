// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	The state of a contract computed from the states of its parties and its dates,
	what of it is in force, its content hash and its serialization.
*/
class CContractTest: public QObject
{
	Q_OBJECT

private slots:
	// State of the contract
	void testState_data();
	void testState();
	void testState_PeriodEndOfARenewingContract();
	void testState_ExpiringAndTerminatingFlags();

	// What is in force
	void testLink_InForceWithinItsPeriodAndBoundParties();
	void testAllocation_InForceOnItsLinkWhileTheGrantorIsBound();
	void testPartyBound_UntilItLeaves();

	// Content hash
	void testContentHash_IgnoresOrderAndProgress();
	void testContentHash_FollowsTheTerms();

	// Serialization
	void testContractInfo_SerializeRoundTrip();
	void testContractTypeInfo_SerializeRoundTrip();
	void testContractEventInfo_SerializeRoundTrip();
};


