// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>
#include <QtCore/QThread>
#include <QtCore/QCoreApplication>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtmdbx/CMask.h>
#include <imtmdbx/CMaskContainer.h>

// std includes
#include <iostream>


class CMdbxTest : public QObject
{
	Q_OBJECT

public:
	CMdbxTest();
	~CMdbxTest();

	void test_write_units();
	void test_write_10unitsInt();

	void test_int();
	void test_int64();
	void test_char();
	void test_mdbxmap();
	void test_mdbx();
	void test_mdbxfind();
	void test_mdbxcursor();
	//void test_read_write();
	void test_read_all_units();
	void test_write_10units();
	void test_create_index();
	void test_get_doc();
	void test_get_key();
	void test_update_doc();

	void initTestCase();
	void cleanupTestCase();
	// void test_mdbxcursor();
    void test_cursor();
	void test_write_masks();

private slots:
	void test_delete_append();
};



