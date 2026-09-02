// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QDateTime>

// ImtCore includes
#include <imtauth/IOidcTokenInfo.h>


namespace imtauth
{


class COidcTokenInfo: virtual public imtauth::IOidcTokenInfo
{
public:
	// reimplemented (imtauth::IOidcTokenInfo)
	virtual QByteArray GetTokenId() const override;
	virtual void SetTokenId(const QByteArray& tokenId) override;
	virtual QByteArray GetClientId() const override;
	virtual void SetClientId(const QByteArray& clientId) override;
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QString GetScope() const override;
	virtual void SetScope(const QString& scope) override;
	virtual OidcTokenType GetTokenType() const override;
	virtual void SetTokenType(OidcTokenType tokenType) override;
	virtual QDateTime GetExpirationDate() const override;
	virtual void SetExpirationDate(const QDateTime& expirationDate) override;
	virtual bool IsRevoked() const override;
	virtual void SetRevoked(bool revoked) override;
	virtual QDateTime GetCreatedAt() const override;
	virtual void SetCreatedAt(const QDateTime& createdAt) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_tokenId;
	QByteArray m_clientId;
	QByteArray m_userId;
	QString m_scope;
	OidcTokenType m_tokenType = OTT_ACCESS;
	QDateTime m_expirationDate;
	bool m_revoked = false;
	QDateTime m_createdAt;
};


} // namespace imtauth


