// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtTest/QtTest>


/**
	The access token of a session expires while the client is subscribed over the WebSocket.
	The real session controller, authentication manager and WebSocket servlet are wired together.
*/
class CSessionTokenExpiryTest: public QObject
{
	Q_OBJECT

private slots:
	void LifetimesComeFromSettings_data();
	void LifetimesComeFromSettings();
	void ExpiredTokenIsRefusedAsUnauthorized();
	void RefreshKeepsTheSession();
	void RotatedRefreshTokenIsRefused();
	void ReusedRefreshTokenClosesTheSession();
	void UnknownRefreshTokenClosesNothing();
	void ReusedRefreshTokenWithinTheLeewayIsRefused();
	void RotatedTokensLiveWithTheirSession();
	void ConcurrentRefreshRotatesOnce();
	void CachedTokenIsNotServedPastExpiry();
	void EstablishedSubscriptionOutlivesTheToken();
	void ResubscribeWithTheRefreshedToken();
	void RemoteSourceAsksForANewToken();
	void LogoutThroughProductServerDropsTheToken();
	void SessionEndEndsTheSubscription();
	void RefreshMovesTheSessionEnd();
};
