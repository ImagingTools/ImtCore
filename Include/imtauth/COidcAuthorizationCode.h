// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QDateTime>

// ImtCore includes
#include <imtauth/IOidcAuthorizationCode.h>


namespace imtauth
{


class COidcAuthorizationCode: virtual public imtauth::IOidcAuthorizationCode
{
public:
	// reimplemented (imtauth::IOidcAuthorizationCode)
	virtual QByteArray GetCode() const override;
	virtual void SetCode(const QByteArray& code) override;
	virtual QByteArray GetClientId() const override;
	virtual void SetClientId(const QByteArray& clientId) override;
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QString GetScope() const override;
	virtual void SetScope(const QString& scope) override;
	virtual QString GetRedirectUri() const override;
	virtual void SetRedirectUri(const QString& redirectUri) override;
	virtual QDateTime GetExpirationDate() const override;
	virtual void SetExpirationDate(const QDateTime& expirationDate) override;
	virtual QByteArray GetCodeChallenge() const override;
	virtual void SetCodeChallenge(const QByteArray& codeChallenge) override;
	virtual QByteArray GetCodeChallengeMethod() const override;
	virtual void SetCodeChallengeMethod(const QByteArray& method) override;
	virtual bool IsUsed() const override;
	virtual void SetUsed(bool used) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_code;
	QByteArray m_clientId;
	QByteArray m_userId;
	QString m_scope;
	QString m_redirectUri;
	QDateTime m_expirationDate;
	QByteArray m_codeChallenge;
	QByteArray m_codeChallengeMethod;
	bool m_used = false;
};


} // namespace imtauth


