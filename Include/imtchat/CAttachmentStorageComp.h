// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtchat/IAttachmentStorage.h>
#include <imtchat/IAttachment.h>
#include <imtbase/IObjectCollection.h>


namespace imtchat
{


class CAttachmentStorageComp:
		public icomp::CComponentBase,
		virtual public IAttachmentStorage
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAttachmentStorageComp)
		I_REGISTER_INTERFACE(IAttachmentStorage);
		I_ASSIGN(m_attachmentCollectionCompPtr, "AttachmentCollection", "Collection of attachments backed by SQL", true, "AttachmentCollection");
		I_ASSIGN(m_attachmentFactCompPtr, "AttachmentFactory", "Factory for creating attachment objects", true, "AttachmentFactory");
	I_END_COMPONENT

	// reimplemented (imtchat::IAttachmentStorage)
	virtual QByteArray StoreAttachment(
				const QByteArray& data,
				const QString& fileName,
				const QString& mimeType) override;
	virtual bool GetAttachment(
				const QByteArray& attachmentId,
				QByteArray& data,
				QString& fileName,
				QString& mimeType) const override;
	virtual bool DeleteAttachment(const QByteArray& attachmentId) override;

private:
	I_REF(imtbase::IObjectCollection, m_attachmentCollectionCompPtr);
	I_FACT(IAttachment, m_attachmentFactCompPtr);
};


} // namespace imtchat
