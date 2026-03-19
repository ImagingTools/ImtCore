// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CHttpGraphQLServletComp.h>

#include <iterator>

// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QReadWriteLock>
#include <QtCore/QScopeGuard>

// ACF includes
#include <iser/CJsonMemWriteArchive.h>
#include <iprm/TParamsPtr.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtrest/IProtocolEngine.h>


namespace imtservergql
{

namespace
{


constexpr qint64 s_jwtContextCacheTtlMs = 5000;
constexpr int s_jwtContextCacheLimit = 256;


bool IsJwtToken(const QByteArray& token)
{
	return !token.isEmpty() && !token.startsWith("imt_pat_");
}


QByteArray GetTokenCacheKey(const QByteArray& token)
{
	return QCryptographicHash::hash(token, QCryptographicHash::Sha256);
}


qint64 GetJwtExpirationTimeMs(const QByteArray& token)
{
	QByteArrayList parts = token.split('.');
	if (parts.size() != 3){
		return 0;
	}

	QByteArray payload = QByteArray::fromBase64(parts[1], QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
	QJsonParseError error;
	QJsonDocument document = QJsonDocument::fromJson(payload, &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()){
		return 0;
	}

	QJsonValue expValue = document.object().value("exp");
	if (!expValue.isDouble()){
		return 0;
	}

	qint64 expSecs = static_cast<qint64>(expValue.toDouble());
	if (expSecs <= 0){
		return 0;
	}

	return expSecs * 1000;
}


class CGqlContextCache
{
public:
	static CGqlContextCache& Instance()
	{
		static CGqlContextCache instance;
		return instance;
	}

	bool TryCreateRequestContext(
				const QByteArray& token,
				const QByteArray& productId,
				const imtrest::CHttpServletCompBase::HeadersMap& headers,
				imtgql::IGqlContextSharedPtr& gqlContextPtr) const
	{
		if (!IsJwtToken(token)){
			return false;
		}

		CachedContextItem item;
		{
			QReadLocker locker(&m_lock);
			auto itemIter = m_cacheItems.constFind(GetTokenCacheKey(token));
			if (itemIter == m_cacheItems.cend()){
				return false;
			}

			item = itemIter.value();
		}

		if (!item.prototypeContextPtr.IsValid() || item.expirationTimeMs <= QDateTime::currentMSecsSinceEpoch()){
			Remove(token);
			return false;
		}

		gqlContextPtr.MoveCastedPtr(item.prototypeContextPtr->CloneMe());
		if (!gqlContextPtr.IsValid()){
			Remove(token);
			return false;
		}

		gqlContextPtr->SetToken(token);
		gqlContextPtr->SetProductId(productId);
		gqlContextPtr->SetHeaders(headers);

		return true;
	}

	void Insert(const QByteArray& token, const imtgql::IGqlContext* gqlContextPtr) const
	{
		if (!IsJwtToken(token) || gqlContextPtr == nullptr){
			return;
		}

		qint64 currentTimeMs = QDateTime::currentMSecsSinceEpoch();
		qint64 expirationTimeMs = currentTimeMs + s_jwtContextCacheTtlMs;
		qint64 jwtExpirationTimeMs = GetJwtExpirationTimeMs(token);
		if (jwtExpirationTimeMs > 0 && jwtExpirationTimeMs < expirationTimeMs){
			expirationTimeMs = jwtExpirationTimeMs;
		}

		if (expirationTimeMs <= currentTimeMs){
			return;
		}

		imtgql::IGqlContextSharedPtr prototypeContextPtr;
		prototypeContextPtr.MoveCastedPtr(gqlContextPtr->CloneMe());
		if (!prototypeContextPtr.IsValid()){
			return;
		}

		prototypeContextPtr->SetToken(QByteArray());
		prototypeContextPtr->SetProductId(QByteArray());
		prototypeContextPtr->SetHeaders(imtgql::IGqlContext::Headers());

		QWriteLocker locker(&m_lock);
		RemoveExpiredItems(currentTimeMs);
		EnsureSpaceForOneMoreItem();
		m_cacheItems.insert(GetTokenCacheKey(token), {prototypeContextPtr, expirationTimeMs, currentTimeMs});
	}

private:
	struct CachedContextItem
	{
		imtgql::IGqlContextSharedPtr prototypeContextPtr;
		qint64 expirationTimeMs = 0;
		qint64 createdAtMs = 0;
	};

	void Remove(const QByteArray& token) const
	{
		QWriteLocker locker(&m_lock);
		m_cacheItems.remove(GetTokenCacheKey(token));
	}

	void RemoveExpiredItems(qint64 currentTimeMs) const
	{
		for (auto itemIter = m_cacheItems.begin(); itemIter != m_cacheItems.end();){
			if (!itemIter.value().prototypeContextPtr.IsValid() || itemIter.value().expirationTimeMs <= currentTimeMs){
				itemIter = m_cacheItems.erase(itemIter);
			}
			else{
				++itemIter;
			}
		}
	}

	void EnsureSpaceForOneMoreItem() const
	{
		while (m_cacheItems.size() >= s_jwtContextCacheLimit){
			auto oldestIter = m_cacheItems.begin();
			for (auto itemIter = std::next(m_cacheItems.begin()); itemIter != m_cacheItems.end(); ++itemIter){
				if (itemIter.value().createdAtMs < oldestIter.value().createdAtMs){
					oldestIter = itemIter;
				}
			}

			m_cacheItems.erase(oldestIter);
		}
	}

private:
	mutable QMap<QByteArray, CachedContextItem> m_cacheItems;
	mutable QReadWriteLock m_lock = QReadWriteLock(QReadWriteLock::Recursive);
};


void ApplyRequestContext(imtgql::CGqlRequest& gqlRequest, imtgql::IGqlContextSharedPtr gqlContextPtr)
{
	imtgql::CGqlRequestContextManager::SetContext(gqlContextPtr.GetPtr());
	gqlRequest.SetGqlContext(gqlContextPtr);
}


} // namespace


// protected methods

// reimplemented (IRequestHandler)

bool CHttpGraphQLServletComp::IsCommandSupported(const QByteArray& commandId) const
{
	if (m_applicationInfoCompPtr.IsValid()){
		QString appId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID);
		if (commandId == appId + QStringLiteral("/graphql")){
			return true;
		}
	}

