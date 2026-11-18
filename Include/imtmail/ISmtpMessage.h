// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QString>

// ACF includes
#include <iser/ISerializable.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtmail
{


class ISmtpMessage: virtual public iser::ISerializable
{
public:
	struct Attachment
	{
		QString fileName;
		QByteArray data;

		bool operator==(const Attachment& other) const
		{
			return fileName == other.fileName && data == other.data;
		}

		bool Serialize(iser::IArchive& archive)
		{
			iser::CArchiveTag fileNameTag("FileName", "File name", iser::CArchiveTag::TT_LEAF);
			iser::CArchiveTag dataTag("Data", "Data", iser::CArchiveTag::TT_LEAF);

			bool retVal = archive.BeginTag(fileNameTag);
			retVal = retVal && archive.Process(fileName);
			retVal = retVal && archive.EndTag(fileNameTag);
			retVal = retVal && archive.BeginTag(dataTag);
			retVal = retVal && archive.Process(data);
			retVal = retVal && archive.EndTag(dataTag);

			return retVal;
		}
	};

	using Attachments = QList<Attachment>;

	virtual QString GetFrom() const = 0;
	virtual void SetFrom(const QString& from) = 0;
	virtual QString GetTo() const = 0;
	virtual void SetTo(const QString& to) = 0;
	virtual QString GetSubject() const = 0;
	virtual void SetSubject(const QString& subject) = 0;
	virtual QString GetBody() const = 0;
	virtual void SetBody(const QString& body) = 0;

	/**
		Get the files attached to the message, in the order they were added.
	*/
	virtual Attachments GetAttachments() const = 0;

	/**
		Attach a file to the message. Its content type is taken from the name and the data of the file.
	*/
	virtual void AddAttachment(const QString& fileName, const QByteArray& data) = 0;
};


} // namespace imtmail

