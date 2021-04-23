#include "CJsonModelBasedHandlerComp.h"

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtrest/IRequestHandler.h>


namespace imtrest
{


CJsonModelBasedHandlerComp::CJsonModelBasedHandlerComp()
{

}

// reimplemented (IRequestHandler)

IRequestHandler::ConstResponsePtr CJsonModelBasedHandlerComp::ProcessRequest(const IRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();
	QByteArray errorBody = "<html><head><title>Error</title></head><body><p>File resource was not found</p></body></html>";
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	ConstResponsePtr errorResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_RESOURCE_NOT_AVAILABLE, errorBody, reponseTypeId));

	auto generateErrorResponsePtr = [&request, &engine, reponseTypeId](QByteArray errorBody){
		qCritical() << __FILE__ << __LINE__ << "Error occurred" << errorBody;
		QByteArray generatedErrorBody = "<html><head><title>Error</title></head><body><p>";
		generatedErrorBody.append(errorBody);
		generatedErrorBody.append("</p></body></html>");
		return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_INTERNAL_ERROR, generatedErrorBody, reponseTypeId));
	};



	QByteArray body;
	QByteArray commandId = request.GetCommandId();
	QByteArray commandIdBase = *this->m_commandIdAttrPtr;
	if(commandIdBase.endsWith('*'))
	{
		commandIdBase.chop(1);
	}
	QString modelName = commandId.replace(commandIdBase, "");
	if(modelName.endsWith('/'))
	{
		modelName.chop(1);
	}
	while(modelName.startsWith('/'))
	{
		modelName.remove(0,1);
	}

	if(modelName == "__HELLO_MODEL__")
	{
		reponseTypeId = "application/json";
		body = QByteArray(R"({"I'm seyin to you": "HELLO WORLD"})");
	}
	else if(modelName == "__EXAMPLE_1__")
	{
		reponseTypeId = "application/json";
		body = QByteArray(R"({"adresses":[{"city":"Moscou","country":"Russia","postalCode":644099,"street":"Lenina 10"},{"city":"Munic","country":"Germany","postalCode":123456,"street":"Street St"}],"firstName":"Ivan","lastName":"Ivanov","nicName":"NicIvan"})");
	}
	else if(modelName == "__EXAMPLE_2__")
	{
		reponseTypeId = "application/json";
		body = QByteArray(R"({"firstName":"Ivan","lastName":"Ivanov","nicName":"NicIvan","adresses":[{"country":"Russia","city":"Moscow","postalCode":644099,"street":"Lenina 10"},{"country":"Germany","city":"Munic","postalCode":123456,"street":"Street St"}]})");
	}
	else if(m_jsonModelProcessor.IsValid())
	{
//		m_jsonModelProcessor->GetModel(modelName);
		reponseTypeId = "application/json";
	}
	else
	{
		generateErrorResponsePtr(QByteArray("Cannot init ModelProcessor OffLine OR Not created OR not setted"));
	}

	ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId));

	return responsePtr;
}

QByteArray CJsonModelBasedHandlerComp::GetSupportedCommandId() const
{
	return *m_commandIdAttrPtr;
}



} // namespace imtrest
