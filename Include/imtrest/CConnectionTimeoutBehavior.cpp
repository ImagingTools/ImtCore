// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CConnectionTimeoutBehavior.h>


// ImtCore includes
#include <imtrest/CHttpDateTools.h>
#include <imtrest/ProtocolTypeIds.h>


namespace imtrest
{


// public static methods

CConnectionTimeoutBehavior CConnectionTimeoutBehavior::CreateForProtocol(const QByteArray& protocolTypeId)
{
	CConnectionTimeoutBehavior retVal;

	if (protocolTypeId.toLower() == ProtocolTypeIds::s_http){
		retVal.createRequestTimeoutMessage = [](){
			return CreateHttpRequestTimeoutResponse(QDateTime::currentDateTimeUtc());
		};
		retVal.isLimitedBeforeFirstMessageOnly = false;
	}

	return retVal;
}


QByteArray CConnectionTimeoutBehavior::CreateHttpRequestTimeoutResponse(const QDateTime& dateTime)
{
	QByteArray retVal("HTTP/1.1 408 Request Timeout\r\n");
	retVal += "Date: " + CHttpDateTools::ToImfFixdate(dateTime) + "\r\n";
	retVal += "Connection: close\r\n";
	retVal += "Content-Length: 0\r\n";
	retVal += "\r\n";

	return retVal;
}


} // namespace imtrest


