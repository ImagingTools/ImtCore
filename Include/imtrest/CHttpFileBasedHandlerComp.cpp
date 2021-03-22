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

bool CHttpFileBasedHandlerComp::ProcessRequest(const IRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();
    QByteArray errorBody = "<html><head><title>Error</title></head><body><p>The requested command could not be executed</p></body></html>";
    QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

    istd::TDelPtr<IResponse> errorResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_RESOURCE_NOT_AVAILABLE, errorBody, reponseTypeId));

    if (!m_fileTemplatePathCompPtr.IsValid())
    {
        engine.GetResponder().SendResponse(*errorResponsePtr);
        return false;
    }
    
    QString templateFilePath = m_fileTemplatePathCompPtr->GetPath();
    
    if (templateFilePath.isEmpty())
    {
        engine.GetResponder().SendResponse(*errorResponsePtr);
        return false;
    }
    
    QFile templateFile(templateFilePath);

    if (!templateFile.open(QIODevice::ReadOnly))
    {
        engine.GetResponder().SendResponse(*errorResponsePtr);
        return false;
    }

    QByteArray body = templateFile.readAll();


    istd::TDelPtr<IResponse> responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId));
    if (!responsePtr.IsValid())
    {
        return false;
    }
     engine.GetResponder().SendResponse(*responsePtr);
     return true;
}


QByteArray CHttpFileBasedHandlerComp::GetSupportedCommandId() const
{
    return *m_commandIdAttrPtr;
}

} // namespace imtrest


