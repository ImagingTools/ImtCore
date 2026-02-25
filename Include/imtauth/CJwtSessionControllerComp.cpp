// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CJwtSessionControllerComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QCryptographicHash>
#include <QtCore/QFileInfo>

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
		return imtauth::IJwtSessionController::JS_INVALID;
	}

	QByteArray headerBase64 = parts[0];
	QByteArray payloadBase64 = parts[1];
	QByteArray signatureBase64 = parts[2];

	// Check signature
	QByteArray expectedSignature = CreateSignature(headerBase64, payloadBase64);

	if (signatureBase64 != expectedSignature){
		qWarning() << "Invalid JWT signature!";
		return imtauth::IJwtSessionController::JS_INVALID;
	}

	QJsonObject payloadObj = JsonObjectFromBase64(payloadBase64);
	if (!payloadObj.contains("exp") || !payloadObj.contains("sessionId")){
		return imtauth::IJwtSessionController::JS_INVALID;
	}

	QByteArray sessionId = payloadObj["sessionId"].toString().toUtf8();
	if (!ValidateSession(sessionId)){
		if (!RemoveSession(sessionId)){
			SendWarningMessage(0, QString("Unable to remove session '%1' after JWT validation").arg(qPrintable(sessionId)), "CJwtSessionControllerComp");
		}

		return imtauth::IJwtSessionController::JS_INVALID;
	}

	qint64 exp = payloadObj["exp"].toInt();
	if (exp < QDateTime::currentSecsSinceEpoch()){
		qWarning() << "JWT token has expired!";
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

	QByteArray sessionId = GetSessionIdByRefreshToken(refreshToken);
	ISessionSharedPtr sessionInfoPtr = GetSession(sessionId);
	if (!sessionInfoPtr.IsValid()){
		return false;
	}

	istd::TUniqueInterfacePtr<imtauth::ISession> clonedSessionInfo;
	clonedSessionInfo.MoveCastedPtr(sessionInfoPtr->CloneMe());
	if (!clonedSessionInfo.IsValid()){
		return false;
	}

	if (!RemoveSession(sessionId)){
		SendErrorMessage(0, QString("Unable to remove old session '%1' from collection")
							 .arg(qPrintable(refreshToken)), "CJwtSessionControllerComp");
		return false;
	}

	QByteArray newRefreshToken = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	clonedSessionInfo->SetToken(newRefreshToken);

	QByteArray newSessionId = InsertNewSession(*clonedSessionInfo.GetPtr());
	if (newSessionId.isEmpty()){
		SendErrorMessage(0, QString("Unable to insert session '%1' to collection")
								.arg(qPrintable(newRefreshToken)), "CJwtSessionControllerComp");
		return false;
	}

	QByteArray userId = sessionInfoPtr->GetUserId();

	QByteArray jwt = GenerateJwt(newSessionId, userId);
	if (jwt.isEmpty()){
		SendErrorMessage(0, QString("Unable to refresh session for user '%1'. Error: JWT is invalid").arg(qPrintable(userId)), "CJwtSessionControllerComp");
		return false;
	}

	userSession.accessToken = jwt;
	userSession.refreshToken = newRefreshToken;
	userSession.userId = userId;

	return true;
}


bool CJwtSessionControllerComp::CreateNewSession(
			const QByteArray& userId,
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

	QByteArray refreshToken = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	sessionInfoPtr->SetToken(refreshToken);

	QByteArray sessionId = InsertNewSession(*sessionInfoPtr.GetPtr());
	if (sessionId.isEmpty()){
		SendErrorMessage(0, QString("Unable to insert session '%1' to collection").arg(qPrintable(refreshToken)), "CJwtSessionControllerComp");
		return false;
	}

	userSession.userId = userId;

	QByteArray jwt = GenerateJwt(sessionId, userId);
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


QByteArray CJwtSessionControllerComp::GenerateJwt(const QByteArray& sessionId, const QByteArray& userId) const
{
	qint64 expiresAt = QDateTime::currentSecsSinceEpoch() + *m_jwtLifetimeAttrPtr;
	QJsonObject header;
	header["alg"] = "HS256";
	header["typ"] = "JWT";

	QJsonObject payload;
	payload["userId"] = QString(userId);
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
