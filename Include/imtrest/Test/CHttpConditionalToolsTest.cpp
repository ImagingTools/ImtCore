// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpConditionalToolsTest.h"


// Qt includes
#include <QtCore/QTimeZone>

// ImtCore includes
#include <imtrest/CHttpConditionalTools.h>


namespace
{


using Tools = imtrest::CHttpConditionalTools;


// Sun, 06 Nov 1994 08:49:37 GMT, the example of RFC 9110
const qint64 s_time = 784111777;
const char* const s_date = "Sun, 06 Nov 1994 08:49:37 GMT";
const char* const s_dateBefore = "Sun, 06 Nov 1994 08:49:36 GMT";
const char* const s_dateAfter = "Sun, 06 Nov 1994 08:49:38 GMT";

// The last change of the representation is unknown
const qint64 s_unknownTime = -1;


QDateTime ToDateTime(qint64 seconds)
{
	return (seconds < 0) ? QDateTime() : QDateTime::fromSecsSinceEpoch(seconds, QTimeZone::UTC);
}


void AddPreconditionRow(
			const char* name,
			const char* method,
			const char* ifMatch,
			const char* ifNoneMatch,
			const char* ifModifiedSince,
			const char* ifUnmodifiedSince,
			const char* eTag,
			qint64 lastModified,
			Tools::PreconditionResult expected)
{
	QTest::newRow(name) << QByteArray(method) << QByteArray(ifMatch) << QByteArray(ifNoneMatch)
				<< QByteArray(ifModifiedSince) << QByteArray(ifUnmodifiedSince) << QByteArray(eTag) << lastModified << expected;
}


} // namespace


// private slots

void CHttpConditionalToolsTest::ParseEntityTagTest_data()
{
	QTest::addColumn<QByteArray>("value");
	QTest::addColumn<QByteArray>("opaqueTag");
	QTest::addColumn<bool>("isWeak");

	QTest::newRow("strong") << QByteArray("\"xyzzy\"") << QByteArray("xyzzy") << false;
	QTest::newRow("weak") << QByteArray("W/\"xyzzy\"") << QByteArray("xyzzy") << true;
	QTest::newRow("empty strong") << QByteArray("\"\"") << QByteArray() << false;
	QTest::newRow("empty weak") << QByteArray("W/\"\"") << QByteArray() << true;
	QTest::newRow("hash") << QByteArray("\"33a64df551425fcc55e4d42a148795d9f25f89d4\"") << QByteArray("33a64df551425fcc55e4d42a148795d9f25f89d4") << false;
	QTest::newRow("comma inside") << QByteArray("\"a,b\"") << QByteArray("a,b") << false;
	QTest::newRow("slash inside") << QByteArray("\"W/x\"") << QByteArray("W/x") << false;
	QTest::newRow("the allowed marks") << QByteArray("\"!#$%&'()*+,-./:;<=>?@[\\]^_`{|}~\"") << QByteArray("!#$%&'()*+,-./:;<=>?@[\\]^_`{|}~") << false;
	QTest::newRow("obsolete text") << QByteArray("\"\xE9\xFF\"") << QByteArray("\xE9\xFF") << false;
	QTest::newRow("surrounding whitespace") << QByteArray(" \t\"xyzzy\" \t") << QByteArray("xyzzy") << false;
	QTest::newRow("weak with surrounding whitespace") << QByteArray(" W/\"xyzzy\" ") << QByteArray("xyzzy") << true;
}


void CHttpConditionalToolsTest::ParseEntityTagTest()
{
	QFETCH(QByteArray, value);
	QFETCH(QByteArray, opaqueTag);
	QFETCH(bool, isWeak);

	Tools::EntityTag tag;

	QVERIFY(Tools::ParseEntityTag(value, tag));
	QCOMPARE(tag.opaqueTag, opaqueTag);
	QCOMPARE(tag.isWeak, isWeak);
}


