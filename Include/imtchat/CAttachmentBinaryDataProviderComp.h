// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtrest/IBinaryDataProvider.h>
#include <imtrest/IBinaryDataController.h>
#include <imtchat/IAttachmentStorage.h>


namespace imtchat
{


/**
	Bridge component that exposes IAttachmentStorage as an HTTP binary
	data provider (GET) and controller (POST/DELETE).

	Register as BinaryDataProvider and BinaryDataController in the server
	configuration so that CHttpFileControllerServletComp can serve
	attachment uploads and downloads via the /files/* endpoint.
*/
class CAttachmentBinaryDataProviderComp:
		public icomp::CComponentBase,
		virtual public imtrest::IBinaryDataProvider,
		virtual public imtrest::IBinaryDataController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAttachmentBinaryDataProviderComp)
		I_REGISTER_INTERFACE(imtrest::IBinaryDataProvider)
		I_REGISTER_INTERFACE(imtrest::IBinaryDataController)
		I_ASSIGN(m_attachmentStorageCompPtr, "AttachmentStorage", "Storage service for binary attachments", false, "AttachmentStorage");
	I_END_COMPONENT

	// reimplemented (imtrest::IBinaryDataProvider)
	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const override;

	// reimplemented (imtrest::IBinaryDataController)
	virtual bool SetData(const QByteArray& data, QByteArray& dataId) const override;
	virtual bool RemoveData(const QByteArray& dataId) const override;
	virtual bool EnsureDataExists(const QByteArray& dataId) const override;

private:
	/**
		Strip file extension from a dataId to obtain the plain UUID
		used by IAttachmentStorage.
	*/
	static QByteArray StripExtension(const QByteArray& dataId);

	/**
		Detect MIME type from a file name extension.
	*/
	static QString DetectMimeType(const QString& fileName);

	I_REF(imtchat::IAttachmentStorage, m_attachmentStorageCompPtr);
};


} // namespace imtchat
