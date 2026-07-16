// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CJwtSessionControllerComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QCryptographicHash>
#include <QtCore/QFileInfo>
#include <QtCore/QMutexLocker>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtauth/ISession.h>


namespace imtauth
{


// protected methods

// reimplemented (imtauth::IJwtSessionController)

bool CJwtSessionControllerComp::ValidateSession(const QByteArray& sessionId) const
{
	ISessionSharedPtr sessionInfoPtr = GetSession(sessionId);
	if (!sessionInfoPtr.IsValid()){
		return false;
	}

	QDateTime currentDate = QDateTime::currentDateTimeUtc();
	QDateTime expirationDate = sessionInfoPtr->GetExpirationDate();

	return currentDate < expirationDate;
}


imtauth::IJwtSessionController::JwtState CJwtSessionControllerComp::ValidateJwt(const QByteArray& token) const
{
	QByteArrayList parts = token.split('.');
	if (parts.size() != 3){
		SendWarningMessage(0, QString("JWT rejected: malformed token, expected 3 dot-separated parts, got %1")
								.arg(parts.size()), "CJwtSessionControllerComp");
		return imtauth::IJwtSessionController::JS_INVALID;
	}

	QByteArray headerBase64 = parts[0];
	QByteArray payloadBase64 = parts[1];
	QByteArray signatureBase64 = parts[2];

	// Check signature
	QByteArray expectedSignature = CreateSignature(headerBase64, payloadBase64);

	if (signatureBase64 != expectedSignature){
		SendWarningMessage(0, "JWT rejected: signature mismatch (forged/tampered token or secret-key rotation)", "CJwtSessionControllerComp");
		return imtauth::IJwtSessionController::JS_INVALID;
	}

	QJsonObject payloadObj = JsonObjectFromBase64(payloadBase64);
	if (!payloadObj.contains("exp") || !payloadObj.contains("sessionId")){
		SendWarningMessage(0, QString("JWT rejected: payload is missing required claim(s), has 'exp': %1, has 'sessionId': %2")
								.arg(payloadObj.contains("exp") ? "yes" : "no")
								.arg(payloadObj.contains("sessionId") ? "yes" : "no"), "CJwtSessionControllerComp");
		return imtauth::IJwtSessionController::JS_INVALID;
	}

	QByteArray sessionId = payloadObj["sessionId"].toString().toUtf8();

	// Check the token's own expiration before touching the session store. A
	// token that is expired by time must be reported as JS_EXPIRED regardless
	// of whether its session record still exists, so the client is told to
	// refresh rather than told the token is permanently invalid.
	qint64 exp = payloadObj["exp"].toInt();
	if (exp < QDateTime::currentSecsSinceEpoch()){
		SendWarningMessage(0, QString("JWT rejected: token expired by time, exp = %1 for session-ID '%2'")
								.arg(exp).arg(qPrintable(sessionId)), "CJwtSessionControllerComp");
		return imtauth::IJwtSessionController::JS_EXPIRED;
	}

	// Read-only check: a missing or expired session is reported as JS_EXPIRED
	// (prompting the client to refresh) rather than removed here. Deleting the
	// session as a side-effect of validation would destroy the refresh token
	// along with it, making it impossible for the client to recover; session
	// cleanup belongs to RefreshToken()/a maintenance task, not to validation.
	//
	// GetSession() is called directly (instead of ValidateSession()) so the two
	// distinct failure reasons - no session record at all vs. an existing but
	// expired session - can be logged and diagnosed separately.
	ISessionSharedPtr sessionInfoPtr = GetSession(sessionId);
	if (!sessionInfoPtr.IsValid()){
		SendWarningMessage(0, QString("JWT rejected: no session record found for session-ID '%1' "
								"(already removed by cleanup, or the JWT references a session that never existed)")
								.arg(qPrintable(sessionId)), "CJwtSessionControllerComp");
		return imtauth::IJwtSessionController::JS_EXPIRED;
	}

	QDateTime currentDate = QDateTime::currentDateTimeUtc();
	QDateTime sessionExpirationDate = sessionInfoPtr->GetExpirationDate();
	if (currentDate >= sessionExpirationDate){
		SendWarningMessage(0, QString("JWT rejected: session '%1' expired at %2 (current time %3) - "
								"the client was idle longer than RefreshTokenLifetime")
								.arg(qPrintable(sessionId), sessionExpirationDate.toString(Qt::ISODate), currentDate.toString(Qt::ISODate)),
								"CJwtSessionControllerComp");
		return imtauth::IJwtSessionController::JS_EXPIRED;
	}

	return imtauth::IJwtSessionController::JS_OK;
}


bool CJwtSessionControllerComp::RefreshToken(
			const QByteArray& refreshToken,
			imtauth::IJwtSessionController::UserSession& userSession) const
{
	if (!m_sessionCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SessionCollection' was not set", "CJwtSessionControllerComp");
		return false;
	}

	// Serialize rotation: two concurrent refresh calls for the same session
	// (e.g. several requests failing with an expired token at once) must not
	// race on the same session record.
	QMutexLocker refreshLocker(&m_refreshMutex);

	QByteArray sessionId = GetSessionIdByRefreshToken(refreshToken);
	if (sessionId.isEmpty()){
		SendWarningMessage(0, "Refresh rejected: no session found for the given refresh token (unknown, already rotated, or already used once)",
								"CJwtSessionControllerComp");
		return false;
	}

	ISessionSharedPtr sessionInfoPtr = GetSession(sessionId);
	if (!sessionInfoPtr.IsValid()){
		SendWarningMessage(0, QString("Refresh rejected: session-ID '%1' resolved from the refresh token but the session record is gone (race with cleanup?)")
								.arg(qPrintable(sessionId)), "CJwtSessionControllerComp");
		return false;
	}

	QDateTime currentDate = QDateTime::currentDateTimeUtc();
	if (currentDate >= sessionInfoPtr->GetExpirationDate()){
		// The session itself has expired (the user has been away longer than
		// RefreshTokenLifetime) - refuse to rotate it and clean it up.
		SendWarningMessage(0, QString("Refresh rejected: session '%1' expired at %2 - the user was away longer than "
								"RefreshTokenLifetime; removing the dead session")
								.arg(qPrintable(sessionId), sessionInfoPtr->GetExpirationDate().toString(Qt::ISODate)),
								"CJwtSessionControllerComp");
		if (!RemoveSession(sessionId)){
			SendWarningMessage(0, QString("Unable to remove expired session '%1'").arg(qPrintable(sessionId)), "CJwtSessionControllerComp");
		}

		return false;
	}

	istd::TUniqueInterfacePtr<imtauth::ISession> clonedSessionInfo;
	clonedSessionInfo.MoveCastedPtr(sessionInfoPtr->CloneMe());
	if (!clonedSessionInfo.IsValid()){
		SendErrorMessage(0, QString("Unable to clone session '%1' for rotation").arg(qPrintable(sessionId)), "CJwtSessionControllerComp");
		return false;
	}

	QByteArray newRefreshToken = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	clonedSessionInfo->SetToken(newRefreshToken);

	// Sliding expiration: an actively-used session keeps getting extended
	// instead of dying exactly RefreshTokenLifetime after the original login.
	clonedSessionInfo->SetExpirationDate(currentDate.addSecs(*m_refreshTokenLifetimeAttrPtr));

	// Update the session record in place (same session-ID) instead of
	// remove-then-insert. This means any JWT already issued for this session -
	// including ones still in flight from concurrent requests, or the JWT the
	// client is about to discard - keeps validating for the remainder of its
	// own lifetime, instead of being invalidated the instant a refresh happens.
	if (!m_sessionCollectionCompPtr->SetObjectData(sessionId, *clonedSessionInfo.GetPtr())){
		SendErrorMessage(0, QString("Unable to update session '%1' in collection")
								.arg(qPrintable(refreshToken)), "CJwtSessionControllerComp");
		return false;
	}

	QByteArray userId = sessionInfoPtr->GetUserId();

	QByteArray jwt = GenerateJwt(sessionId, userId, sessionInfoPtr->GetTenantId());
	if (jwt.isEmpty()){
		SendErrorMessage(0, QString("Unable to refresh session for user '%1'. Error: JWT is invalid").arg(qPrintable(userId)), "CJwtSessionControllerComp");
		return false;
	}

	userSession.accessToken = jwt;
	userSession.refreshToken = newRefreshToken;
	userSession.userId = userId;
	userSession.tenantId = sessionInfoPtr->GetTenantId();

	return true;
}


bool CJwtSessionControllerComp::CreateNewSession(
			const QByteArray& userId,
			const QByteArray& tenantId,
			imtauth::IJwtSessionController::UserSession& userSession) const
{
	if (!m_sessionFactoryCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SessionFactory' was not set", "CJwtSessionControllerComp");
		return false;
	}

	imtauth::ISessionUniquePtr sessionInfoPtr = m_sessionFactoryCompPtr.CreateInstance();

	QDateTime creationDate = QDateTime::currentDateTimeUtc();
	sessionInfoPtr->SetCreationDate(creationDate);

	QDateTime expirationDate = creationDate.addSecs(*m_refreshTokenLifetimeAttrPtr);
	sessionInfoPtr->SetExpirationDate(expirationDate);

	sessionInfoPtr->SetUserId(userId);
	sessionInfoPtr->SetTenantId(tenantId);

	QByteArray refreshToken = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	sessionInfoPtr->SetToken(refreshToken);

	QByteArray sessionId = InsertNewSession(*sessionInfoPtr.GetPtr());
	if (sessionId.isEmpty()){
		SendErrorMessage(0, QString("Unable to insert session '%1' to collection").arg(qPrintable(refreshToken)), "CJwtSessionControllerComp");
		return false;
	}

	userSession.userId = userId;
	userSession.tenantId = tenantId;

	QByteArray jwt = GenerateJwt(sessionId, userId, tenantId);
	if (jwt.isEmpty()){
		SendErrorMessage(0, QString("Unable to create a new session for user '%1'. Error: JWT is invalid").arg(qPrintable(userId)), "CJwtSessionControllerComp");
		return false;
	}

	userSession.accessToken = jwt;
	userSession.refreshToken = refreshToken;

	return true;
}


ISessionSharedPtr CJwtSessionControllerComp::GetSession(const QByteArray& sessionId) const
{
	if (!m_sessionCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SessionCollection' was not set", "CJwtSessionControllerComp");
		return nullptr;
	}

	ISessionSharedPtr sessionInfoPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_sessionCollectionCompPtr->GetObjectData(sessionId, dataPtr)){
		sessionInfoPtr.SetCastedPtr<istd::IChangeable>(dataPtr);
	}

