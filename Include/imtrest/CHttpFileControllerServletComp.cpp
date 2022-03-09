#include <imtrest/CHttpFileControllerServletComp.h>


// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/IProtocolEngine.h>



namespace imtrest
{


// protected methods

// reimplemented (CHttpServletCompBase)


CHttpFileControllerServletComp::ConstResponsePtr CHttpFileControllerServletComp::OnPost(
			const QByteArray& commandId,
			const IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
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

	QByteArray body;

	QByteArray commandIdFileName;
	{
		QByteArray commandId = request.GetCommandId();
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

	for (int i = 0; i < m_binaryDataControllersCompPtr.GetCount(); ++i){

		if ((loadRes = (m_binaryDataControllersCompPtr[i]->SetData(request.GetBody(), commandIdFileName)))){
			QByteArray fileSuffix;
			int index = commandIdFileName.lastIndexOf('.');
			if (index > 0){
				index = commandIdFileName.count() - index;
				fileSuffix = commandIdFileName.right(index);
			}
			reponseTypeId = this->GetMimeType(fileSuffix);
			reponseTypeId.append(QByteArray("; charset=utf-8"));
			break;
		}
	}

	ConstResponsePtr responsePtr;
	if (loadRes){
		responsePtr = ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId));
	}
	else{
		responsePtr = generateErrorResponsePtr("Unable to open file", IProtocolEngine::SC_NOT_FOUND);
	}

	return responsePtr;
}


CHttpFileControllerServletComp::ConstResponsePtr CHttpFileControllerServletComp::OnDelete(
			const QByteArray& commandId,
			const IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
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
		QByteArray commandId = request.GetCommandId();
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


