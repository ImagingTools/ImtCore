// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QDateTime>

// ImtCore includes
#include <imtauth/IOidcClient.h>


namespace imtauth
{


class COidcClient: virtual public imtauth::IOidcClient
{
public:
	// reimplemented (imtauth::IOidcClient)
	virtual QByteArray GetClientId() const override;
	virtual void SetClientId(const QByteArray& clientId) override;
	virtual QByteArray GetClientSecretHash() const override;
	virtual void SetClientSecretHash(const QByteArray& secretHash) override;
	virtual QString GetClientName() const override;
	virtual void SetClientName(const QString& name) override;
	virtual QStringList GetRedirectUris() const override;
	virtual void SetRedirectUris(const QStringList& uris) override;
	virtual QByteArrayList GetGrantTypes() const override;
	virtual void SetGrantTypes(const QByteArrayList& grantTypes) override;
	virtual QByteArrayList GetScopes() const override;
	virtual void SetScopes(const QByteArrayList& scopes) override;
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
	QByteArray m_clientId;
	QByteArray m_clientSecretHash;
	QString m_clientName;
	QStringList m_redirectUris;
	QByteArrayList m_grantTypes;
	QByteArrayList m_scopes;
	QDateTime m_createdAt;
};


} // namespace imtauth