	return sessionInfoPtr;
}


bool CJwtSessionControllerComp::RemoveSession(const QByteArray& sessionId) const
{
	if (!m_sessionCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SessionCollection' was not set", "CJwtSessionControllerComp");
		return false;
	}

	if (sessionId.isEmpty()){
		return false;
	}

	imtbase::ICollectionInfo::Ids elementIds;
	elementIds << sessionId;

	return m_sessionCollectionCompPtr->RemoveElements(elementIds);
}


QByteArray CJwtSessionControllerComp::GetUserFromJwt(const QByteArray& jwt) const
{
	QByteArrayList parts = jwt.split('.');
	if (parts.size() != 3){
		return QByteArray();
	}

	QByteArray payloadBase64 = parts[1];

	QJsonObject payloadObj = JsonObjectFromBase64(payloadBase64);
	if (!payloadObj.contains("userId")){
		return QByteArray();
	}

	QString userId = payloadObj["userId"].toString();

	return userId.toUtf8();
}


QByteArray CJwtSessionControllerComp::GetSessionFromJwt(const QByteArray& jwt) const
{
	QByteArrayList parts = jwt.split('.');
	if (parts.size() != 3){
		return QByteArray();
	}

	QByteArray payloadBase64 = parts[1];

	QJsonObject payloadObj = JsonObjectFromBase64(payloadBase64);
	if (!payloadObj.contains("sessionId")){
		return QByteArray();
	}

	QString sessionId = payloadObj["sessionId"].toString();

	return sessionId.toUtf8();
}


