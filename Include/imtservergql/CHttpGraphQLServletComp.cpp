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
#include <imtrest/IProtocolEngine.h>


namespace imtservergql
{


// reimplemented (imtrest::IRequestServlet)

imtrest::ConstResponsePtr CHttpGraphQLServletComp::OnPost(
	const QByteArray& /*commandId*/,
	const imtrest::IRequest::CommandParams& /*commandParams*/,
	const HeadersMap& headers,
	const imtrest::CHttpRequest& request) const
{
	m_lastRequest.ResetData();

	int errorPosition = -1;
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
			const QByteArray protocolVersion = *headerIter;
			m_lastRequest.SetProtocolVersion(protocolVersion);

			break;
		}
	}

	QByteArray userId;

	bool isSuccessful = false;

	imtgql::IGqlContext* gqlContextPtr = nullptr;
	QByteArray accessToken = headers.value(QByteArrayLiteral("x-authentication-token"));

	if (!accessToken.isEmpty() && m_jwtSessionControllerCompPtr.IsValid()){
		using JwtState = imtauth::IJwtSessionController::JwtState;
		JwtState state = m_jwtSessionControllerCompPtr->ValidateJwt(accessToken);
		if (state == JwtState::JS_EXPIRED){
			return CreateResponse(StatusCode::SC_UNAUTHORIZED, QByteArray(), request);
		}
		else if (state == JwtState::JS_INVALID){
			return CreateResponse(StatusCode::SC_FORBIDDEN, QByteArray(), request);
		}
	}

	if (!accessToken.isEmpty() && m_gqlContextControllerCompPtr.IsValid()){
		QString errorMessage;

		gqlContextPtr = m_gqlContextControllerCompPtr->GetRequestContext(m_lastRequest, accessToken, headers, errorMessage);
		if (gqlContextPtr != nullptr){
			m_lastRequest.SetGqlContext(gqlContextPtr);

			if (gqlContextPtr->GetUserInfo() != nullptr){
				userId = gqlContextPtr->GetUserInfo()->GetId();
			}
		}
		else{
			SendCriticalMessage(
						0,
						QStringLiteral("Unable to get a GraphQL context for the access token '%1' for Command-ID: '%2'")
											.arg(QString(accessToken), QString(gqlCommand)),
						QStringLiteral("GraphQL - servlet"));

			return GenerateError(StatusCode::SC_INTERNAL_SERVER_ERROR, QStringLiteral("Request context is invalid"), request);
		}
	}
	else if (!headers.isEmpty()){
		imtgql::CGqlContext* gqlContextImplPtr = new imtgql::CGqlContext();
		gqlContextImplPtr->SetHeaders(headers);
		m_lastRequest.SetGqlContext(gqlContextImplPtr);
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

	if (!isSuccessful){
		SendErrorMessage(0, QStringLiteral("Invalid command request:'%1'").arg(QString(gqlCommand)), QStringLiteral("GraphQL - servlet"));

		return CreateResponse(StatusCode::SC_BAD_REQUEST, responseData, request);
	}
	else if (!responseData.isEmpty()){
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