void CHttpConditionalToolsTest::ParseEntityTagInvalidTest_data()
{
	QTest::addColumn<QByteArray>("value");

	QTest::newRow("empty") << QByteArray();
	QTest::newRow("no quotes") << QByteArray("xyzzy");
	QTest::newRow("no closing quote") << QByteArray("\"xyzzy");
	QTest::newRow("no opening quote") << QByteArray("xyzzy\"");
	QTest::newRow("only quote") << QByteArray("\"");
	QTest::newRow("single quotes") << QByteArray("'xyzzy'");
	QTest::newRow("weak prefix is case-sensitive") << QByteArray("w/\"xyzzy\"");
	QTest::newRow("space after the weak prefix") << QByteArray("W/ \"xyzzy\"");
	QTest::newRow("weak prefix without a tag") << QByteArray("W/");
	QTest::newRow("weak prefix without quotes") << QByteArray("W/xyzzy");
	QTest::newRow("backslash instead of slash") << QByteArray("W\\\"xyzzy\"");
	QTest::newRow("space inside") << QByteArray("\"a b\"");
	QTest::newRow("tab inside") << QByteArray("\"a\tb\"");
	QTest::newRow("line break inside") << QByteArray("\"a\nb\"");
	QTest::newRow("delete character inside") << QByteArray("\"a\x7F\"");
	QTest::newRow("null character inside") << QByteArray("\"a\0b\"", 5);
	QTest::newRow("quote inside") << QByteArray("\"a\"b\"");
	QTest::newRow("two quotes inside") << QByteArray("\"a\"\"b\"");
	QTest::newRow("text after the tag") << QByteArray("\"a\"x");
	QTest::newRow("two tags") << QByteArray("\"a\" \"b\"");
	QTest::newRow("list of tags") << QByteArray("\"a\", \"b\"");
	QTest::newRow("comma after the tag") << QByteArray("\"a\",");
	QTest::newRow("star") << QByteArray("*");
}


void CHttpConditionalToolsTest::ParseEntityTagInvalidTest()
{
	QFETCH(QByteArray, value);

	Tools::EntityTag tag;

	QVERIFY2(!Tools::ParseEntityTag(value, tag), value.constData());
}


void CHttpConditionalToolsTest::ParseEntityTagKeepsResultOnErrorTest()
{
	Tools::EntityTag tag;
	tag.opaqueTag = "old";
	tag.isWeak = true;

	QVERIFY(!Tools::ParseEntityTag("\"a b\"", tag));

	QCOMPARE(tag.opaqueTag, QByteArray("old"));
	QVERIFY(tag.isWeak);
}


void CHttpConditionalToolsTest::CreateEntityTagTest_data()
{
	QTest::addColumn<QByteArray>("opaqueTag");
	QTest::addColumn<bool>("isWeak");
	QTest::addColumn<QByteArray>("expected");

	QTest::newRow("strong") << QByteArray("xyzzy") << false << QByteArray("\"xyzzy\"");
	QTest::newRow("weak") << QByteArray("xyzzy") << true << QByteArray("W/\"xyzzy\"");
	QTest::newRow("empty strong") << QByteArray() << false << QByteArray("\"\"");
	QTest::newRow("empty weak") << QByteArray() << true << QByteArray("W/\"\"");
	QTest::newRow("size and time") << QByteArray("5a8-3f2b1c") << false << QByteArray("\"5a8-3f2b1c\"");
	QTest::newRow("comma inside") << QByteArray("a,b") << false << QByteArray("\"a,b\"");
	QTest::newRow("obsolete text") << QByteArray("\xE9") << false << QByteArray("\"\xE9\"");
}


void CHttpConditionalToolsTest::CreateEntityTagTest()
{
	QFETCH(QByteArray, opaqueTag);
	QFETCH(bool, isWeak);
	QFETCH(QByteArray, expected);

	QCOMPARE(Tools::CreateEntityTag(opaqueTag, isWeak), expected);
}


void CHttpConditionalToolsTest::CreateEntityTagInvalidTest_data()
{
	QTest::addColumn<QByteArray>("opaqueTag");

	QTest::newRow("quote") << QByteArray("a\"b");
	QTest::newRow("space") << QByteArray("a b");
	QTest::newRow("tab") << QByteArray("a\tb");
	QTest::newRow("line break") << QByteArray("a\r\nb");
	QTest::newRow("delete character") << QByteArray("a\x7F");
	QTest::newRow("null character") << QByteArray("a\0b", 3);
}


void CHttpConditionalToolsTest::CreateEntityTagInvalidTest()
{
	QFETCH(QByteArray, opaqueTag);

	QVERIFY(Tools::CreateEntityTag(opaqueTag, false).isEmpty());
	QVERIFY(Tools::CreateEntityTag(opaqueTag, true).isEmpty());
}


