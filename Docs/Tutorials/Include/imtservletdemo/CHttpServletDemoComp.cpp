#include "CHttpServletDemoComp.h"

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


namespace imtservletdemo
{
	const imtrest::IResponse* CHttpServletDemoComp::OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const
	{
		return request.GetProtocolEngine().CreateResponse(
					request,
					imtrest::IProtocolEngine::SC_OK,
					"[GET] HELLO WORLD!",
					QByteArray("text/plain; charset=utf-8"));
	}


	const imtrest::IResponse* CHttpServletDemoComp::OnPost(
				const QByteArray& commandId,
				const QByteArray body,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const
	{
		return request.GetProtocolEngine().CreateResponse(
					request,
					imtrest::IProtocolEngine::SC_OK,
					"[POST] HELLO WORLD!",
					QByteArray("text/plain; charset=utf-8"));
	}


} //namespace imtservletdemo
