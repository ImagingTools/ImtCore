// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// imtrest includes
#include <imtrest/CHttpServletCompBase.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


class CSdlJsonServletBaseComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;
	typedef QSharedPointer<imtrest::IResponse> ResponsePtr;

	I_BEGIN_BASE_COMPONENT(CSdlJsonServletBaseComp)
	I_END_COMPONENT

protected:
	template <class SdlClass>
	bool ReadRequestToSdlStruct(
				SdlClass& out,
				const imtrest::CHttpRequest& request,
				ResponsePtr& response) const;

	template <class SdlClass>
	bool CreateResponseFromSdlStruct(
			const SdlClass& input,
			const imtrest::CHttpRequest& request,
			IProtocolEngine::StatusCode protocolStatusCode,
			ResponsePtr& response,
			QJsonDocument::JsonFormat jsonFormat = QJsonDocument::Compact) const;


	virtual ResponsePtr CreateDefaultErrorResponse(
			int errorCode,
			const QString& errorString,
			const QString& additionalInfo,
			IProtocolEngine::StatusCode protocolStatusCode,
			const imtrest::CHttpRequest& request) const;
};


// template protected methods

template<class SdlClass>
bool CSdlJsonServletBaseComp::ReadRequestToSdlStruct(
			SdlClass& out,
			const CHttpRequest& request,
			ResponsePtr& responsePtr) const
{
	const QByteArray requestBody = request.GetBody();
	const QJsonDocument payloadDocument = QJsonDocument::fromJson(requestBody);
	if (!payloadDocument.isObject()){
		const QString baseErrorString = QT_TR_NOOP("The request is not a valid json object");
		SendErrorMessage(0, baseErrorString + QString(": '%1'").arg(QString(requestBody)), __func__);

		responsePtr = CreateDefaultErrorResponse(
			400,
			"Incorrect request",
			baseErrorString,
			imtrest::IProtocolEngine::SC_BAD_REQUEST,
			request);

		return false;
	}

	const QJsonObject payloadObject = payloadDocument.object();
	const bool isCreated = SdlClass::ReadFromJsonObject(out, payloadObject);
	if (!isCreated){
		const QString baseErrorString = QT_TR_NOOP("The request does not match the scheme");
		SendErrorMessage(0, baseErrorString + QString(": '%1'").arg(QString(requestBody)), __func__);
		responsePtr = CreateDefaultErrorResponse(
			400,
			"Incorrect request",
			baseErrorString,
			imtrest::IProtocolEngine::SC_BAD_REQUEST,
			request);

		return false;
	}

	return true;
}


template <class SdlClass>
bool CSdlJsonServletBaseComp::CreateResponseFromSdlStruct(
	const SdlClass& input,
	const imtrest::CHttpRequest& request,
	IProtocolEngine::StatusCode protocolStatusCode,
	ResponsePtr& responsePtr,
	QJsonDocument::JsonFormat jsonFormat) const
{
	QJsonObject replyJson;
	const bool isAdded = input.WriteToJsonObject(replyJson);
	if (!isAdded){
		QString baseErrorString = QT_TR_NOOP("Unable to convert reply to json");
		SendCriticalMessage(0, baseErrorString, __func__);
		Q_ASSERT_X(false, __func__, baseErrorString.toLocal8Bit());

		responsePtr = CreateDefaultErrorResponse(
			500,
			"Internal error",
			baseErrorString,
			imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR,
			request);

		return false;
	}

	QByteArray replyPayload = QJsonDocument(replyJson).toJson(jsonFormat);
	responsePtr.reset(request.GetProtocolEngine().CreateResponse(
		request,
		protocolStatusCode,
		replyPayload,
		QByteArray("application/json; charset=utf-8")).PopInterfacePtr());

	return true;
}


} // namespace imtrest