QByteArray CJwtSessionControllerComp::GetTenantFromJwt(const QByteArray& jwt) const
{
	QByteArrayList parts = jwt.split('.');
	if (parts.size() != 3){
		return QByteArray();
	}

	QByteArray payloadBase64 = parts[1];

	QJsonObject payloadObj = JsonObjectFromBase64(payloadBase64);
	if (!payloadObj.contains("tenantId")){
		return QByteArray();
	}

	QString tenantId = payloadObj["tenantId"].toString();

	return tenantId.toUtf8();
}


// private methods

QByteArray CJwtSessionControllerComp::JsonObjectToBase64(const QJsonObject& object) const
{
	return QJsonDocument(object).toJson(QJsonDocument::Compact)
				.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}


QJsonObject CJwtSessionControllerComp::JsonObjectFromBase64(const QByteArray& data) const
{
	QByteArray json = QByteArray::fromBase64(
				data,
				QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
	QJsonDocument document = QJsonDocument::fromJson(json);
	return document.object();
}


QByteArray CJwtSessionControllerComp::InsertNewSession(const istd::IChangeable& object) const
{
	if (!m_sessionCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SessionCollection' was not set", "CJwtSessionControllerComp");
		return QByteArray();
	}

	return m_sessionCollectionCompPtr->InsertNewObject("Session", "", "", &object);
}


QByteArray CJwtSessionControllerComp::CreateSignature(
			const QByteArray& headerBase64,
			const QByteArray& payloadBase64) const
{
	if (!m_secretKeyParamCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SecretKeyParam' was not set", "CJwtSessionControllerComp");
		return QByteArray();
	}

	QString key = m_secretKeyParamCompPtr->GetName();

	return QCryptographicHash::hash(headerBase64 + "." + payloadBase64 + key.toUtf8(),
							 QCryptographicHash::Sha256).toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}


QByteArray CJwtSessionControllerComp::GenerateJwt(const QByteArray& sessionId, const QByteArray& userId, const QByteArray& tenantId) const
{
	qint64 expiresAt = QDateTime::currentSecsSinceEpoch() + *m_jwtLifetimeAttrPtr;
	QJsonObject header;
	header["alg"] = "HS256";
	header["typ"] = "JWT";

	QJsonObject payload;
	payload["userId"] = QString(userId);
	payload["tenantId"] = QString(tenantId);
	payload["sessionId"] = QString(sessionId);
	payload["exp"] = expiresAt;

	QByteArray headerBase64 = JsonObjectToBase64(header);
	QByteArray payloadBase64 = JsonObjectToBase64(payload);
	QByteArray signature = CreateSignature(headerBase64, payloadBase64);
	if (signature.isEmpty()){
		SendErrorMessage(0, "Unable to generate JWT. Error: Signature is invalid", "CJwtSessionControllerComp");
		return QByteArray();
	}

	return headerBase64 + "." + payloadBase64 + "." + signature;
}


QByteArray CJwtSessionControllerComp::GetSessionIdByRefreshToken(const QByteArray& refreshToken) const
{
	if (!m_sessionCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SessionCollection' was not set", "CJwtSessionControllerComp");
		return QByteArray();
	}

	iprm::CParamsSet filterParams;

	iprm::CTextParam textParam;
	textParam.SetText(refreshToken);

	filterParams.SetEditableParameter("RefreshToken", &textParam);

	imtbase::ICollectionInfo::Ids elementIds = m_sessionCollectionCompPtr->GetElementIds(0, -1, &filterParams);

	if (!elementIds.isEmpty()){
		return elementIds[0];
	}

	return QByteArray();
}


} // namespace imtauth
