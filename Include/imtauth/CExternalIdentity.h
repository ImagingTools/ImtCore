// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IExternalIdentity.h>


namespace imtauth
{


class CExternalIdentity: virtual public IExternalIdentity
{
public:
	// reimplemented (imtauth::IExternalIdentity)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QByteArray GetProviderId() const override;
	virtual void SetProviderId(const QByteArray& providerId) override;
	virtual QString GetSubject() const override;
	virtual void SetSubject(const QString& subject) override;
	virtual QString GetEmail() const override;
	virtual void SetEmail(const QString& email) override;
	virtual QDateTime GetLinkedAt() const override;
	virtual void SetLinkedAt(const QDateTime& linkedAt) override;
	virtual QDateTime GetLastSignInAt() const override;
	virtual void SetLastSignInAt(const QDateTime& lastSignInAt) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_id;
	QByteArray m_userId;
	QByteArray m_providerId;
	QString m_subject;
	QString m_email;
	QDateTime m_linkedAt;
	QDateTime m_lastSignInAt;
};


} // namespace imtauth


