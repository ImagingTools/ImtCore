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
	QByteArray requestBody = request.GetBody();
	if (!m_lastRequest.ParseQuery(requestBody, errorPosition)){
		qCritical() << __FILE__ << __LINE__ << QString("Error when parsing request: '%1'; Error position: '%2'")
		.arg(qPrintable(request.GetBody()))
			.arg(errorPosition);
	}

	QByteArray gqlCommand = m_lastRequest.GetCommandId();

	//set a protocol version to gql object
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
	QByteArray accessToken = headers.value("x-authentication-token");

	if (!accessToken.isEmpty() && m_jwtSessionControllerCompPtr.IsValid()){
		imtauth::IJwtSessionController::JwtState state = m_jwtSessionControllerCompPtr->ValidateJwt(accessToken);
		if (state == imtauth::IJwtSessionController::JS_EXPIRED){
			return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_UNAUTHORIZED, "", request);
		}
		else if (state == imtauth::IJwtSessionController::JS_INVALID){
			return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_FORBIDDEN, "", request);
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
						QString("Unable to get a GraphQL context for the access token '%1' for Command-ID: '%2'")
									.arg(qPrintable(accessToken), qPrintable(gqlCommand)),
						"GraphQL - servlet");

			return GenerateError(imtrest::IProtocolEngine::StatusCode::SC_INTERNAL_SERVER_ERROR, "Request incorrected", request);
		}
	}
	else{
		if (!headers.isEmpty()){
			imtgql::CGqlContext* gqlContextImplPtr = new imtgql::CGqlContext();
			gqlContextImplPtr->SetHeaders(headers);
			m_lastRequest.SetGqlContext(gqlContextImplPtr);
		}
	}

	QByteArray responseData;

	int dataControllersCount = m_gqlRequestHandlerCompPtr.GetCount();
	for (int index = 0; index < dataControllersCount; index++){
		const imtgql::IGqlRequestHandler* requestHandlerPtr = m_gqlRequestHandlerCompPtr[index];
		if (requestHandlerPtr == nullptr){
			continue;
		}

		if (requestHandlerPtr->IsRequestSupported(m_lastRequest)){
			isSuccessful = true;

			QString errorMessage;
			QString errorType = "Warning";
			istd::TDelPtr<imtbase::CTreeItemModel> sourceItemModelPtr;
			sourceItemModelPtr.SetPtr(requestHandlerPtr->CreateResponse(m_lastRequest, errorMessage));

			bool isError = false;
			imtbase::CTreeItemModel rootModel;
			if(sourceItemModelPtr.IsValid()){
				imtbase::CTreeItemModel* errorsModelPtr = sourceItemModelPtr->GetTreeItemModel("errors");
				if (errorsModelPtr != nullptr){
					if (errorsModelPtr->ContainsKey("message")){
						errorMessage = errorsModelPtr->GetData("message").toString();
					}

					if (errorsModelPtr->ContainsKey("type")){
						errorType = errorsModelPtr->GetData("type").toString();
					}

					isError = true;
				}
				else{
					imtbase::CTreeItemModel* dataModelPtr = rootModel.AddTreeModel("data");

					imtbase::CTreeItemModel* sourceDataModelPtr = sourceItemModelPtr->GetTreeItemModel("data");
					if (sourceDataModelPtr != nullptr){
						dataModelPtr->SetExternTreeModel(gqlCommand, sourceDataModelPtr->CopyMe());
					}
					else{
						dataModelPtr->SetExternTreeModel(gqlCommand, sourceItemModelPtr->CopyMe());
					}
				}
			}
			else{
				isError = true;
			}

			if (isError){
				imtbase::CTreeItemModel* errorsModelPtr = rootModel.AddTreeModel("errors");
				imtbase::CTreeItemModel* errorItemModelPtr = errorsModelPtr->AddTreeModel(gqlCommand);

				errorItemModelPtr->SetData("message", errorMessage);
				errorItemModelPtr->SetData("type", errorType);
			}

			iser::CJsonMemWriteArchive archive(nullptr, false);
			if (rootModel.SerializeModel(archive)){
				responseData = archive.GetData();
			}
			else{
				isSuccessful = false;
			}

			break;
		}
	}

	if (!isSuccessful){
		SendErrorMessage(0, QString("Invalid command request:'%1'").arg(qPrintable(gqlCommand)), "GraphQL - servlet");

		return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_BAD_REQUEST, responseData, request);
	}
	else{
		if (!responseData.isEmpty()){
			return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_OK, responseData, request, "application/json; charset=utf-8");
		}
	}

	SendErrorMessage(0, QString("Internal server error for command '%1'").arg(qPrintable(gqlCommand)), "GraphQL - servlet");

	return GenerateError(imtrest::IProtocolEngine::StatusCode::SC_INTERNAL_SERVER_ERROR, "Request incorrected", request);
}


// reimplemented (imtgql::IGqlRequestProvider)

const imtgql::IGqlRequest* CHttpGraphQLServletComp::GetGqlRequest() const
{
	return &m_lastRequest;
}


// private methods

imtrest::ConstResponsePtr CHttpGraphQLServletComp::CreateResponse(
	const imtrest::IProtocolEngine::StatusCode& statusCode,
	const QByteArray& payload,
	const imtrest::IRequest& request,
	const QByteArray& contentTypeId) const
{
	return imtrest::ConstResponsePtr(request.GetProtocolEngine().CreateResponse(request, statusCode, payload, contentTypeId));
}


imtrest::ConstResponsePtr CHttpGraphQLServletComp::GenerateError(
	const imtrest::IProtocolEngine::StatusCode& errorCode,
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
			QByteArray("application/json;charset=utf-8")));
}


} // namespace imtservergql


