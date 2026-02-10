// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IUserVerification.h>


namespace imtauth
{


class CUserVerification: virtual public IUserVerification
{
public:
	// reimplemented (iser::IUserVerification)
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QByteArray GetVerificationCode() const override;
	virtual void SetVerificationCode(const QByteArray& code) override;
	virtual QDateTime GetCreationDate() const override;
	virtual void SetCreationDate(const QDateTime& creationDate) override;
	virtual QDateTime GetExpirationDate() const override;
	virtual void SetExpirationDate(const QDateTime& expirationDate) override;
	virtual bool IsExpired() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_userId;
	QByteArray m_verificationCode;
	QDateTime m_creationDate;
	QDateTime m_expirationDate;
};

typedef imtbase::TIdentifiableWrap<CUserVerification> CIdentifiableUserVerificationInfo;


} // namespace imtauth


