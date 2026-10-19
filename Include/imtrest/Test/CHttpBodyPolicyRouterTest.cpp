// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpBodyPolicyRouterTest.h"


// ImtCore includes
#include <imtrest/CHttpBodyPolicyRouterComp.h>


namespace
{


int Find(const QByteArrayList& prefixes, const QByteArray& path)
{
	return imtrest::CHttpBodyPolicyRouterComp::FindPrefixIndex(prefixes, path);
}


} // namespace


// private slots

void CHttpBodyPolicyRouterTest::MatchTest_data()
{
	QTest::addColumn<QByteArray>("prefix");
	QTest::addColumn<QByteArray>("path");
	QTest::addColumn<bool>("isMatch");

	QTest::newRow("same path") << QByteArray("/files") << QByteArray("/files") << true;
	QTest::newRow("nested path") << QByteArray("/files") << QByteArray("/files/a/b") << true;
	QTest::newRow("trailing slash of the path") << QByteArray("/files") << QByteArray("/files/") << true;
	QTest::newRow("no slash in the prefix") << QByteArray("files") << QByteArray("/files/a") << true;
	QTest::newRow("slashes in the prefix") << QByteArray("/files/") << QByteArray("/files/a") << true;
	QTest::newRow("nested prefix") << QByteArray("/api/files") << QByteArray("/api/files/a") << true;

	// Only whole segments match
	QTest::newRow("longer segment") << QByteArray("/files") << QByteArray("/files2") << false;
	QTest::newRow("longer segment with a path") << QByteArray("/files") << QByteArray("/files2/a") << false;
	QTest::newRow("shorter path") << QByteArray("/api/files") << QByteArray("/api") << false;
	QTest::newRow("other path") << QByteArray("/files") << QByteArray("/other") << false;
	QTest::newRow("parent path") << QByteArray("/api/files") << QByteArray("/files") << false;
	QTest::newRow("case of the letters") << QByteArray("/files") << QByteArray("/Files") << false;

	// The empty prefix is the prefix of all paths
	QTest::newRow("empty prefix") << QByteArray() << QByteArray("/files") << true;
	QTest::newRow("root prefix") << QByteArray("/") << QByteArray("/files/a") << true;
	QTest::newRow("empty prefix, root path") << QByteArray() << QByteArray("/") << true;
	QTest::newRow("empty prefix, empty path") << QByteArray() << QByteArray() << true;
	QTest::newRow("prefix, root path") << QByteArray("/files") << QByteArray("/") << false;
}


void CHttpBodyPolicyRouterTest::MatchTest()
{
	QFETCH(QByteArray, prefix);
	QFETCH(QByteArray, path);
	QFETCH(bool, isMatch);

	QCOMPARE(Find(QByteArrayList() << prefix, path), isMatch ? 0 : -1);
}


void CHttpBodyPolicyRouterTest::LongestPrefixWinsTest()
{
	const QByteArrayList prefixes = QByteArrayList() << "/api" << "/api/files" << "/api/files/upload" << "/other";

	QCOMPARE(Find(prefixes, "/api/users"), 0);
	QCOMPARE(Find(prefixes, "/api/files"), 1);
	QCOMPARE(Find(prefixes, "/api/files/a"), 1);
	QCOMPARE(Find(prefixes, "/api/files/upload/1"), 2);
	QCOMPARE(Find(prefixes, "/other/a"), 3);
	QCOMPARE(Find(prefixes, "/unknown"), -1);

	// The order of the prefixes does not matter
	const QByteArrayList reversed = QByteArrayList() << "/api/files/upload" << "/api/files" << "/api";

	QCOMPARE(Find(reversed, "/api/files/upload/1"), 0);
	QCOMPARE(Find(reversed, "/api/files/a"), 1);
	QCOMPARE(Find(reversed, "/api/users"), 2);
}


void CHttpBodyPolicyRouterTest::FirstOfEqualPrefixesWinsTest()
{
	// The same prefix with slashes and without them is the same prefix
	const QByteArrayList prefixes = QByteArrayList() << "/files" << "files/" << "/files";

	QCOMPARE(Find(prefixes, "/files/a"), 0);
}


void CHttpBodyPolicyRouterTest::DefaultPrefixTest()
{
	const QByteArrayList prefixes = QByteArrayList() << "/files" << "";

	// The empty prefix is used if nothing else matches
	QCOMPARE(Find(prefixes, "/files/a"), 0);
	QCOMPARE(Find(prefixes, "/other"), 1);
	QCOMPARE(Find(prefixes, "/files2"), 1);
	QCOMPARE(Find(prefixes, "/"), 1);
}


void CHttpBodyPolicyRouterTest::NoPrefixesTest()
{
	QCOMPARE(Find(QByteArrayList(), "/files"), -1);
}


I_ADD_TEST(CHttpBodyPolicyRouterTest);


