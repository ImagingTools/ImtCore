#include <imtrest/CHttpFileReceivedServletCompBase.h>


// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/IProtocolEngine.h>



namespace imtrest
{


// reimplemented (IProtocolEngine)


// reimplemented (icomp::CComponentBase)

void CHttpFileReceivedServletCompBase::OnComponentCreated()
{
    BaseClass::OnComponentCreated();
}

CHttpFileReceivedServletCompBase::ConstResponsePtr CHttpFileReceivedServletCompBase::ProcessRequest(const IRequest& request) const
{


    QByteArray body;
    body = R"(<head> <meta http-equiv="refresh" content="0; URL=)";
    body.append(*m_redirectToCommandID);
    body.append(R"(" /></head>)");

    qDebug() << request.GetBody(); // file_data
    qDebug() << request.GetCommandParams(); // params
    qDebug() << request.GetCommandId(); // URL

    ConstResponsePtr retval = ConstResponsePtr(request.GetProtocolEngine()
                                               .CreateResponse(request,
                                                               IProtocolEngine::SC_MOVED_PERMANENTLY,
                                                               body,
                                                               "text/html; charset=UTF-8")
                                               );
   imtrest::CHttpRequest* httpRequestPtr = dynamic_cast<imtrest::CHttpRequest*>(const_cast<imtrest::IRequest*>(&request));     

//   if (httpRequestPtr != nullptr && httpRequestPtr->GetMethodType() == imtrest::CHttpRequest::MT_POST)
//   {
//       QByteArray requestBody = httpRequestPtr->GetBody();
//       reponseTypeId = "application/json";
//       if (m_representationDataProvider->SetRepresentationData(
//                   imtrest::IRepresentationDataProvider::RF_JSON,
//                   requestBody,modelName) == false){
//           return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_NOT_IMPLEMENTED, "FAIL(", "plain/text; charset=utf-8"));
//       }
//       else{
//           return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, "Sucsess", "plain/text; charset=utf-8"));
//       }

//   }
//   else{
//       reponseTypeId = "application/json";
//       QList<QByteArray> query = modelName.split('/');
//       imtrest::QueryParams params = request.GetCommandParams();
//       if (m_representationDataProvider->GetRepresentationData(
//                   imtrest::IRepresentationDataProvider::RF_JSON,
//                   body, query, params) == false){
//           return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_NOT_IMPLEMENTED, "FAIL(", "plain/text; charset=utf-8"));
//       }
//       else {
//           return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId));
//       }
//   }
//    return retval;
}

QByteArray CHttpFileReceivedServletCompBase::GetSupportedCommandId() const
{
    return *m_commandIdAttrPtr;
}


} // namespace imtrest


