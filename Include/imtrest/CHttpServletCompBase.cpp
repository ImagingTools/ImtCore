#include <imtrest/CHttpFileBasedHandlerComp.h>
#include "CHttpServletCompBase.h"


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include<QtCore/QUrlQuery>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponder.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>

#include <imtrest/CHttpResponse.h>


namespace imtrest
{


// public methods

// reimplemented (IProtocolEngine)

const imtrest::IResponse* CHttpServletCompBase::ProcessRequest(const imtrest::IRequest& request) const
{
    const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();
    QByteArray errorBody = "<html><head><title>Error</title></head><body><p>The requested command could not be executed</p></body></html>";
    QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

    istd::TDelPtr<imtrest::IResponse> errorResponsePtr(engine.CreateResponse(request, imtrest::IProtocolEngine::SC_RESOURCE_NOT_AVAILABLE, errorBody, reponseTypeId));

    imtrest::CHttpRequest* httpRequest = dynamic_cast<imtrest::CHttpRequest*>(const_cast<imtrest::IRequest*>(&request));

    if(httpRequest == nullptr)
    {
        qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << "\nwhat(): invalid request!";
        return nullptr;
    }

    using HMT = imtrest::CHttpRequest::MethodType;
    HMT methodType = httpRequest->GetMethodType();

    switch (methodType)
    {
    case HMT::MT_GET: this->OnGetRequestReceived(httpRequest, engine); break;
    case HMT::MT_POST: this->OnPostRequestReceived(httpRequest, engine); break;
    case HMT::MT_DELETE: this->OnDeleteRequestReceived(httpRequest, engine); break;
    case HMT::MT_PATCH: this->OnPatchRequestReceived(httpRequest, engine); break;
    case HMT::MT_PUT: this->OnPutRequestReceived(httpRequest, engine); break;
    case HMT::MT_HEAD: this->OnHeadRequestReceived(httpRequest, engine); break;
    case HMT::MT_OPTIONS: this->OnOptionsRequestReceived(httpRequest, engine); break;
    case HMT::MT_UNKNOWN: this->OnUnknownRequestReceived(httpRequest, engine); break;
    default: this->OnInvalidRequestReceived(httpRequest, engine);  break;
    }

    return nullptr;
}


QByteArray CHttpServletCompBase::GetSupportedCommandId() const
{
    return *m_commandIdAttrPtr;
}

const imtrest::IResponse* CHttpServletCompBase::OnGetRequestReceived(const imtrest::CHttpRequest * const request, const imtrest::IProtocolEngine& engine) const
{
    qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << "\nwhat(): GET request received but NOT procseed!";

    QByteArray commandId = request->GetCommandId();
    imtrest::IRequest::CommandParams commandParams = request->GetCommandParams();
    HeadersMap headers;
    QByteArrayList headersKeys = request->GetHeaders();
    for(const QByteArray& key: ::qAsConst(headersKeys))
    {
        headers.insert(key, request->GetHeaderValue(key));
    }

    return this->OnGet(commandId, commandParams, headers, request);
}

const imtrest::IResponse* CHttpServletCompBase::OnPostRequestReceived(const imtrest::CHttpRequest * const request, const imtrest::IProtocolEngine& engine) const
{
    qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << "\nwhat(): POST request received but NOT procseed!";
    QByteArray commandId = request->GetCommandId();
    QByteArray body = request->GetBody();
    imtrest::IRequest::CommandParams commandParams = request->GetCommandParams();
    HeadersMap headers;
    QByteArrayList headersKeys = request->GetHeaders();
    for(const QByteArray& key: ::qAsConst(headersKeys))
    {
        headers.insert(key, request->GetHeaderValue(key));
    }

    return this->OnPost(commandId, body, commandParams, headers, request);
}

const imtrest::IResponse* CHttpServletCompBase::OnDeleteRequestReceived(const imtrest::CHttpRequest * const request, const imtrest::IProtocolEngine& engine) const
{
    qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << "\nwhat(): DELETE request received but NOT procseed!";
    QByteArray commandId = request->GetCommandId();
    imtrest::IRequest::CommandParams commandParams = request->GetCommandParams();
    HeadersMap headers;
    QByteArrayList headersKeys = request->GetHeaders();
    for(const QByteArray& key: ::qAsConst(headersKeys))
    {
        headers.insert(key, request->GetHeaderValue(key));
    }

    return this->OnDelete(commandId, commandParams, headers, request);
}

const imtrest::IResponse* CHttpServletCompBase::OnPatchRequestReceived(const imtrest::CHttpRequest * const request, const imtrest::IProtocolEngine& engine) const
{
    qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << "\nwhat(): PATCH request received but NOT procseed!";
    QByteArray commandId = request->GetCommandId();
    imtrest::IRequest::CommandParams commandParams = request->GetCommandParams();
    HeadersMap headers;
    QByteArrayList headersKeys = request->GetHeaders();
    for(const QByteArray& key: ::qAsConst(headersKeys))
    {
        headers.insert(key, request->GetHeaderValue(key));
    }

    return this->OnPatch(commandId, commandParams, headers, request);
}

const imtrest::IResponse* CHttpServletCompBase::OnPutRequestReceived(const imtrest::CHttpRequest * const request, const imtrest::IProtocolEngine& engine) const
{
    qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << "\nwhat(): PUT request received but NOT procseed!";
    QByteArray commandId = request->GetCommandId();
    imtrest::IRequest::CommandParams commandParams = request->GetCommandParams();
    HeadersMap headers;
    QByteArrayList headersKeys = request->GetHeaders();
    for(const QByteArray& key: ::qAsConst(headersKeys))
    {
        headers.insert(key, request->GetHeaderValue(key));
    }

    return this->OnPut(commandId, commandParams, headers, request);
}

const imtrest::IResponse* CHttpServletCompBase::OnHeadRequestReceived(const imtrest::CHttpRequest * const request, const imtrest::IProtocolEngine& engine) const
{
    qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << "\nwhat(): HEAD request received but NOT procseed!";
    QByteArray commandId = request->GetCommandId();
    imtrest::IRequest::CommandParams commandParams = request->GetCommandParams();
    HeadersMap headers;
    QByteArrayList headersKeys = request->GetHeaders();
    for(const QByteArray& key: ::qAsConst(headersKeys))
    {
        headers.insert(key, request->GetHeaderValue(key));
    }

    return this->OnHead(commandId, commandParams, headers, request);
}

const imtrest::IResponse* CHttpServletCompBase::OnOptionsRequestReceived(const imtrest::CHttpRequest * const request, const imtrest::IProtocolEngine& engine) const
{
    qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << "\nwhat(): OPTIONS request received but NOT procseed!";
    QByteArray commandId = request->GetCommandId();
    imtrest::IRequest::CommandParams commandParams = request->GetCommandParams();
    HeadersMap headers;
    QByteArrayList headersKeys = request->GetHeaders();
    for(const QByteArray& key: ::qAsConst(headersKeys))
    {
        headers.insert(key, request->GetHeaderValue(key));
    }

    return this->OnOptions(commandId, commandParams, headers, request);
}

const imtrest::IResponse* CHttpServletCompBase::OnUnknownRequestReceived(const imtrest::CHttpRequest * const request, const imtrest::IProtocolEngine& engine) const
{
    qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << "\nwhat(): UNKNOWN request received but NOT procseed!";
    QByteArray commandId = request->GetCommandId();
    imtrest::IRequest::CommandParams commandParams = request->GetCommandParams();
    HeadersMap headers;
    QByteArrayList headersKeys = request->GetHeaders();
    for(const QByteArray& key: ::qAsConst(headersKeys))
    {
        headers.insert(key, request->GetHeaderValue(key));
    }

    return this->OnUnknown(commandId, commandParams, headers, request);
}

const imtrest::IResponse* CHttpServletCompBase::OnInvalidRequestReceived(const imtrest::CHttpRequest * const request, const imtrest::IProtocolEngine& engine) const
{
    qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << "\nwhat(): INVALID request received but NOT procseed!";
    QByteArray commandId = request->GetCommandId();
    imtrest::IRequest::CommandParams commandParams = request->GetCommandParams();
    HeadersMap headers;
    QByteArrayList headersKeys = request->GetHeaders();
    for(const QByteArray& key: ::qAsConst(headersKeys))
    {
        headers.insert(key, request->GetHeaderValue(key));
    }

    return this->OnInvalid(commandId, commandParams, headers, request);
}

const imtrest::IResponse* CHttpServletCompBase::OnGet(const QByteArray &commandId, const imtrest::IRequest::CommandParams &commandParams, const HeadersMap &headers, const imtrest::CHttpRequest * const request) const
{
    Q_UNUSED(commandId) Q_UNUSED(commandParams) Q_UNUSED(headers)
    return this->CreateDefaultErrorResponse("GET request received but NOT procseed!", request);
}

const imtrest::IResponse* CHttpServletCompBase::OnPost(const QByteArray &commandId, const QByteArray body, const imtrest::IRequest::CommandParams &commandParams, const HeadersMap &headers, const imtrest::CHttpRequest * const request) const
{
    Q_UNUSED(commandId) Q_UNUSED(body) Q_UNUSED(commandParams) Q_UNUSED(headers)
    return this->CreateDefaultErrorResponse("POST request received but NOT procseed!", request);
}

const imtrest::IResponse* CHttpServletCompBase::OnDelete(const QByteArray &commandId, const imtrest::IRequest::CommandParams &commandParams, const HeadersMap &headers, const imtrest::CHttpRequest * const request) const
{
    Q_UNUSED(commandId) Q_UNUSED(commandParams) Q_UNUSED(headers)
    return this->CreateDefaultErrorResponse("DELETE request received but NOT procseed!", request);

}

const imtrest::IResponse* CHttpServletCompBase::OnPatch(const QByteArray &commandId, const imtrest::IRequest::CommandParams &commandParams, const HeadersMap &headers, const imtrest::CHttpRequest * const request) const
{
    Q_UNUSED(commandId) Q_UNUSED(commandParams) Q_UNUSED(headers)
    return this->CreateDefaultErrorResponse("PATCH request received but NOT procseed!", request);

}

const imtrest::IResponse* CHttpServletCompBase::OnPut(const QByteArray &commandId, const imtrest::IRequest::CommandParams &commandParams, const HeadersMap &headers, const imtrest::CHttpRequest * const request) const
{
    Q_UNUSED(commandId) Q_UNUSED(commandParams) Q_UNUSED(headers)
    return this->CreateDefaultErrorResponse("PUT request received but NOT procseed!", request);

}

const imtrest::IResponse* CHttpServletCompBase::OnHead(const QByteArray &commandId, const imtrest::IRequest::CommandParams &commandParams, const HeadersMap &headers, const imtrest::CHttpRequest * const request) const
{
    Q_UNUSED(commandId) Q_UNUSED(commandParams) Q_UNUSED(headers)
    return this->CreateDefaultErrorResponse("HEAD request received but NOT procseed!", request);

}

const imtrest::IResponse* CHttpServletCompBase::OnOptions(const QByteArray &commandId, const imtrest::IRequest::CommandParams &commandParams, const HeadersMap &headers, const imtrest::CHttpRequest * const request) const
{
    Q_UNUSED(commandId) Q_UNUSED(commandParams) Q_UNUSED(headers)
    return this->CreateDefaultErrorResponse("OPTIONS request received but NOT procseed!", request);

}

const imtrest::IResponse* CHttpServletCompBase::OnUnknown(const QByteArray &commandId, const imtrest::IRequest::CommandParams &commandParams, const HeadersMap &headers, const imtrest::CHttpRequest * const request) const
{
    Q_UNUSED(commandId) Q_UNUSED(commandParams) Q_UNUSED(headers)
    return this->CreateDefaultErrorResponse("UNKNOWN request received but NOT procseed!", request);

}

const imtrest::IResponse* CHttpServletCompBase::OnInvalid(const QByteArray &commandId, const imtrest::IRequest::CommandParams &commandParams, const HeadersMap &headers, const imtrest::CHttpRequest * const request) const
{
    Q_UNUSED(commandId) Q_UNUSED(commandParams) Q_UNUSED(headers)
    return this->CreateDefaultErrorResponse("INVALID request received but NOT procseed!", request);

}

imtrest::IResponse* CHttpServletCompBase::CreateDefaultErrorResponse(const QByteArray &errorString, const imtrest::CHttpRequest * const request) const
{
    return request->GetProtocolEngine().CreateResponse(
                        *request,
                        imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE,
                        errorString,
                        QByteArray("text/plain; charset=utf-8"));
}

} // namespace imtrest


