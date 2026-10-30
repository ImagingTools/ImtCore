// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CSslCertificateChainTest: public QObject
{
	Q_OBJECT
private slots:
	// The chain of the server
	void ComposeChainTest();
	void ComposeChainWithoutServerCertificateTest();
	void ComposeChainOfNothingTest();

	// The certificates of a file or of data
	void BundleFileTest();
	void SingleCertificateFileTest();
	void EncodedDataBundleTest();
	void MissingFileTest();
	void FileWithoutCertificatesTest();

	// The configuration
	void ConfigurationWithBundleTest();
	void ConfigurationWithSingleCertificateTest();
	void ConfigurationWithoutCertificateTest();
	void ConfigurationWithMissingFileTest();
	void ConfigurationWithInvalidFileTest();
	void ConfigurationWithCaBundleTest();
	void ConfigurationWithoutCaFileTest();
};