	return BaseClass::IsCommandSupported(commandId);
}


// reimplemented (imtrest::IRequestServlet)

imtrest::ConstResponsePtr CHttpGraphQLServletComp::OnPost(
	const QByteArray& /*commandId*/,
	const imtrest::IRequest::CommandParams& /*commandParams*/,
	const HeadersMap& headers,
	const imtrest::CHttpRequest& request) const
{
	// Ensure Clear() is called on every return path
	auto cleanup = qScopeGuard([]() {
		imtgql::CGqlRequestContextManager::Clear();
	});

	m_lastRequest.ResetData();

	qsizetype errorPosition = -1;
	const QByteArray requestBody = request.GetBody();
	if (!m_lastRequest.ParseQuery(requestBody, errorPosition)){
		qCritical() << __FILE__ << __LINE__ << QStringLiteral("Error when parsing request: '%1'; Error position: '%2'")
														.arg(QString(request.GetBody()),
															 QString::number(errorPosition));

		return GenerateError(StatusCode::SC_BAD_REQUEST, QStringLiteral("Request is incorrect"), request);
	}

	const QByteArray gqlCommand = m_lastRequest.GetCommandId();

	// Set protocol version - RFC 2616: 4.2 case-insensitive header comparison
	for (HeadersMap::const_iterator headerIter = headers.cbegin(); headerIter != headers.cend(); ++headerIter) {
		// find header. compare with lowercase. RFC 2616: 4.2
		if (headerIter.key().toLower() == imtbase::s_protocolVersionHeaderId.toLower()) {
			const QByteArray& protocolVersion = *headerIter;
			m_lastRequest.SetProtocolVersion(protocolVersion);

			break;
		}
	}

	QByteArray userId;
	QByteArray accessToken = headers.value(QByteArrayLiteral("x-authentication-token"));

	QByteArray productId;
	if (headers.contains(imtbase::s_productIdHeaderId)){
		productId = headers.value(imtbase::s_productIdHeaderId);
	}

	imtgql::IGqlContextSharedPtr cachedContextPtr;
	if (CGqlContextCache::Instance().TryCreateRequestContext(accessToken, productId, headers, cachedContextPtr)){
		ApplyRequestContext(m_lastRequest, cachedContextPtr);
	}
	else{

		// Validate token based on prefix: pat_ for PAT tokens, otherwise JWT
		if (!accessToken.isEmpty()){
			// Check if token starts with "pat_" prefix and has content beyond the prefix
			if (accessToken.size() > 8 && accessToken.startsWith("imt_pat_")){
				// PAT token - validate with PAT manager
				if (m_patManagerCompPtr.IsValid()){
					QByteArray tokenId;
					QByteArrayList scopes;
					if (!m_patManagerCompPtr->ValidateToken(accessToken, userId, tokenId, scopes)){
						return CreateResponse(StatusCode::SC_FORBIDDEN, QByteArray(), request);
					}

					m_patManagerCompPtr->UpdateLastUsedAt(tokenId);
				}
				else{
					return CreateResponse(StatusCode::SC_FORBIDDEN, QByteArray(), request);
				}
			}
			else{
				// JWT token - validate with JWT controller
				if (m_jwtSessionControllerCompPtr.IsValid()){
					using JwtState = imtauth::IJwtSessionController::JwtState;
					JwtState state = m_jwtSessionControllerCompPtr->ValidateJwt(accessToken);
					if (state == JwtState::JS_EXPIRED){
						return CreateResponse(StatusCode::SC_UNAUTHORIZED, QByteArray(), request);
					}
					else if (state == JwtState::JS_INVALID){
						return CreateResponse(StatusCode::SC_FORBIDDEN, QByteArray(), request);
					}

					userId = m_jwtSessionControllerCompPtr->GetUserFromJwt(accessToken);
				}
			}
		}

		if (m_gqlContextCreatorCompPtr.IsValid()){
			QString errorMessage;
			imtgql::IGqlContextUniquePtr gqlContextPtr = m_gqlContextCreatorCompPtr->CreateGqlContext(accessToken, productId, userId, headers, errorMessage);
			if (!gqlContextPtr.IsValid()){
				SendCriticalMessage(
							0,
							QStringLiteral("Unable to create a GraphQL context for the access token '%1' for Command-ID: '%2'. Error: '%3'")
												.arg(QString(accessToken), QString(gqlCommand), errorMessage),
							QStringLiteral("GraphQL - servlet"));
				return GenerateError(StatusCode::SC_INTERNAL_SERVER_ERROR, QStringLiteral("Request context is invalid"), request);
			}

			imtgql::IGqlContextSharedPtr shared;
			shared.MoveCastedPtr(gqlContextPtr);
			CGqlContextCache::Instance().Insert(accessToken, shared.GetPtr());
			ApplyRequestContext(m_lastRequest, shared);
		}
		else{
			// Q_ASSERT(false);
		}
	}

	QByteArray responseData;
	bool isSuccessful = false;

	int dataControllersCount = m_gqlRequestHandlerCompPtr.GetCount();
	for (int index = 0; index < dataControllersCount; index++){
		const imtgql::IGqlRequestHandler* requestHandlerPtr = m_gqlRequestHandlerCompPtr[index];
		if (requestHandlerPtr == nullptr){
			continue;
		}

		if (!requestHandlerPtr->IsRequestSupported(m_lastRequest)) {
			// unsupported request
			continue;
		}

		QString errorMessage;
		QString errorType = QStringLiteral("Warning");
		istd::TDelPtr<imtbase::CTreeItemModel> sourceItemModelPtr(requestHandlerPtr->CreateResponse(m_lastRequest, errorMessage));

		imtbase::CTreeItemModel rootModel;
		bool isError = !sourceItemModelPtr.IsValid();

		// Detect if result model itself contains an "errors" child
		if (!isError) {
			imtbase::CTreeItemModel* errorsModelPtr = sourceItemModelPtr->GetTreeItemModel(QByteArrayLiteral("errors"));
			if (errorsModelPtr != nullptr) {
				isError = true;
				if (errorsModelPtr->ContainsKey(QByteArrayLiteral("message"))) {
					errorMessage = errorsModelPtr->GetData(QByteArrayLiteral("message")).toString();
				}

				if (errorsModelPtr->ContainsKey(QByteArrayLiteral("type"))) {
					errorType = errorsModelPtr->GetData(QByteArrayLiteral("type")).toString();
				}
			}
		}

		if (isError) {
			responseData = BuildGqlErrorJson(gqlCommand, errorMessage, errorType);
			isSuccessful = !responseData.isEmpty();
		}
		else {
			// Success path remains manual for the "data" mapping
			imtbase::CTreeItemModel rootModel;
			auto* dataModelPtr = rootModel.AddTreeModel(QByteArrayLiteral("data"));
			auto* sourceData = sourceItemModelPtr->GetTreeItemModel(QByteArrayLiteral("data"));

			dataModelPtr->SetExternTreeModel(gqlCommand,
											 sourceData ?
												 sourceData->CopyMe() :
												 sourceItemModelPtr->CopyMe());

			iser::CJsonMemWriteArchive archive(nullptr, false);
			if (rootModel.SerializeModel(archive)) {
				responseData = archive.GetData();
				isSuccessful = true;
			}
		}

		break;
	}

	// If we have a GraphQL JSON response (Success OR Warning/Execution Error)
	// We return 200 OK so the client can read the { data: null, errors: [...] } structure.
	if (!responseData.isEmpty()){
		return CreateResponse(
			StatusCode::SC_OK,
			responseData,
			request,
			QByteArrayLiteral("application/json; charset=utf-8"));
	}

	// If no handler supported the request (isSuccessful is still false)
	// or if the loop never ran/serialization failed.
	if (!isSuccessful){
		SendErrorMessage(0, QStringLiteral("Invalid command request:'%1'").arg(QString(gqlCommand)), QStringLiteral("GraphQL - servlet"));

		 // Return 400 because this is a transport/routing failure, not a business logic error.
		return GenerateError(StatusCode::SC_BAD_REQUEST, responseData, request);
	}

	// Fallback for unexpected internal state
	SendErrorMessage(0, QStringLiteral("Internal server error for command '%1'").arg(QString(gqlCommand)), QStringLiteral("GraphQL - servlet"));
	return GenerateError(StatusCode::SC_INTERNAL_SERVER_ERROR, QStringLiteral("Request is incorrect"), request);
}


// reimplemented (imtgql::IGqlRequestProvider)

const imtgql::IGqlRequest* CHttpGraphQLServletComp::GetGqlRequest() const
{
	return &m_lastRequest;
}


// private methods

imtrest::ConstResponsePtr CHttpGraphQLServletComp::CreateResponse(
	const StatusCode& statusCode,
	const QByteArray& payload,
	const imtrest::IRequest& request,
	const QByteArray& contentTypeId) const
{
	return imtrest::ConstResponsePtr(request.GetProtocolEngine().CreateResponse(request, statusCode, payload, contentTypeId).PopInterfacePtr());
}


imtrest::ConstResponsePtr CHttpGraphQLServletComp::GenerateError(
	const StatusCode& errorCode,
	const QString& errorString,
	const imtrest::CHttpRequest& request) const
{
	// Default type to "Error" for 400/500 codes.
	QByteArray responseJson = BuildGqlErrorJson(QByteArray(), errorString, QStringLiteral("Error"));

	return imtrest::ConstResponsePtr(
		request.GetProtocolEngine().CreateResponse(
									   request,
									   errorCode,
									   responseJson,
									   QByteArray("application/json;charset=utf-8")).PopInterfacePtr());
}


QByteArray CHttpGraphQLServletComp::BuildGqlErrorJson(
	const QByteArray& gqlCommand,
	const QString& message,
	const QString& type) const
{
	imtbase::CTreeItemModel rootModel;

	// { "data": null }
	rootModel.SetData(QByteArrayLiteral("data"), QVariant());

	imtbase::CTreeItemModel* errorsArr = rootModel.AddTreeModel(QByteArrayLiteral("errors"));
	if (errorsArr) {
		errorsArr->SetIsArray(true);

		// Add a single error object to the errors array
		int errIdx = errorsArr->InsertNewItem();
		errorsArr->SetData(QByteArrayLiteral("message"), message, errIdx);

		// path: [ "CommandName" ]
		if (!gqlCommand.isEmpty()) {
			imtbase::CTreeItemModel* pathArr = errorsArr->AddTreeModel(QByteArrayLiteral("path"), errIdx);
			pathArr->SetIsArray(true);
			pathArr->SetData(QByteArray(), QString::fromUtf8(gqlCommand), pathArr->InsertNewItem());
		}

		// extensions: { "type": "Warning/Error" }
		imtbase::CTreeItemModel* ext = errorsArr->AddTreeModel(QByteArrayLiteral("extensions"), errIdx);
		if (ext) {
			ext->SetData(QByteArrayLiteral("type"), type, 0);
		}
	}

	iser::CJsonMemWriteArchive archive(nullptr, false);
	if (!rootModel.SerializeModel(archive)) {
		// Serialization failure
		return QByteArray();
	}

	return archive.GetData();
}




} // namespace imtservergql
