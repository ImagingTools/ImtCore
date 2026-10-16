// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/COidcClient.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IOidcClient)

QByteArray COidcClient::GetClientId() const
{
	return m_clientId;
}


void COidcClient::SetClientId(const QByteArray& clientId)
{
	if (m_clientId != clientId){
		istd::CChangeNotifier changeNotifier(this);

		m_clientId = clientId;
	}
}


QByteArray COidcClient::GetClientSecretHash() const
{
	return m_clientSecretHash;
}


void COidcClient::SetClientSecretHash(const QByteArray& secretHash)
{
	if (m_clientSecretHash != secretHash){
		istd::CChangeNotifier changeNotifier(this);

		m_clientSecretHash = secretHash;
	}
}


QString COidcClient::GetClientName() const
{
	return m_clientName;
}


void COidcClient::SetClientName(const QString& name)
{
	if (m_clientName != name){
		istd::CChangeNotifier changeNotifier(this);

		m_clientName = name;
	}
}


QStringList COidcClient::GetRedirectUris() const
{
	return m_redirectUris;
}


void COidcClient::SetRedirectUris(const QStringList& uris)
{
	if (m_redirectUris != uris){
		istd::CChangeNotifier changeNotifier(this);

		m_redirectUris = uris;
	}
}


QByteArrayList COidcClient::GetGrantTypes() const
{
	return m_grantTypes;
}


void COidcClient::SetGrantTypes(const QByteArrayList& grantTypes)
{
	if (m_grantTypes != grantTypes){
		istd::CChangeNotifier changeNotifier(this);

		m_grantTypes = grantTypes;
	}
}


QByteArrayList COidcClient::GetScopes() const
{
	return m_scopes;
}


void COidcClient::SetScopes(const QByteArrayList& scopes)
{
	if (m_scopes != scopes){
		istd::CChangeNotifier changeNotifier(this);

		m_scopes = scopes;
	}
}


QDateTime COidcClient::GetCreatedAt() const
{
	return m_createdAt;
}


void COidcClient::SetCreatedAt(const QDateTime& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier changeNotifier(this);

		m_createdAt = createdAt;
	}
}


// reimplemented (iser::ISerializable)

bool COidcClient::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag clientIdTag("ClientId", "Client identifier");
	retVal = retVal && archive.BeginTag(clientIdTag);
	retVal = retVal && archive.Process(m_clientId);
	retVal = retVal && archive.EndTag(clientIdTag);

	static iser::CArchiveTag secretHashTag("ClientSecretHash", "Client secret hash");
	retVal = retVal && archive.BeginTag(secretHashTag);
	retVal = retVal && archive.Process(m_clientSecretHash);
	retVal = retVal && archive.EndTag(secretHashTag);

	static iser::CArchiveTag nameTag("ClientName", "Client name");
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_clientName);
	retVal = retVal && archive.EndTag(nameTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QStringList>(archive, m_redirectUris, "RedirectUris", "Redirect URIs");

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_grantTypes, "GrantTypes", "Grant types");

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_scopes, "Scopes", "Permission scopes");

	static iser::CArchiveTag createdAtTag("CreatedAt", "Creation timestamp");
	retVal = retVal && archive.BeginTag(createdAtTag);
	iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	return retVal;
}


// reimplemented (iser::IChangeable)

bool COidcClient::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const COidcClient* clientPtr = dynamic_cast<const COidcClient*>(&object);
	if (clientPtr == nullptr){
		return false;
	}

	m_clientId = clientPtr->m_clientId;
	m_clientSecretHash = clientPtr->m_clientSecretHash;
	m_clientName = clientPtr->m_clientName;
	m_redirectUris = clientPtr->m_redirectUris;
	m_grantTypes = clientPtr->m_grantTypes;
	m_scopes = clientPtr->m_scopes;
	m_createdAt = clientPtr->m_createdAt;

	return true;
}


bool COidcClient::IsEqual(const IChangeable& object) const
{
	const COidcClient* sourcePtr = dynamic_cast<const COidcClient*>(&object);
	if (sourcePtr != nullptr){
		bool retVal = true;

		retVal = retVal && (m_clientId == sourcePtr->m_clientId);
		retVal = retVal && (m_clientSecretHash == sourcePtr->m_clientSecretHash);
		retVal = retVal && (m_clientName == sourcePtr->m_clientName);
		retVal = retVal && (m_redirectUris == sourcePtr->m_redirectUris);
		retVal = retVal && (m_grantTypes == sourcePtr->m_grantTypes);
		retVal = retVal && (m_scopes == sourcePtr->m_scopes);
		retVal = retVal && (m_createdAt == sourcePtr->m_createdAt);

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr COidcClient::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new COidcClient);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool COidcClient::ResetData(CompatibilityMode /*mode*/)
{
	m_clientId.clear();
	m_clientSecretHash.clear();
	m_clientName.clear();
	m_redirectUris.clear();
	m_grantTypes.clear();
	m_scopes.clear();
	m_createdAt = QDateTime();

	return true;
}


} // namespace imtauth


