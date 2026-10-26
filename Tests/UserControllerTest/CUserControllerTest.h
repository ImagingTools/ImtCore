// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	The account requests of CUserControllerComp: a user changes the own password with the old one,
	the recovery proves the account only by the e-mailed code, the code works once,
	the anonymous recovery requests do not tell which accounts exist,
	and the id of an account is told to its user and to whoever may view users.
*/
class CUserControllerTest: public QObject
{
	Q_OBJECT

private slots:
	// Change with the old password
	void testChangeWithOldPassword_Succeeds();
	void testChangeWithWrongOldPassword_IsRefused();
	void testChangeWithoutOldPasswordAndCode_IsRefused();
	void testSignedInUserWithoutOldPassword_CannotChangeAnotherPassword();
	void testAdmin_ChangesAnotherPasswordWithoutOldPassword();

	// Recovery by the e-mailed code
	void testRecoveryWithoutSentCode_IsRefused();
	void testRecoveryWithWrongCode_IsRefused();
	void testRecoveryWithRightCode_SucceedsOnce();
	void testRecoveryByEmail_HashesWithTheLogin();
	void testRecoveryAfterTooManyWrongCodes_IsRefused();
	void testRecoveryCodeOfAnotherUser_IsRefused();
	void testCheckEmailCode_DoesNotUseTheCode();

	// Answers to anonymous requests
	void testUnknownAccount_AnswersLikeAWrongPassword();
	void testEmailOfAnAccount_AnswersLikeAnUnknownEmail();
	void testExternalAccount_AnswersLikeAWrongPasswordUntilProven();
	void testSendEmailCode_UnknownAccountAnswersLikeAKnownOne();
	void testSendEmailCode_FailedSendingAnswersLikeAnUnknownAccount();
	void testCheckEmail_NamesNoAccount();

	// Account ids
	void testGetUserObjectId_OwnAccount();
	void testGetUserObjectId_UserViewerGetsAnotherAccount();
	void testGetUserObjectId_OtherUserGetsNothing();
	void testGetUserObjectId_AnonymousGetsNothing();
};


