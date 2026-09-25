// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpSender.h>


// Qt includes
#include <QtCore/QDataStream>

// ImtCore includes
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

CHttpSender::CHttpSender(QAbstractSocket* tcpSocketPtr)
{
	m_tcpSocketPtr = tcpSocketPtr;
}


// reimplemented (ITransport)

bool CHttpSender::SendData(QByteArray& data) const
{
	if (m_tcpSocketPtr == nullptr || !m_tcpSocketPtr->isOpen()){
		return false;
	}

	m_tcpSocketPtr->write(data);

	return true;
}


// public methods

bool CHttpSender::BuildResponseData(const IResponse& response, QByteArray& data)
{
	int protocolStatusCode = -1;
	QByteArray statusLiteral;

	if (!response.GetProtocolEngine().GetProtocolStatusCode(response.GetStatusCode(), protocolStatusCode, statusLiteral)){
		return false;
	}

	data.clear();
	data.append(QByteArrayLiteral("HTTP/1.0 "));
	data.append(QByteArray::number(protocolStatusCode));
	data.append(" ");
	data.append(statusLiteral);
	data.append("\r\n");

	IResponse::Headers headers = response.GetHeaders();

	for (IResponse::Headers::ConstIterator headerIter = headers.constBegin(); headerIter != headers.constEnd(); ++headerIter){
		data.append(headerIter.key());
		data.append(": ");
		data.append(headerIter.value());
		data.append("\r\n");
	}

	const QByteArray& contentData = response.GetData();
	quint64 contentLength = contentData.size();

	data.append(QByteArrayLiteral("Content-Length: "));
	data.append(QByteArray::number(contentLength));
	data.append("\r\n");
	data.append(QByteArrayLiteral("Content-Type: "));
	data.append(response.GetDataTypeId());
	data.append("\r\n");

	data.append("\r\n");
	data.append(contentData);

	return true;
}


} // namespace imtrest


