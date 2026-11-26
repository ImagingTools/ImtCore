// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtTest/QtTest>


class CRemoteFileSystemStructureProviderTest: public QObject
{
	Q_OBJECT

private slots:
	void LargeSizesSurviveTheWireTest();
	void ServerErrorReachesTheCallerTest();
	void RequestsEveryFieldOfTheListingTest();
	void DownloadLinkSurvivesTheWireTest();
	void DownloadLinkWithUrlOnlySurvivesTheWireTest();
	void DownloadLinkErrorReachesTheCallerTest();
	void RequestsEveryFieldOfTheLinkTest();
};
