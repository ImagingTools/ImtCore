// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <iser/ISerializable.h>


namespace imtchat
{


/**
	Interface representing a stored attachment (image, file, etc.).

	Attachments are stored in the database with their binary data and metadata.
	Messages reference attachments by their IDs.

	\ingroup imtchat
*/
class IAttachment: virtual public iser::ISerializable
{
public:
	/**
		Get the unique attachment identifier.
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set the unique attachment identifier.
	*/
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Get the original file name.
	*/
	virtual QString GetFileName() const = 0;

	/**
		Set the original file name.
	*/
	virtual void SetFileName(const QString& fileName) = 0;

	/**
		Get the MIME type (e.g. "image/png").
	*/
	virtual QString GetMimeType() const = 0;

	/**
		Set the MIME type.
	*/
	virtual void SetMimeType(const QString& mimeType) = 0;

	/**
		Get the binary data of the attachment.
	*/
	virtual QByteArray GetData() const = 0;

	/**
		Set the binary data of the attachment.
	*/
	virtual void SetData(const QByteArray& data) = 0;

	/**
		Get the file size in bytes.
	*/
	virtual qint64 GetFileSize() const = 0;

	/**
		Set the file size in bytes.
	*/
	virtual void SetFileSize(qint64 fileSize) = 0;

	/**
		Get the ISO 8601 creation timestamp.
	*/
	virtual QString GetCreatedAt() const = 0;

	/**
		Set the creation timestamp.
	*/
	virtual void SetCreatedAt(const QString& createdAt) = 0;
};


} // namespace imtchat
