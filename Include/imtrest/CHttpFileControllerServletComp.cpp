#include <imtrest/CHttpFileControllerServletComp.h>


// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// protected methods

// reimplemented (CHttpServletCompBase)

ConstResponsePtr CHttpFileControllerServletComp::OnPost(
			const QByteArray& commandId,
			const IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const CHttpRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();
	QByteArray errorBody = "<html><head><title>Error</title></head><body><p>File resource was not found</p></body></html>";
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	ConstResponsePtr errorResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_RESOURCE_NOT_AVAILABLE, errorBody, reponseTypeId));

	auto generateErrorResponsePtr = [&request, &engine, reponseTypeId](QByteArray errorBody, int _errorCode = IProtocolEngine::SC_INTERNAL_ERROR){
		qCritical() << __FILE__ << __LINE__ << "Error occurred" << errorBody;
		QByteArray generatedErrorBody = "<html><head><title>Error</title></head><body><p>";
		generatedErrorBody.append(errorBody);
		generatedErrorBody.append("</p></body></html>");
		return ConstResponsePtr(engine.CreateResponse(request, _errorCode, generatedErrorBody, reponseTypeId));
	};

	QByteArray commandIdFileName;
	{
		QByteArray commandIdBase = *m_commandIdAttrPtr;

		if (commandIdBase.endsWith('*')){
			commandIdBase.chop(1);
		}
		commandIdFileName = commandId;
		commandIdFileName = commandIdFileName.replace(commandIdBase, "");

		while (commandIdFileName.startsWith('/')){
			commandIdFileName.remove(0,1);
		}
		while (commandIdFileName.endsWith('/')){
			commandIdFileName.chop(1);
		}
	}

	bool loadRes = false;
	bool fileExists = false;
	ConstResponsePtr responsePtr;
	QByteArray httpRequestBody = request.GetBody();
	for (int i = 0; i < m_binaryDataControllersCompPtr.GetCount(); ++i){
		loadRes = m_binaryDataControllersCompPtr[i]->SetData(httpRequestBody, commandIdFileName);
		if (loadRes){
			responsePtr = ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, commandIdFileName, "text/plain;charset=utf-8"));
			break;
		}
		else {
			fileExists = m_binaryDataControllersCompPtr[i]->EnsureDataExists(commandIdFileName);
		}
	}

	if(fileExists){
		QString errorString = QString("<p>File already exists</p><p>%1</p>");
		errorString = errorString.arg(QString(commandIdFileName));
		return generateErrorResponsePtr(errorString.toUtf8(), IProtocolEngine::SC_CONFLICT);
	}
	else if(!loadRes){
		responsePtr = generateErrorResponsePtr("Unable to open file", IProtocolEngine::SC_INTERNAL_SERVER_ERROR);
	}

	return responsePtr;
}


ConstResponsePtr CHttpFileControllerServletComp::OnDelete(
			const QByteArray& /*commandId*/,
			const IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const CHttpRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();
	QByteArray errorBody = "<html><head><title>Error</title></head><body><p>File resource was not found</p></body></html>";
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	ConstResponsePtr errorResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_RESOURCE_NOT_AVAILABLE, errorBody, reponseTypeId));

	auto generateErrorResponsePtr = [&request, &engine, reponseTypeId](QByteArray errorBody, int _errorCode = IProtocolEngine::SC_INTERNAL_ERROR){
		qCritical() << __FILE__ << __LINE__ << "Error occurred" << errorBody;
		QByteArray generatedErrorBody = "<html><head><title>Error</title></head><body><p>";
		generatedErrorBody.append(errorBody);
		generatedErrorBody.append("</p></body></html>");
		return ConstResponsePtr(engine.CreateResponse(request, _errorCode, generatedErrorBody, reponseTypeId));
	};

	QByteArray commandIdFileName;
	{
		QByteArray requestCommandId = request.GetCommandId();
		QByteArray commandIdBase = *m_commandIdAttrPtr;

		if (commandIdBase.endsWith('*')){
			commandIdBase.chop(1);
		}

		commandIdFileName = requestCommandId;
		commandIdFileName = commandIdFileName.replace(commandIdBase, "");

		while (commandIdFileName.startsWith('/')){
			commandIdFileName.remove(0,1);
		}

		while (commandIdFileName.endsWith('/')){
			commandIdFileName.chop(1);
		}
	}

	bool loadRes = false;

	for (int i = 0; i < m_binaryDataControllersCompPtr.GetCount(); ++i){
		loadRes = m_binaryDataControllersCompPtr[i]->RemoveData(commandIdFileName);
		if (loadRes){
			break;
		}
	}

	ConstResponsePtr responsePtr;
	if (loadRes){
		responsePtr = ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, "OK", reponseTypeId));
	}
	else{
		responsePtr = generateErrorResponsePtr("Unable to open file", IProtocolEngine::SC_NOT_FOUND);
	}

	return responsePtr;
}


} // namespace imtrest


