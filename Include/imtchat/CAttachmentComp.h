// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtchat/IAttachment.h>


namespace imtchat
{


class CAttachmentComp:
		public icomp::CComponentBase,
		virtual public IAttachment
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAttachmentComp)
		I_REGISTER_INTERFACE(IAttachment);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT

	// reimplemented (imtchat::IAttachment)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QString GetFileName() const override;
	virtual void SetFileName(const QString& fileName) override;
	virtual QString GetMimeType() const override;
	virtual void SetMimeType(const QString& mimeType) override;
	virtual QByteArray GetData() const override;
	virtual void SetData(const QByteArray& data) override;
	virtual qint64 GetFileSize() const override;
	virtual void SetFileSize(qint64 fileSize) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_id;
	QString m_fileName;
	QString m_mimeType;
	QByteArray m_data;
	qint64 m_fileSize = 0;
	QString m_createdAt;
};


} // namespace imtchat