void CHttpConditionalToolsTest::CreateEntityTagRoundTripTest()
{
	const QByteArray opaqueTags[] = {QByteArray(), QByteArray("v1"), QByteArray("a,b"), QByteArray("!#~"), QByteArray("\xC0\xFF")};

	for (const QByteArray& opaqueTag : opaqueTags){
		for (bool isWeak : {false, true}){
			const QByteArray value = Tools::CreateEntityTag(opaqueTag, isWeak);

			Tools::EntityTag tag;
			QVERIFY2(Tools::ParseEntityTag(value, tag), value.constData());

			QCOMPARE(tag.opaqueTag, opaqueTag);
			QCOMPARE(tag.isWeak, isWeak);
		}
	}
}


void CHttpConditionalToolsTest::MatchEntityTagListTest_data()
{
	QTest::addColumn<QByteArray>("listValue");
	QTest::addColumn<QByteArray>("eTag");
	QTest::addColumn<bool>("useStrongComparison");
	QTest::addColumn<Tools::EntityTagMatch>("expected");

	// Star matches any representation, even the representation without an entity tag
	QTest::newRow("star, strong tag, weak comparison") << QByteArray("*") << QByteArray("\"a\"") << false << Tools::ETM_MATCH;
	QTest::newRow("star, strong tag, strong comparison") << QByteArray("*") << QByteArray("\"a\"") << true << Tools::ETM_MATCH;
	QTest::newRow("star, weak tag, strong comparison") << QByteArray("*") << QByteArray("W/\"a\"") << true << Tools::ETM_MATCH;
	QTest::newRow("star, no tag, weak comparison") << QByteArray("*") << QByteArray() << false << Tools::ETM_MATCH;
	QTest::newRow("star, no tag, strong comparison") << QByteArray("*") << QByteArray() << true << Tools::ETM_MATCH;
	QTest::newRow("star with whitespace") << QByteArray(" * ") << QByteArray("\"a\"") << false << Tools::ETM_MATCH;

	// Strong tags
	QTest::newRow("equal strong tags, weak comparison") << QByteArray("\"a\"") << QByteArray("\"a\"") << false << Tools::ETM_MATCH;
	QTest::newRow("equal strong tags, strong comparison") << QByteArray("\"a\"") << QByteArray("\"a\"") << true << Tools::ETM_MATCH;
	QTest::newRow("different tags, weak comparison") << QByteArray("\"a\"") << QByteArray("\"b\"") << false << Tools::ETM_NO_MATCH;
	QTest::newRow("different tags, strong comparison") << QByteArray("\"a\"") << QByteArray("\"b\"") << true << Tools::ETM_NO_MATCH;
	QTest::newRow("tags are case-sensitive") << QByteArray("\"A\"") << QByteArray("\"a\"") << false << Tools::ETM_NO_MATCH;
	QTest::newRow("prefix of the tag") << QByteArray("\"ab\"") << QByteArray("\"a\"") << false << Tools::ETM_NO_MATCH;
	QTest::newRow("empty tags") << QByteArray("\"\"") << QByteArray("\"\"") << true << Tools::ETM_MATCH;

	// Weak tags (RFC 9110, section 8.8.3.2: the table of comparison)
	QTest::newRow("weak and weak, weak comparison") << QByteArray("W/\"a\"") << QByteArray("W/\"a\"") << false << Tools::ETM_MATCH;
	QTest::newRow("weak and weak, strong comparison") << QByteArray("W/\"a\"") << QByteArray("W/\"a\"") << true << Tools::ETM_NO_MATCH;
	QTest::newRow("weak and strong, weak comparison") << QByteArray("W/\"a\"") << QByteArray("\"a\"") << false << Tools::ETM_MATCH;
	QTest::newRow("weak and strong, strong comparison") << QByteArray("W/\"a\"") << QByteArray("\"a\"") << true << Tools::ETM_NO_MATCH;
	QTest::newRow("strong and weak, weak comparison") << QByteArray("\"a\"") << QByteArray("W/\"a\"") << false << Tools::ETM_MATCH;
	QTest::newRow("strong and weak, strong comparison") << QByteArray("\"a\"") << QByteArray("W/\"a\"") << true << Tools::ETM_NO_MATCH;
	QTest::newRow("weak and weak with different tags") << QByteArray("W/\"a\"") << QByteArray("W/\"b\"") << false << Tools::ETM_NO_MATCH;

	// Lists
	QTest::newRow("first in the list") << QByteArray("\"a\", \"b\", \"c\"") << QByteArray("\"a\"") << false << Tools::ETM_MATCH;
	QTest::newRow("middle of the list") << QByteArray("\"a\", \"b\", \"c\"") << QByteArray("\"b\"") << false << Tools::ETM_MATCH;
	QTest::newRow("last in the list") << QByteArray("\"a\", \"b\", \"c\"") << QByteArray("\"c\"") << false << Tools::ETM_MATCH;
	QTest::newRow("not in the list") << QByteArray("\"a\", \"b\", \"c\"") << QByteArray("\"d\"") << false << Tools::ETM_NO_MATCH;
	QTest::newRow("list without spaces") << QByteArray("\"a\",\"b\"") << QByteArray("\"b\"") << true << Tools::ETM_MATCH;
	QTest::newRow("list with tabs") << QByteArray("\"a\"\t,\t\"b\"") << QByteArray("\"b\"") << true << Tools::ETM_MATCH;
	QTest::newRow("weak element before strong element, strong comparison") << QByteArray("W/\"a\", \"a\"") << QByteArray("\"a\"") << true << Tools::ETM_MATCH;
	QTest::newRow("only weak elements, strong comparison") << QByteArray("W/\"a\", W/\"b\"") << QByteArray("\"a\"") << true << Tools::ETM_NO_MATCH;
	QTest::newRow("empty list elements") << QByteArray(" ,\"a\" ,, \"b\" ,") << QByteArray("\"b\"") << false << Tools::ETM_MATCH;

	// A comma inside of the tag does not separate the tags
	QTest::newRow("comma inside, equal tag") << QByteArray("\"a,b\"") << QByteArray("\"a,b\"") << false << Tools::ETM_MATCH;
	QTest::newRow("comma inside, first part") << QByteArray("\"a,b\"") << QByteArray("\"a\"") << false << Tools::ETM_NO_MATCH;
	QTest::newRow("comma inside, second part") << QByteArray("\"a,b\"") << QByteArray("\"b\"") << false << Tools::ETM_NO_MATCH;
	QTest::newRow("comma inside, in the list") << QByteArray("\"x\", \"a,b\", \"y\"") << QByteArray("\"a,b\"") << false << Tools::ETM_MATCH;

	// The representation has no valid entity tag
	QTest::newRow("no tag, weak comparison") << QByteArray("\"a\"") << QByteArray() << false << Tools::ETM_NO_MATCH;
	QTest::newRow("no tag, strong comparison") << QByteArray("\"a\"") << QByteArray() << true << Tools::ETM_NO_MATCH;
	QTest::newRow("no tag, empty tag in the list") << QByteArray("\"\"") << QByteArray() << false << Tools::ETM_NO_MATCH;
	QTest::newRow("tag without quotes") << QByteArray("\"a\"") << QByteArray("a") << false << Tools::ETM_NO_MATCH;
	QTest::newRow("tag in quotes is not a star") << QByteArray("\"*\"") << QByteArray("\"a\"") << false << Tools::ETM_NO_MATCH;
}


