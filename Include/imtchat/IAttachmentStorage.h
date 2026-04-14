// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>


namespace imtchat
{


/**
	Interface for storing and retrieving binary attachments.

	Attachments are stored by their unique ID and can be retrieved
	as binary data with associated metadata (file name, MIME type).

	\ingroup imtchat
*/
class IAttachmentStorage: virtual public istd::IPolymorphic
{
public:
	/**
		Store an attachment and return its generated ID.

		\param data Binary content of the attachment.
		\param fileName Original file name.
		\param mimeType MIME type (e.g. "image/png").
		\return Generated attachment ID, or empty QByteArray on failure.
	*/
	virtual QByteArray StoreAttachment(
				const QByteArray& data,
				const QString& fileName,
				const QString& mimeType) = 0;

	/**
		Retrieve an attachment by its ID.

		\param attachmentId Unique attachment identifier.
		\param[out] data Binary content.
		\param[out] fileName Original file name.
		\param[out] mimeType MIME type.
		\return true if the attachment was found and loaded.
	*/
	virtual bool GetAttachment(
				const QByteArray& attachmentId,
				QByteArray& data,
				QString& fileName,
				QString& mimeType) const = 0;

	/**
		Delete an attachment by its ID.

		\param attachmentId Unique attachment identifier.
		\return true if the attachment was deleted.
	*/
	virtual bool DeleteAttachment(const QByteArray& attachmentId) = 0;
};


} // namespace imtchat
