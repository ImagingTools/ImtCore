// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtTest/QtTest>


class CRemoteSessionTest: public QObject
{
	Q_OBJECT

private slots:
	void FirstRequestSignsInTest();
	void TokenServesTheNextRequestsTest();
	void RejectedTokenIsReplacedTest();
	void RefusedSignInIsNotRepeatedTest();
	void ChangedPasswordIsTriedTest();
	void AnswerWithoutTokenIsARefusalTest();
	void ConnectionFailureIsNoRefusalTest();
	void ParallelRequestsShareOneSignInTest();
	void ParallelRequestsShareAFailedSignInTest();
	void RequestFromInsideTheSignInFailsTest();
	void NewTokenIsAnnouncedInTheThreadOfTheSessionTest();
};