void CHttpConditionalToolsTest::MatchEntityTagListTest()
{
	QFETCH(QByteArray, listValue);
	QFETCH(QByteArray, eTag);
	QFETCH(bool, useStrongComparison);
	QFETCH(Tools::EntityTagMatch, expected);

	QCOMPARE(Tools::MatchEntityTagList(listValue, eTag, useStrongComparison), expected);
}


void CHttpConditionalToolsTest::MatchEntityTagListInvalidTest_data()
{
	QTest::addColumn<QByteArray>("listValue");

	QTest::newRow("empty") << QByteArray();
	QTest::newRow("whitespace") << QByteArray(" \t ");
	QTest::newRow("only comma") << QByteArray(",");
	QTest::newRow("only commas") << QByteArray(", ,,");
	QTest::newRow("tag without quotes") << QByteArray("a");
	QTest::newRow("no closing quote") << QByteArray("\"a");
	QTest::newRow("text after the tag") << QByteArray("\"a\"x");
	QTest::newRow("two tags without comma") << QByteArray("\"a\" \"b\"");
	QTest::newRow("two tags glued") << QByteArray("\"a\"\"b\"");
	QTest::newRow("weak prefix in lower case") << QByteArray("w/\"a\"");
	QTest::newRow("space inside of the tag") << QByteArray("\"a b\"");

	// A match is not enough, the whole list should be valid
	QTest::newRow("bad element after the matching element") << QByteArray("\"a\",x");
	QTest::newRow("bad element before the matching element") << QByteArray("x,\"a\"");
	QTest::newRow("bad element between") << QByteArray("\"a\",x,\"b\"");

	// Star is valid only alone
	QTest::newRow("star and a tag") << QByteArray("*,\"a\"");
	QTest::newRow("tag and a star") << QByteArray("\"a\",*");
	QTest::newRow("two stars") << QByteArray("*,*");
	QTest::newRow("double star") << QByteArray("**");
}


