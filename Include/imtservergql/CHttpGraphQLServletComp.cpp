// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CHttpGraphQLServletComp.h>


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

	// set a protocol version to gql object
	for (HeadersMap::const_iterator headerIter = headers.cbegin(); headerIter != headers.cend(); ++headerIter){
		// find header. compare with lowercase. RFC 2616: 4.2
		if (headerIter.key().toLower() == imtbase::s_protocolVersionHeaderId.toLower()){
			const QByteArray& protocolVersion = *headerIter;
			m_lastRequest.SetProtocolVersion(protocolVersion);

			break;
		}
	}

	bool isSuccessful = false;

	QByteArray userId;
	QByteArray accessToken = headers.value(QByteArrayLiteral("x-authentication-token"));

	// Validate token based on prefix: pat_ for PAT tokens, otherwise JWT
	if (!accessToken.isEmpty()){
		// Check if token starts with "pat_" prefix and has content beyond the prefix
		if (accessToken.size() > 4 && accessToken.startsWith("pat_")){
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

	QByteArray productId;
	if (headers.contains(imtbase::s_productIdHeaderId)){
		productId = headers.value(imtbase::s_productIdHeaderId);
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
		m_lastRequest.SetGqlContext(shared);
	}
	else if (!headers.isEmpty()){
		Q_ASSERT(false);
	}

	QByteArray responseData;

	int dataControllersCount = m_gqlRequestHandlerCompPtr.GetCount();
	for (int index = 0; index < dataControllersCount; index++){
		const imtgql::IGqlRequestHandler* requestHandlerPtr = m_gqlRequestHandlerCompPtr[index];
		if (requestHandlerPtr == nullptr){
			continue;
		}

		isSuccessful = requestHandlerPtr->IsRequestSupported(m_lastRequest);
		// unsupported request
		if (!isSuccessful){
			continue;
		}

		QString errorMessage;
		QString errorType = QStringLiteral("Warning");
		istd::TDelPtr<imtbase::CTreeItemModel> sourceItemModelPtr;
		sourceItemModelPtr.SetPtr(requestHandlerPtr->CreateResponse(m_lastRequest, errorMessage));

		bool isError = false;
		imtbase::CTreeItemModel rootModel;

		isError = !sourceItemModelPtr.IsValid();
		if(!isError){
			imtbase::CTreeItemModel* errorsModelPtr = sourceItemModelPtr->GetTreeItemModel(QByteArrayLiteral("errors"));
			isError = errorsModelPtr != nullptr;

			if (isError){
				if (errorsModelPtr->ContainsKey(QByteArrayLiteral("message"))){
					errorMessage = errorsModelPtr->GetData(QByteArrayLiteral("message")).toString();
				}

				if (errorsModelPtr->ContainsKey(QByteArrayLiteral("type"))){
					errorType = errorsModelPtr->GetData(QByteArrayLiteral("type")).toString();
				}
			}
			else{
				imtbase::CTreeItemModel* dataModelPtr = rootModel.AddTreeModel(QByteArrayLiteral("data"));

				imtbase::CTreeItemModel* sourceDataModelPtr = sourceItemModelPtr->GetTreeItemModel(QByteArrayLiteral("data"));
				if (sourceDataModelPtr != nullptr){
					dataModelPtr->SetExternTreeModel(gqlCommand, sourceDataModelPtr->CopyMe());
				}
				else{
					dataModelPtr->SetExternTreeModel(gqlCommand, sourceItemModelPtr->CopyMe());
				}
			}
		}

		if (isError){
			imtbase::CTreeItemModel* errorsModelPtr = rootModel.AddTreeModel(QByteArrayLiteral("errors"));
			imtbase::CTreeItemModel* errorItemModelPtr = errorsModelPtr->AddTreeModel(gqlCommand);

			errorItemModelPtr->SetData(QByteArrayLiteral("message"), errorMessage);
			errorItemModelPtr->SetData(QByteArrayLiteral("type"), errorType);
		}

		iser::CJsonMemWriteArchive archive(nullptr, false);

		isSuccessful = rootModel.SerializeModel(archive);
		if (isSuccessful){
			responseData = archive.GetData();
		}

		break;
	}

	imtgql::CGqlRequestContextManager::Clear();

	if (!isSuccessful){
		SendErrorMessage(0, QStringLiteral("Invalid command request:'%1'").arg(QString(gqlCommand)), QStringLiteral("GraphQL - servlet"));

		return CreateResponse(StatusCode::SC_BAD_REQUEST, responseData, request);
	}
	if (!responseData.isEmpty()){
		return CreateResponse(
			StatusCode::SC_OK,
			responseData,
			request,
			QByteArrayLiteral("application/json; charset=utf-8"));
	}

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
	const QString& /*errorString*/,
	const imtrest::CHttpRequest& request) const
{
	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	int protocolErrorCode = 200;
	QByteArray protocolErrorString;
	engine.GetProtocolStatusCode(errorCode, protocolErrorCode, protocolErrorString);

	QByteArray responseJson;
	return imtrest::ConstResponsePtr(
		engine.CreateResponse(
			request,
			errorCode,
			responseJson,
			QByteArray("application/json;charset=utf-8")).PopInterfacePtr());
}


} // namespace imtservergql


