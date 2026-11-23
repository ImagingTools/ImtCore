// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFileToolsTest.h"


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>

// ImtCore includes
#include <imtfile/CFileTools.h>

#ifndef Q_OS_WIN
#include <atomic>
#include <fcntl.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#endif


namespace
{


bool WriteFile(const QString& path, const QByteArray& text)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly)){
		return false;
	}

	return file.write(text) == text.size();
}


} // namespace


// The digest is the SHA-256 of the whole contents, the opened file reads them
void CFileToolsTest::DigestOfRegularFileTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString filePath = root.filePath(QStringLiteral("Data.txt"));
	QVERIFY(WriteFile(filePath, "contents"));

	QCOMPARE(imtfile::CFileTools::GetFileDigest(filePath), QCryptographicHash::hash("contents", QCryptographicHash::Sha256));

	QFile file;
	QVERIFY(imtfile::CFileTools::OpenRegularFile(filePath, file));
	QCOMPARE(file.readAll(), QByteArray("contents"));
}


void CFileToolsTest::FolderAndMissingFileHaveNoDigestTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());

	QVERIFY(imtfile::CFileTools::GetFileDigest(root.path()).isEmpty());
	QVERIFY(imtfile::CFileTools::GetFileDigest(root.filePath(QStringLiteral("Missing.txt"))).isEmpty());

	QFile file;
	QVERIFY(!imtfile::CFileTools::OpenRegularFile(root.path(), file));
}


// A link to a regular file is not followed: what is read is the file itself, not the one that the link leads to
void CFileToolsTest::LinkIsNotFollowedTest()
{
#ifdef Q_OS_WIN
	QSKIP("A link on Windows needs a privilege to be created");
#else
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString filePath = root.filePath(QStringLiteral("Data.txt"));
	QVERIFY(WriteFile(filePath, "contents"));
	const QString linkPath = root.filePath(QStringLiteral("Link.txt"));
	QVERIFY(QFile::link(filePath, linkPath));

	QVERIFY(imtfile::CFileTools::GetFileDigest(linkPath).isEmpty());
#endif
}


// A FIFO without a writer: the reading neither waits nor reads it
void CFileToolsTest::SpecialFileIsNotWaitedForTest()
{
#ifdef Q_OS_WIN
	QSKIP("Windows has no special files in the folders");
#else
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QByteArray nativeFilePath = QFile::encodeName(root.filePath(QStringLiteral("Fifo")));
	QCOMPARE(::mkfifo(nativeFilePath.constData(), 0600), 0);

	// If the reading waited for a writer, one would come every 3 seconds: the test fails instead of hanging
	const qint64 writerDelay = 3000;
	std::atomic<bool> isFinished(false);
	std::thread writer([nativeFilePath, &isFinished](){
		QElapsedTimer writerTimer;
		writerTimer.start();
		while (!isFinished){
			std::this_thread::sleep_for(std::chrono::milliseconds(50));

			if (writerTimer.elapsed() >= writerDelay){
				const int fileHandle = ::open(nativeFilePath.constData(), O_WRONLY | O_NONBLOCK);
				if (fileHandle >= 0){
					::close(fileHandle);
				}

				writerTimer.restart();
			}
		}
	});

	QElapsedTimer timer;
	timer.start();

	const QByteArray digest = imtfile::CFileTools::GetFileDigest(QFile::decodeName(nativeFilePath));

	const qint64 elapsed = timer.elapsed();
	isFinished = true;
	writer.join();

	QVERIFY(elapsed < writerDelay);
	QVERIFY(digest.isEmpty());
#endif
}


I_ADD_TEST(CFileToolsTest);


