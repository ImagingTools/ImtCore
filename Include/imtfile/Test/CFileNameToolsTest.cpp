// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFileNameToolsTest.h"


// ImtCore includes
#include <imtfile/CFileNameTools.h>


void CFileNameToolsTest::CheckFileNameTest_data()
{
	QTest::addColumn<QString>("name");
	QTest::addColumn<QString>("error");

	// Valid names
	QTest::newRow("plain") << QStringLiteral("Setup.exe") << QString();
	QTest::newRow("cyrillic and space") << QStringLiteral(u"\u041E\u0442\u0447\u0451\u0442 2026.pdf") << QString();
	QTest::newRow("leading space") << QStringLiteral(" leading") << QString();
	QTest::newRow("leading dot") << QStringLiteral(".hidden") << QString();
	QTest::newRow("surrogate pair") << QStringLiteral(u"\U0001F600.txt") << QString();
	QTest::newRow("like a device") << QStringLiteral("console.txt") << QString();
	QTest::newRow("COM10") << QStringLiteral("COM10") << QString();
	QTest::newRow("LPT") << QStringLiteral("LPT") << QString();
	QTest::newRow("255 bytes") << QString(255, QLatin1Char('a')) << QString();
	QTest::newRow("254 bytes of two-byte characters") << QString(127, QChar(u'\u00E9')) << QString();

	// Names that cannot be used
	QTest::newRow("empty") << QString() << QStringLiteral("The name is empty");
	QTest::newRow("dot") << QStringLiteral(".") << QStringLiteral("The name '.' is not allowed");
	QTest::newRow("two dots") << QStringLiteral("..") << QStringLiteral("The name '..' is not allowed");
	QTest::newRow("256 bytes") << QString(256, QLatin1Char('a')) << QStringLiteral("The name is longer than 255 bytes in UTF-8");
	QTest::newRow("256 bytes of two-byte characters") << QString(128, QChar(u'\u00E9')) << QStringLiteral("The name is longer than 255 bytes in UTF-8");
	const QString forbiddenCharacters = QStringLiteral("<>:\"/\\|?*");
	for (const QChar character : forbiddenCharacters){
		QTest::addRow("character %c", character.toLatin1())
					<< (QStringLiteral("a") + character + QStringLiteral("b"))
					<< QStringLiteral("The name contains the character '%1', which is not allowed").arg(character);
	}
	QTest::newRow("tab") << QStringLiteral("a\tb") << QStringLiteral("The name contains a control character");
	QTest::newRow("delete") << QStringLiteral(u"a\u007Fb") << QStringLiteral("The name contains a control character");
	QTest::newRow("next line") << QStringLiteral(u"a\u0085b") << QStringLiteral("The name contains a control character");
	QTest::newRow("unpaired surrogate") << (QStringLiteral("a") + QChar(u'\xD83D') + QStringLiteral("b")) << QStringLiteral("The name contains an unpaired surrogate");
	QTest::newRow("trailing dot") << QStringLiteral("name.") << QStringLiteral("The name ends with a dot or a space");
	QTest::newRow("trailing space") << QStringLiteral("name ") << QStringLiteral("The name ends with a dot or a space");
	QTest::newRow("CON") << QStringLiteral("CON") << QStringLiteral("The name 'CON' is reserved for a device on Windows");
	QTest::newRow("con with an extension") << QStringLiteral("con.txt") << QStringLiteral("The name 'con.txt' is reserved for a device on Windows");
	QTest::newRow("NUL with two extensions") << QStringLiteral("Nul.tar.gz") << QStringLiteral("The name 'Nul.tar.gz' is reserved for a device on Windows");
	QTest::newRow("space before the extension") << QStringLiteral("AUX .log") << QStringLiteral("The name 'AUX .log' is reserved for a device on Windows");
	QTest::newRow("COM1") << QStringLiteral("COM1") << QStringLiteral("The name 'COM1' is reserved for a device on Windows");
	QTest::newRow("LPT0") << QStringLiteral("lpt0.txt") << QStringLiteral("The name 'lpt0.txt' is reserved for a device on Windows");
	QTest::newRow("COM superscript") << QStringLiteral(u"COM\u00B9") << QStringLiteral(u"The name 'COM\u00B9' is reserved for a device on Windows");
}


void CFileNameToolsTest::CheckFileNameTest()
{
	QFETCH(QString, name);
	QFETCH(QString, error);

	QString actualError;
	QCOMPARE(imtfile::CFileNameTools::CheckFileName(name, actualError), error.isEmpty());
	QCOMPARE(actualError, error);
}


I_ADD_TEST(CFileNameToolsTest);
