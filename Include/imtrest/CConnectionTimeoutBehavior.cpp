// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CConnectionTimeoutBehavior.h>


// ImtCore includes
#include <imtrest/CHttpDateTools.h>
#include <imtrest/CHttpStreamingResponseSender.h>
#include <imtrest/ProtocolTypeIds.h>


namespace
{


// An error response of HTTP without content, after which the connection is closed
QByteArray CreateHttpClosingErrorResponse(const QByteArray& statusLine, const QDateTime& dateTime)
{
	QByteArray retVal("HTTP/1.1 " + statusLine + "\r\n");
	retVal += "Date: " + imtrest::CHttpDateTools::ToImfFixdate(dateTime) + "\r\n";
	retVal += "Connection: close\r\n";
	retVal += "Content-Length: 0\r\n";
	retVal += "\r\n";

	return retVal;
}


} // namespace


namespace imtrest
{


// public methods

bool CConnectionTimeoutBehavior::IsRequestResponseCycle() const
{
	return !isLimitedBeforeFirstMessageOnly;
}


// public static methods

CConnectionTimeoutBehavior CConnectionTimeoutBehavior::CreateForProtocol(const QByteArray& protocolTypeId)
{
	CConnectionTimeoutBehavior retVal;

	if (protocolTypeId.toLower() == ProtocolTypeIds::s_http){
		retVal.createRequestTimeoutMessage = [](){
			return CreateHttpRequestTimeoutResponse(QDateTime::currentDateTimeUtc());
		};
		retVal.createBadRequestMessage = [](){
			return CreateHttpBadRequestResponse(QDateTime::currentDateTimeUtc());
		};
		retVal.createProcessingTimeoutMessage = [](){
			return CreateHttpProcessingTimeoutResponse(QDateTime::currentDateTimeUtc());
		};
		retVal.createServerErrorMessage = [](){
			return CreateHttpInternalServerErrorResponse(QDateTime::currentDateTimeUtc());
		};
		retVal.createStreamingResponseSender = [](QAbstractSocket& socket, CSocketTimeoutWatcher& watcher, qint64 writeHighWatermark){
			return IStreamingResponseSenderPtr(new CHttpStreamingResponseSender(socket, watcher, writeHighWatermark));
		};
		retVal.isLimitedBeforeFirstMessageOnly = false;
	}

	return retVal;
}


QByteArray CConnectionTimeoutBehavior::CreateHttpRequestTimeoutResponse(const QDateTime& dateTime)
{
	return CreateHttpClosingErrorResponse("408 Request Timeout", dateTime);
}


QByteArray CConnectionTimeoutBehavior::CreateHttpBadRequestResponse(const QDateTime& dateTime)
{
	return CreateHttpClosingErrorResponse("400 Bad Request", dateTime);
}


QByteArray CConnectionTimeoutBehavior::CreateHttpProcessingTimeoutResponse(const QDateTime& dateTime)
{
	return CreateHttpClosingErrorResponse("503 Service Unavailable", dateTime);
}


QByteArray CConnectionTimeoutBehavior::CreateHttpInternalServerErrorResponse(const QDateTime& dateTime, bool isConnectionClosing)
{
	QByteArray retVal("HTTP/1.1 500 Internal Server Error\r\n");
	retVal += "Date: " + CHttpDateTools::ToImfFixdate(dateTime) + "\r\n";

	if (isConnectionClosing){
		retVal += "Connection: close\r\n";
	}

	retVal += "Content-Length: 0\r\n";
	retVal += "\r\n";

	return retVal;
}


} // namespace imtrest


