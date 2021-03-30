#include <imtrest/CHttpFileBasedHandlerComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponder.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

// reimplemented (IProtocolEngine)

const imtrest::IResponse* CHttpFileBasedHandlerComp::ProcessRequest(const IRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();
	QByteArray errorBody = "<html><head><title>Error</title></head><body><p>File resource was not found</p></body></html>";
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	istd::TDelPtr<IResponse> errorResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_RESOURCE_NOT_AVAILABLE, errorBody, reponseTypeId));

    if (!m_fileTemplatePathCompPtr.IsValid()){

        return errorResponsePtr.PopPtr();
	}

	QString templateFilePath = m_fileTemplatePathCompPtr->GetPath();

    if (templateFilePath.isEmpty()){

        return errorResponsePtr.PopPtr();
	}

	QFile templateFile(templateFilePath);

    if (!templateFile.open(QIODevice::ReadOnly)){

        return errorResponsePtr.PopPtr();
	}

	QByteArray body = templateFile.readAll();

	istd::TDelPtr<IResponse> responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId));
	if (!responsePtr.IsValid()){
        return nullptr;
	}

    return responsePtr.PopPtr();
}


QByteArray CHttpFileBasedHandlerComp::GetSupportedCommandId() const
{
	return *m_commandIdAttrPtr;
}


} // namespace imtrest


