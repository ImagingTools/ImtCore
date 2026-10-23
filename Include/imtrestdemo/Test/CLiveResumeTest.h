// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Local includes
#include "CLiveTestBase.h"


/**
	Live server: the upload that can be resumed. The client sends a file with \c Content-Range (the whole file or its parts), the server appends the
	parts to the data that it has and keeps them if the connection breaks; the client asks how much the server has and sends the rest.
*/
class CLiveResumeTest: public CLiveTestBase
{
	Q_OBJECT
private Q_SLOTS:
	void ResumeOnePartTest();
	void ResumeByPartsTest();
	void ResumeBrokenUploadTest();
	void ResumeBrokenManyTimesTest();
	void ResumeWrongOffsetTest();
	void ResumeBadRangeTest_data();
	void ResumeBadRangeTest();
	void ResumeStateAndDeleteTest();
	void ResumeConcurrentUploadTest();
	void ResumeTemporaryNamesTest();
	void ResumeReplacesFileTest();

private:
	/**
		The part of the pattern file: bytes \c first - \c first + \c length - 1 of the file of \c total bytes.
	*/
	bool SendPart(const QByteArray& name, qint64 first, qint64 length, qint64 total, Response& response, const Headers& more = Headers());

	/**
		Send the head of the part and a part of its body, then break the connection (the client goes away).
	*/
	bool SendBrokenPart(const QByteArray& name, qint64 first, qint64 length, qint64 total, qint64 sent);

	QJsonObject GetState(const QByteArray& name, int* statusCode = nullptr);

	/**
		Wait while the server notices that the connection is broken and releases the file, give the state of the upload.
	*/
	bool WaitForIdle(const QByteArray& name, QJsonObject& state);

	QString GetDataPath(const QString& name) const;
};

