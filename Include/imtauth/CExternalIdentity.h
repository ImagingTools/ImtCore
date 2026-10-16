// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QDateTime>

// ImtCore includes
#include <imtauth/IExternalIdentity.h>


namespace imtauth
{


class CExternalIdentity: virtual public imtauth::IExternalIdentity
{
public:
	// reimplemented (imtauth::IExternalIdentity)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QByteArray GetProvider() const override;
	virtual void SetProvider(const QByteArray& provider) override;
	virtual QString GetExternalSubject() const override;
	virtual void SetExternalSubject(const QString& subject) override;
	virtual QString GetExternalEmail() const override;
	virtual void SetExternalEmail(const QString& email) override;
	virtual QDateTime GetLinkedAt() const override;
	virtual void SetLinkedAt(const QDateTime& linkedAt) override;
	virtual QDateTime GetLastAuthAt() const override;
	virtual void SetLastAuthAt(const QDateTime& lastAuthAt) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_id;
	QByteArray m_userId;
	QByteArray m_provider;
	QString m_externalSubject;
	QString m_externalEmail;
	QDateTime m_linkedAt;
	QDateTime m_lastAuthAt;
};


} // namespace imtauth
