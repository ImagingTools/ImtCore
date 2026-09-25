// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imttag/ITagAssignment.h>


namespace imttag
{


class CTagAssignment: virtual public ITagAssignment
{
public:
	// reimplemented (imttag::ITagAssignment)
	virtual QByteArray GetTagId() const override;
	virtual void SetTagId(const QByteArray& tagId) override;
	virtual QByteArray GetEntityType() const override;
	virtual void SetEntityType(const QByteArray& entityType) override;
	virtual QByteArray GetEntityId() const override;
	virtual void SetEntityId(const QByteArray& entityId) override;
	virtual QDateTime GetCreatedAt() const override;
	virtual void SetCreatedAt(const QDateTime& createdAt) override;
	virtual QByteArray GetCreatedByUserId() const override;
	virtual void SetCreatedByUserId(const QByteArray& userId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_tagId;
	QByteArray m_entityType;
	QByteArray m_entityId;
	QDateTime m_createdAt;
	QByteArray m_createdByUserId;
};


} // namespace imttag


