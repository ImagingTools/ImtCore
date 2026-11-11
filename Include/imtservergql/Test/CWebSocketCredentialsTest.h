// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtTest/QtTest>


class CWebSocketCredentialsTest: public QObject
{
	Q_OBJECT

private slots:
	void RegistrationWithoutChange();
	void RefusedCredentialsAreNotRegistered_data();
	void RefusedCredentialsAreNotRegistered();
	void RegistrationRollback_data();
	void RegistrationRollback();
	void RegistrationKeptWithSameAccess();
	void CheckOfSubscription_data();
	void CheckOfSubscription();
	void AnonymousSubscription();
	void RegistrationRefusalIsTyped_data();
	void RegistrationRefusalIsTyped();
	void NoCheckWithoutChangeBeforeTheControl();
	void PassedSessionDeadlineIsNotUnknown();
	void ChangeOfTheAccessChecksAgain();
	void ChangesDuringACheckCoalesce();
	void CheckDoesNotTriggerItself();
	void GraceEndsAsTemporarilyUnavailable();
	void HungCheckHoldsBackNothing();
	void RemovalDuringACheck();
	void LateAnswerAfterSubscribingAgain();
	void ExhaustedPoolHoldsBackNothing();
	void ExpiredQueuedCheckDoesNotReadTheSource();
	void StopWithChecksRunning();
	void ChangedServerTokenChecksAgain();
	void MoreAccessKeepsTheSubscription();
	void MoreAccessKeepsTheCacheFresh();
	void RecipientChannelOutlivesANarrowing_data();
	void RecipientChannelOutlivesANarrowing();
	void RecipientChannelEndsWhenRefused();
	void RecipientChannelEndsWithTheGrace();
	void RecipientChannelsAreDeclaredByCommand();
	void MoreAccessDuringTheRegistration();
	void RecipientChannelEndsForAnotherRecipient();
	void PublisherReadPermissions_data();
	void PublisherReadPermissions();
	void CollectionNotificationContainsOnlyIdentifiers_data();
	void CollectionNotificationContainsOnlyIdentifiers();
	void ReconnectInvalidatesCacheAndRechecksAccess();
};
