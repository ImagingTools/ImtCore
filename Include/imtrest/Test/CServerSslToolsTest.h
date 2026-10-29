// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CServerSslToolsTest: public QObject
{
	Q_OBJECT
private slots:
	void CertificateChainTest();
	void SingleCertificateTest();
	void ProtocolTest();
	void CaCertificatesTest();
	void EmptyConfigurationKeepsDefaultsTest();
	void MissingFieldsAreNotChangedTest();
};