void CHttpConditionalToolsTest::MatchEntityTagListInvalidTest()
{
	QFETCH(QByteArray, listValue);

	for (bool useStrongComparison : {false, true}){
		QCOMPARE(Tools::MatchEntityTagList(listValue, "\"a\"", useStrongComparison), Tools::ETM_INVALID);
		QCOMPARE(Tools::MatchEntityTagList(listValue, QByteArray(), useStrongComparison), Tools::ETM_INVALID);
	}
}


void CHttpConditionalToolsTest::EvaluatePreconditionsTest_data()
{
	QTest::addColumn<QByteArray>("method");
	QTest::addColumn<QByteArray>("ifMatch");
	QTest::addColumn<QByteArray>("ifNoneMatch");
	QTest::addColumn<QByteArray>("ifModifiedSince");
	QTest::addColumn<QByteArray>("ifUnmodifiedSince");
	QTest::addColumn<QByteArray>("eTag");
	QTest::addColumn<qint64>("lastModified");
	QTest::addColumn<Tools::PreconditionResult>("expected");

	const char* const tag = "\"v2\"";

	// No conditions
	AddPreconditionRow("no conditions, GET", "GET", "", "", "", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("no conditions, HEAD", "HEAD", "", "", "", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("no conditions, PUT", "PUT", "", "", "", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("no conditions, no validators", "GET", "", "", "", "", "", s_unknownTime, Tools::PR_PROCEED);

	// If-Match: the strong comparison
	AddPreconditionRow("If-Match equal", "PUT", "\"v2\"", "", "", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Match different", "PUT", "\"v1\"", "", "", "", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Match different, GET", "GET", "\"v1\"", "", "", "", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Match star", "PUT", "*", "", "", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Match list", "PUT", "\"v1\", \"v2\"", "", "", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Match list without the tag", "PUT", "\"v0\", \"v1\"", "", "", "", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Match weak tag in the request", "PUT", "W/\"v2\"", "", "", "", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Match weak tag of the representation", "PUT", "\"v2\"", "", "", "", "W/\"v2\"", s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Match star, no tag", "PUT", "*", "", "", "", "", s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Match tag, no tag", "PUT", "\"v2\"", "", "", "", "", s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Match bad value", "PUT", "v2", "", "", "", tag, s_time, Tools::PR_BAD_REQUEST);
	AddPreconditionRow("If-Match bad value after a match", "PUT", "\"v2\",x", "", "", "", tag, s_time, Tools::PR_BAD_REQUEST);

	// If-Unmodified-Since
	AddPreconditionRow("If-Unmodified-Since equal", "PUT", "", "", "", s_date, tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Unmodified-Since after", "PUT", "", "", "", s_dateAfter, tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Unmodified-Since before", "PUT", "", "", "", s_dateBefore, tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Unmodified-Since before, GET", "GET", "", "", "", s_dateBefore, tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Unmodified-Since bad date", "PUT", "", "", "", "yesterday", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Unmodified-Since two dates", "PUT", "", "", "", "Sun, 06 Nov 1994 08:49:37 GMT, Sun, 06 Nov 1994 08:49:36 GMT", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Unmodified-Since, unknown time", "PUT", "", "", "", s_dateBefore, tag, s_unknownTime, Tools::PR_PROCEED);
	AddPreconditionRow("If-Unmodified-Since RFC 850 equal", "PUT", "", "", "", "Sunday, 06-Nov-94 08:49:37 GMT", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Unmodified-Since RFC 850 before", "PUT", "", "", "", "Sunday, 06-Nov-94 08:49:36 GMT", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Unmodified-Since asctime before", "PUT", "", "", "", "Sun Nov  6 08:49:36 1994", tag, s_time, Tools::PR_PRECONDITION_FAILED);

	// If-Match wins over If-Unmodified-Since
	AddPreconditionRow("If-Match true ignores If-Unmodified-Since", "PUT", "\"v2\"", "", "", s_dateBefore, tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Match false, If-Unmodified-Since true", "PUT", "\"v1\"", "", "", s_dateAfter, tag, s_time, Tools::PR_PRECONDITION_FAILED);

	// If-None-Match: the weak comparison, 304 for GET and HEAD, 412 for the other methods
	AddPreconditionRow("If-None-Match equal, GET", "GET", "", "\"v2\"", "", "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-None-Match equal, HEAD", "HEAD", "", "\"v2\"", "", "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-None-Match equal, PUT", "PUT", "", "\"v2\"", "", "", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-None-Match equal, POST", "POST", "", "\"v2\"", "", "", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-None-Match equal, DELETE", "DELETE", "", "\"v2\"", "", "", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-None-Match different, GET", "GET", "", "\"v1\"", "", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-None-Match different, PUT", "PUT", "", "\"v1\"", "", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-None-Match weak tag in the request", "GET", "", "W/\"v2\"", "", "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-None-Match weak tag of the representation", "GET", "", "\"v2\"", "", "", "W/\"v2\"", s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-None-Match star, GET", "GET", "", "*", "", "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-None-Match star, PUT", "PUT", "", "*", "", "", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-None-Match star, no tag", "GET", "", "*", "", "", "", s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-None-Match list", "GET", "", "\"v1\", \"v2\"", "", "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-None-Match list without the tag", "GET", "", "\"v0\", \"v1\"", "", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-None-Match tag, no tag", "GET", "", "\"v2\"", "", "", "", s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-None-Match bad value, GET", "GET", "", "v2", "", "", tag, s_time, Tools::PR_BAD_REQUEST);
	AddPreconditionRow("If-None-Match bad value, PUT", "PUT", "", "v2", "", "", tag, s_time, Tools::PR_BAD_REQUEST);

	// If-Modified-Since: only GET and HEAD
	AddPreconditionRow("If-Modified-Since equal, GET", "GET", "", "", s_date, "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-Modified-Since equal, HEAD", "HEAD", "", "", s_date, "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-Modified-Since after", "GET", "", "", s_dateAfter, "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-Modified-Since before", "GET", "", "", s_dateBefore, "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Modified-Since far in the future", "GET", "", "", "Fri, 01 Jan 2100 00:00:00 GMT", "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-Modified-Since bad date", "GET", "", "", "yesterday", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Modified-Since two dates", "GET", "", "", "Sun, 06 Nov 1994 08:49:37 GMT, Sun, 06 Nov 1994 08:49:38 GMT", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Modified-Since, unknown time", "GET", "", "", s_date, "", tag, s_unknownTime, Tools::PR_PROCEED);
	AddPreconditionRow("If-Modified-Since is ignored for PUT", "PUT", "", "", s_date, "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Modified-Since is ignored for POST", "POST", "", "", s_date, "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Modified-Since is ignored for DELETE", "DELETE", "", "", s_date, "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Modified-Since RFC 850 equal", "GET", "", "", "Sunday, 06-Nov-94 08:49:37 GMT", "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-Modified-Since RFC 850 before", "GET", "", "", "Sunday, 06-Nov-94 08:49:36 GMT", "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-Modified-Since asctime equal", "GET", "", "", "Sun Nov  6 08:49:37 1994", "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-Modified-Since asctime before", "GET", "", "", "Sun Nov  6 08:49:36 1994", "", tag, s_time, Tools::PR_PROCEED);

	// If-None-Match wins over If-Modified-Since
	AddPreconditionRow("If-None-Match different ignores If-Modified-Since", "GET", "", "\"v1\"", s_date, "", tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("If-None-Match equal, If-Modified-Since before", "GET", "", "\"v2\"", s_dateBefore, "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-None-Match bad value, If-Modified-Since", "GET", "", "v2", s_date, "", tag, s_time, Tools::PR_BAD_REQUEST);

	// The order of the steps (RFC 9110, section 13.2.2)
	AddPreconditionRow("If-Match true, If-None-Match equal, GET", "GET", "\"v2\"", "\"v2\"", "", "", tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-Match true, If-None-Match equal, PUT", "PUT", "\"v2\"", "\"v2\"", "", "", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Match false, If-None-Match equal, GET", "GET", "\"v1\"", "\"v2\"", "", "", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Match false, If-None-Match different, GET", "GET", "\"v1\"", "\"v0\"", "", "", tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Match bad value, If-None-Match equal", "GET", "v1", "\"v2\"", "", "", tag, s_time, Tools::PR_BAD_REQUEST);
	AddPreconditionRow("If-Unmodified-Since false, If-None-Match equal", "GET", "", "\"v2\"", "", s_dateBefore, tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Unmodified-Since true, If-None-Match equal", "GET", "", "\"v2\"", "", s_date, tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-Unmodified-Since false, If-Modified-Since true", "GET", "", "", s_date, s_dateBefore, tag, s_time, Tools::PR_PRECONDITION_FAILED);
	AddPreconditionRow("If-Unmodified-Since true, If-Modified-Since true", "GET", "", "", s_date, s_date, tag, s_time, Tools::PR_NOT_MODIFIED);
	AddPreconditionRow("If-Unmodified-Since true, If-Modified-Since false", "GET", "", "", s_dateBefore, s_date, tag, s_time, Tools::PR_PROCEED);
	AddPreconditionRow("all conditions are true", "GET", "\"v2\"", "\"v1\"", s_dateBefore, s_date, tag, s_time, Tools::PR_PROCEED);
}


void CHttpConditionalToolsTest::EvaluatePreconditionsTest()
{
	QFETCH(QByteArray, method);
	QFETCH(QByteArray, ifMatch);
	QFETCH(QByteArray, ifNoneMatch);
	QFETCH(QByteArray, ifModifiedSince);
	QFETCH(QByteArray, ifUnmodifiedSince);
	QFETCH(QByteArray, eTag);
	QFETCH(qint64, lastModified);
	QFETCH(Tools::PreconditionResult, expected);

	Tools::Preconditions preconditions;
	preconditions.ifMatch = ifMatch;
	preconditions.ifNoneMatch = ifNoneMatch;
	preconditions.ifModifiedSince = ifModifiedSince;
	preconditions.ifUnmodifiedSince = ifUnmodifiedSince;

	Tools::Validators validators;
	validators.eTag = eTag;
	validators.lastModified = ToDateTime(lastModified);

	QCOMPARE(Tools::EvaluatePreconditions(method, preconditions, validators), expected);
}


void CHttpConditionalToolsTest::EvaluatePreconditionsFractionsOfSecondTest()
{
	// The file system has the milliseconds, HTTP-date has not
	Tools::Validators validators;
	validators.lastModified = QDateTime::fromMSecsSinceEpoch(s_time * 1000 + 999, QTimeZone::UTC);

	Tools::Preconditions preconditions;

	preconditions.ifModifiedSince = s_date;
	QCOMPARE(Tools::EvaluatePreconditions("GET", preconditions, validators), Tools::PR_NOT_MODIFIED);

	preconditions.ifModifiedSince = s_dateBefore;
	QCOMPARE(Tools::EvaluatePreconditions("GET", preconditions, validators), Tools::PR_PROCEED);

	preconditions.ifModifiedSince.clear();
	preconditions.ifUnmodifiedSince = s_date;
	QCOMPARE(Tools::EvaluatePreconditions("PUT", preconditions, validators), Tools::PR_PROCEED);

	preconditions.ifUnmodifiedSince = s_dateBefore;
	QCOMPARE(Tools::EvaluatePreconditions("PUT", preconditions, validators), Tools::PR_PRECONDITION_FAILED);
}


void CHttpConditionalToolsTest::EvaluateIfRangeTest_data()
{
	QTest::addColumn<QByteArray>("ifRange");
	QTest::addColumn<QByteArray>("eTag");
	QTest::addColumn<qint64>("lastModified");
	QTest::addColumn<bool>("expected");

	// Entity tag: only the strong comparison
	QTest::newRow("equal tags") << QByteArray("\"v2\"") << QByteArray("\"v2\"") << s_time << true;
	QTest::newRow("equal tags with whitespace") << QByteArray(" \"v2\" ") << QByteArray("\"v2\"") << s_time << true;
	QTest::newRow("different tags") << QByteArray("\"v1\"") << QByteArray("\"v2\"") << s_time << false;
	QTest::newRow("weak tag in the request") << QByteArray("W/\"v2\"") << QByteArray("\"v2\"") << s_time << false;
	QTest::newRow("weak tag of the representation") << QByteArray("\"v2\"") << QByteArray("W/\"v2\"") << s_time << false;
	QTest::newRow("both tags are weak") << QByteArray("W/\"v2\"") << QByteArray("W/\"v2\"") << s_time << false;
	QTest::newRow("no tag of the representation") << QByteArray("\"v2\"") << QByteArray() << s_time << false;
	QTest::newRow("bad tag of the representation") << QByteArray("\"v2\"") << QByteArray("v2") << s_time << false;
	QTest::newRow("bad tag in the request") << QByteArray("\"v2") << QByteArray("\"v2\"") << s_time << false;
	QTest::newRow("tags are case-sensitive") << QByteArray("\"V2\"") << QByteArray("\"v2\"") << s_time << false;
	QTest::newRow("star is not a tag") << QByteArray("*") << QByteArray("\"v2\"") << s_time << false;
	QTest::newRow("list of tags") << QByteArray("\"v1\", \"v2\"") << QByteArray("\"v2\"") << s_time << false;

	// Date: the exact match
	QTest::newRow("equal date") << QByteArray(s_date) << QByteArray() << s_time << true;
	QTest::newRow("equal date with whitespace") << QByteArray(" Sun, 06 Nov 1994 08:49:37 GMT ") << QByteArray() << s_time << true;
	QTest::newRow("equal date, the representation has a tag") << QByteArray(s_date) << QByteArray("\"v2\"") << s_time << true;
	QTest::newRow("date after the change") << QByteArray(s_dateAfter) << QByteArray() << s_time << false;
	QTest::newRow("date before the change") << QByteArray(s_dateBefore) << QByteArray() << s_time << false;
	QTest::newRow("unknown time of the change") << QByteArray(s_date) << QByteArray() << s_unknownTime << false;
	QTest::newRow("RFC 850 equal") << QByteArray("Sunday, 06-Nov-94 08:49:37 GMT") << QByteArray() << s_time << true;
	QTest::newRow("RFC 850 different") << QByteArray("Sunday, 06-Nov-94 08:49:36 GMT") << QByteArray() << s_time << false;
	QTest::newRow("asctime equal") << QByteArray("Sun Nov  6 08:49:37 1994") << QByteArray() << s_time << true;
	QTest::newRow("asctime different") << QByteArray("Sun Nov  6 08:49:36 1994") << QByteArray() << s_time << false;

	// Values that are neither a tag nor a date
	QTest::newRow("empty") << QByteArray() << QByteArray("\"v2\"") << s_time << false;
	QTest::newRow("whitespace") << QByteArray("  ") << QByteArray("\"v2\"") << s_time << false;
	QTest::newRow("bad date") << QByteArray("yesterday") << QByteArray("\"v2\"") << s_time << false;
	QTest::newRow("tag without quotes") << QByteArray("v2") << QByteArray("\"v2\"") << s_time << false;
}


void CHttpConditionalToolsTest::EvaluateIfRangeTest()
{
	QFETCH(QByteArray, ifRange);
	QFETCH(QByteArray, eTag);
	QFETCH(qint64, lastModified);
	QFETCH(bool, expected);

	Tools::Validators validators;
	validators.eTag = eTag;
	validators.lastModified = ToDateTime(lastModified);

	QCOMPARE(Tools::EvaluateIfRange(ifRange, validators), expected);
}


void CHttpConditionalToolsTest::EvaluateIfRangeFractionsOfSecondTest()
{
	Tools::Validators validators;
	validators.lastModified = QDateTime::fromMSecsSinceEpoch(s_time * 1000 + 500, QTimeZone::UTC);

	QVERIFY(Tools::EvaluateIfRange(s_date, validators));
	QVERIFY(!Tools::EvaluateIfRange(s_dateAfter, validators));
	QVERIFY(!Tools::EvaluateIfRange(s_dateBefore, validators));
}


I_ADD_TEST(CHttpConditionalToolsTest);
