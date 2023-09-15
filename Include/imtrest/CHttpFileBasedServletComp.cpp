#include <imtrest/CHttpFileBasedServletComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

// reimplemented (IRequestHandler)

ConstResponsePtr CHttpFileBasedServletComp::ProcessRequest(const IRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();
	QByteArray errorBody = "<html><head><title>Error</title></head><body><p>File resource was not found</p></body></html>";
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	ConstResponsePtr errorResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_RESOURCE_NOT_AVAILABLE, errorBody, reponseTypeId));

	if (!m_fileTemplatePathCompPtr.IsValid()){
		return errorResponsePtr;
	}

	QString templateFilePath = m_fileTemplatePathCompPtr->GetPath();
	if (templateFilePath.isEmpty()){
		return errorResponsePtr;
	}

	QFile templateFile(templateFilePath);
	if (!templateFile.open(QIODevice::ReadOnly)){
		return errorResponsePtr;
	}

	QByteArray body = templateFile.readAll();

	ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId));

	return responsePtr;
}


QByteArray CHttpFileBasedServletComp::GetSupportedCommandId() const
{
	return *m_commandIdAttrPtr;
}


} // namespace imtrest


