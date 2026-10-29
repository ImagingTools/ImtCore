// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpDataUploaderTest: public QObject
{
	Q_OBJECT
private slots:
	// Properties
	void DefaultsTest();
	void PropertiesTest();

	// Splitting the data into parts
	void UploadMultiplePartsTest();
	void UploadSinglePartTest();
	void UploadExactMultipleOfPartSizeTest();
	void UploadPartSizeOneTest();
	void UploadEmptyDeviceTest();

	// Sources of the data
	void InitializeDeviceFromFileTest();
	void InitializeDeviceFromMissingFileTest();
	void SetDeviceReplacesFileDeviceTest();

	// Request parameters
	void UploadAdditionalHeadersTest();
	void UploadUsesDefaultUrlTest();
	void UploadExplicitUrlOverridesDefaultUrlTest();

	// Errors
	void UploadResponseStatusTest_data();
	void UploadResponseStatusTest();
	void UploadStopsAtFirstFailedPartTest();
	void UploadConnectionRefusedTest();
	void UploadInvalidPartSizeTest_data();
	void UploadInvalidPartSizeTest();
	void UploadUnreadableDeviceTest();
};


