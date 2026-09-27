// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/COidcAuthorizationCode.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IOidcAuthorizationCode)

QByteArray COidcAuthorizationCode::GetCode() const
{
	return m_code;
}


void COidcAuthorizationCode::SetCode(const QByteArray& code)
{
	if (m_code != code){
		istd::CChangeNotifier changeNotifier(this);

		m_code = code;
	}
}


QByteArray COidcAuthorizationCode::GetClientId() const
{
	return m_clientId;
}


void COidcAuthorizationCode::SetClientId(const QByteArray& clientId)
{
	if (m_clientId != clientId){
		istd::CChangeNotifier changeNotifier(this);

		m_clientId = clientId;
	}
}


QByteArray COidcAuthorizationCode::GetUserId() const
{
	return m_userId;
}


void COidcAuthorizationCode::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


QString COidcAuthorizationCode::GetScope() const
{
	return m_scope;
}


void COidcAuthorizationCode::SetScope(const QString& scope)
{
	if (m_scope != scope){
		istd::CChangeNotifier changeNotifier(this);

		m_scope = scope;
	}
}


QString COidcAuthorizationCode::GetRedirectUri() const
{
	return m_redirectUri;
}


void COidcAuthorizationCode::SetRedirectUri(const QString& redirectUri)
{
	if (m_redirectUri != redirectUri){
		istd::CChangeNotifier changeNotifier(this);

		m_redirectUri = redirectUri;
	}
}


QDateTime COidcAuthorizationCode::GetExpirationDate() const
{
	return m_expirationDate;
}


void COidcAuthorizationCode::SetExpirationDate(const QDateTime& expirationDate)
{
	if (m_expirationDate != expirationDate){
		istd::CChangeNotifier changeNotifier(this);

		m_expirationDate = expirationDate;
	}
}


QByteArray COidcAuthorizationCode::GetCodeChallenge() const
{
	return m_codeChallenge;
}


void COidcAuthorizationCode::SetCodeChallenge(const QByteArray& codeChallenge)
{
	if (m_codeChallenge != codeChallenge){
		istd::CChangeNotifier changeNotifier(this);

		m_codeChallenge = codeChallenge;
	}
}


QByteArray COidcAuthorizationCode::GetCodeChallengeMethod() const
{
	return m_codeChallengeMethod;
}


void COidcAuthorizationCode::SetCodeChallengeMethod(const QByteArray& method)
{
	if (m_codeChallengeMethod != method){
		istd::CChangeNotifier changeNotifier(this);

		m_codeChallengeMethod = method;
	}
}


bool COidcAuthorizationCode::IsUsed() const
{
	return m_used;
}


void COidcAuthorizationCode::SetUsed(bool used)
{
	if (m_used != used){
		istd::CChangeNotifier changeNotifier(this);

		m_used = used;
	}
}


// reimplemented (iser::ISerializable)

bool COidcAuthorizationCode::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag codeTag("Code", "Authorization code");
	retVal = retVal && archive.BeginTag(codeTag);
	retVal = retVal && archive.Process(m_code);
	retVal = retVal && archive.EndTag(codeTag);

	static iser::CArchiveTag clientIdTag("ClientId", "Client identifier");
	retVal = retVal && archive.BeginTag(clientIdTag);
	retVal = retVal && archive.Process(m_clientId);
	retVal = retVal && archive.EndTag(clientIdTag);

	static iser::CArchiveTag userIdTag("UserId", "User identifier");
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	static iser::CArchiveTag scopeTag("Scope", "Granted scope");
	retVal = retVal && archive.BeginTag(scopeTag);
	retVal = retVal && archive.Process(m_scope);
	retVal = retVal && archive.EndTag(scopeTag);

	static iser::CArchiveTag redirectUriTag("RedirectUri", "Redirect URI");
	retVal = retVal && archive.BeginTag(redirectUriTag);
	retVal = retVal && archive.Process(m_redirectUri);
	retVal = retVal && archive.EndTag(redirectUriTag);

	static iser::CArchiveTag expirationTag("ExpirationDate", "Expiration timestamp");
	retVal = retVal && archive.BeginTag(expirationTag);
	iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_expirationDate);
	retVal = retVal && archive.EndTag(expirationTag);

	static iser::CArchiveTag challengeTag("CodeChallenge", "PKCE code challenge");
	retVal = retVal && archive.BeginTag(challengeTag);
	retVal = retVal && archive.Process(m_codeChallenge);
	retVal = retVal && archive.EndTag(challengeTag);

	static iser::CArchiveTag methodTag("CodeChallengeMethod", "PKCE code challenge method");
	retVal = retVal && archive.BeginTag(methodTag);
	retVal = retVal && archive.Process(m_codeChallengeMethod);
	retVal = retVal && archive.EndTag(methodTag);

	static iser::CArchiveTag usedTag("Used", "Whether code has been consumed");
	retVal = retVal && archive.BeginTag(usedTag);
	retVal = retVal && archive.Process(m_used);
	retVal = retVal && archive.EndTag(usedTag);

	return retVal;
}


// reimplemented (iser::IChangeable)

bool COidcAuthorizationCode::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const COidcAuthorizationCode* codePtr = dynamic_cast<const COidcAuthorizationCode*>(&object);
	if (codePtr == nullptr){
		return false;
	}

	m_code = codePtr->m_code;
	m_clientId = codePtr->m_clientId;
	m_userId = codePtr->m_userId;
	m_scope = codePtr->m_scope;
	m_redirectUri = codePtr->m_redirectUri;
	m_expirationDate = codePtr->m_expirationDate;
	m_codeChallenge = codePtr->m_codeChallenge;
	m_codeChallengeMethod = codePtr->m_codeChallengeMethod;
	m_used = codePtr->m_used;

	return true;
}


bool COidcAuthorizationCode::IsEqual(const IChangeable& object) const
{
	const COidcAuthorizationCode* sourcePtr = dynamic_cast<const COidcAuthorizationCode*>(&object);
	if (sourcePtr != nullptr){
		bool retVal = true;

		retVal = retVal && (m_code == sourcePtr->m_code);
		retVal = retVal && (m_clientId == sourcePtr->m_clientId);
		retVal = retVal && (m_userId == sourcePtr->m_userId);
		retVal = retVal && (m_scope == sourcePtr->m_scope);
		retVal = retVal && (m_redirectUri == sourcePtr->m_redirectUri);
		retVal = retVal && (m_expirationDate == sourcePtr->m_expirationDate);
		retVal = retVal && (m_codeChallenge == sourcePtr->m_codeChallenge);
		retVal = retVal && (m_codeChallengeMethod == sourcePtr->m_codeChallengeMethod);
		retVal = retVal && (m_used == sourcePtr->m_used);

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr COidcAuthorizationCode::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new COidcAuthorizationCode);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool COidcAuthorizationCode::ResetData(CompatibilityMode /*mode*/)
{
	m_code.clear();
	m_clientId.clear();
	m_userId.clear();
	m_scope.clear();
	m_redirectUri.clear();
	m_expirationDate = QDateTime();
	m_codeChallenge.clear();
	m_codeChallengeMethod.clear();
	m_used = false;

	return true;
}


} // namespace imtauth


