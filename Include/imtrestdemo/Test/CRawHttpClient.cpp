// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CRawHttpClient.h"


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtNetwork/QHostAddress>


CRawHttpClient::CRawHttpClient()
{
}


CRawHttpClient::~CRawHttpClient()
{
	Close();
}


bool CRawHttpClient::Connect(int port, int timeoutMs)
{
	Close();

	m_buffer.clear();
	m_socket.connectToHost(QHostAddress::LocalHost, quint16(port));

	if (!m_socket.waitForConnected(timeoutMs)){
		return false;
	}

	// The parts of a request must reach the server as they are written
	m_socket.setSocketOption(QAbstractSocket::LowDelayOption, 1);

	return true;
}


void CRawHttpClient::Close()
{
	if (m_socket.state() != QAbstractSocket::UnconnectedState){
		m_socket.abort();
	}
}


bool CRawHttpClient::IsOpen() const
{
	return m_socket.state() == QAbstractSocket::ConnectedState;
}


bool CRawHttpClient::Send(const QByteArray& data, int timeoutMs)
{
	if (m_socket.state() != QAbstractSocket::ConnectedState){
		return false;
	}

	qint64 written = 0;
	while (written < data.size()){
		const qint64 count = m_socket.write(data.constData() + written, qMin<qint64>(data.size() - written, 1 << 20));
		if (count < 0){
			return false;
		}

		written += count;

		if (!m_socket.waitForBytesWritten(timeoutMs)){
			// The server can close the connection while the client sends a body that it refused
			return m_socket.bytesToWrite() == 0;
		}
	}

	return true;
}


bool CRawHttpClient::FillBuffer(int timeoutMs)
{
	if (m_socket.bytesAvailable() <= 0){
		if (m_socket.state() != QAbstractSocket::ConnectedState){
			return false;
		}

		if (!m_socket.waitForReadyRead(timeoutMs)){
			return m_socket.bytesAvailable() > 0;
		}
	}

	m_buffer += m_socket.readAll();

	return true;
}


bool CRawHttpClient::ReadHead(QByteArray& head, int timeoutMs)
{
	QElapsedTimer timer;
	timer.start();

	for (;;){
		const qsizetype end = m_buffer.indexOf("\r\n\r\n");
		if (end >= 0){
			head = m_buffer.left(end);
			m_buffer.remove(0, end + 4);

			return true;
		}

		const int left = timeoutMs - int(timer.elapsed());
		if (left <= 0 || !FillBuffer(left)){
			return false;
		}
	}
}


bool CRawHttpClient::ReadLine(QByteArray& line, int timeoutMs)
{
	QElapsedTimer timer;
	timer.start();

	for (;;){
		const qsizetype end = m_buffer.indexOf("\r\n");
		if (end >= 0){
			line = m_buffer.left(end);
			m_buffer.remove(0, end + 2);

			return true;
		}

		const int left = timeoutMs - int(timer.elapsed());
		if (left <= 0 || !FillBuffer(left)){
			return false;
		}
	}
}


bool CRawHttpClient::ReadBodyByLength(qint64 length, QByteArray& body, int timeoutMs)
{
	QElapsedTimer timer;
	timer.start();

	body.reserve(qsizetype(qMin<qint64>(length, 64 * 1024 * 1024)));

	while (body.size() < length){
		if (!m_buffer.isEmpty()){
			const qsizetype count = qsizetype(qMin<qint64>(length - body.size(), m_buffer.size()));
			body += m_buffer.left(count);
			m_buffer.remove(0, count);

			continue;
		}

		const int left = timeoutMs - int(timer.elapsed());
		if (left <= 0 || !FillBuffer(left)){
			return false;
		}
	}

	return true;
}


bool CRawHttpClient::ReadChunkedBody(QByteArray& body, int timeoutMs)
{
	for (;;){
		QByteArray sizeLine;
		if (!ReadLine(sizeLine, timeoutMs)){
			return false;
		}

		// Extensions of the chunk are not used by the server
		bool isNumber = false;
		const qint64 size = sizeLine.split(';').first().trimmed().toLongLong(&isNumber, 16);
		if (!isNumber || size < 0){
			return false;
		}

		if (size == 0){
			// The trailer ends with an empty line
			for (;;){
				QByteArray trailerLine;
				if (!ReadLine(trailerLine, timeoutMs)){
					return false;
				}

				if (trailerLine.isEmpty()){
					return true;
				}
			}
		}

		QByteArray chunk;
		if (!ReadBodyByLength(size, chunk, timeoutMs)){
			return false;
		}

		body += chunk;

		QByteArray endOfChunk;
		if (!ReadLine(endOfChunk, timeoutMs) || !endOfChunk.isEmpty()){
			return false;
		}
	}
}


bool CRawHttpClient::ReadBodyUntilClose(QByteArray& body, int timeoutMs)
{
	QElapsedTimer timer;
	timer.start();

	for (;;){
		body += m_buffer;
		m_buffer.clear();

		if (m_socket.state() != QAbstractSocket::ConnectedState && m_socket.bytesAvailable() <= 0){
			return true;
		}

		const int left = timeoutMs - int(timer.elapsed());
		if (left <= 0){
			return false;
		}

		if (!FillBuffer(left) && m_socket.state() == QAbstractSocket::ConnectedState){
			return false;
		}
	}
}


bool CRawHttpClient::ReadResponse(Response& response, bool isHeadRequest, int timeoutMs)
{
	response = Response();

	QElapsedTimer timer;
	timer.start();

	QByteArray head;
	for (;;){
		const int left = timeoutMs - int(timer.elapsed());
		if (left <= 0 || !ReadHead(head, left)){
			return false;
		}

		const QByteArrayList statusLineParts = head.split('\r').first().split(' ');
		const int status = statusLineParts.value(1).toInt();

		// 100 Continue and the others are not the final response (101 changes the protocol, it is not used here)
		if (status >= 100 && status < 200 && status != 101){
			response.interimStatuses.append(status);

			continue;
		}

		break;
	}

	response.rawHead = head;

	const QByteArrayList lines = head.split('\n');
	const QByteArrayList statusLine = lines.first().trimmed().split(' ');
	response.version = statusLine.value(0);
	response.statusCode = statusLine.value(1).toInt();
	response.reasonPhrase = statusLine.mid(2).join(' ');

	for (int i = 1; i < lines.size(); ++i){
		const QByteArray line = lines[i].trimmed();
		const qsizetype colon = line.indexOf(':');
		if (colon <= 0){
			continue;
		}

		const QByteArray name = line.left(colon).trimmed().toLower();
		const QByteArray value = line.mid(colon + 1).trimmed();

		if (response.headers.contains(name)){
			response.headers[name] += ", " + value;
		}
		else{
			response.headers.insert(name, value);
		}
	}

	response.isChunked = response.GetHeader("transfer-encoding").toLower().contains("chunked");

	const bool isBodyAbsent = isHeadRequest || response.statusCode == 204 || response.statusCode == 304;
	if (isBodyAbsent){
		return true;
	}

	const int left = qMax(1, timeoutMs - int(timer.elapsed()));

	if (response.isChunked){
		return ReadChunkedBody(response.body, left);
	}

	if (response.HasHeader("content-length")){
		bool isNumber = false;
		const qint64 length = response.GetHeader("content-length").toLongLong(&isNumber);
		if (!isNumber || length < 0){
			return false;
		}

		return ReadBodyByLength(length, response.body, left);
	}

	return ReadBodyUntilClose(response.body, left);
}


bool CRawHttpClient::ReadInterimResponse(int& statusCode, int timeoutMs)
{
	QByteArray head;
	if (!ReadHead(head, timeoutMs)){
		return false;
	}

	statusCode = head.split('\r').first().split(' ').value(1).toInt();
	if (statusCode >= 100 && statusCode < 200 && statusCode != 101){
		return true;
	}

	// The final response: it is read later
	m_buffer.prepend(head + "\r\n\r\n");

	return false;
}


bool CRawHttpClient::WaitForClose(int timeoutMs)
{
	QElapsedTimer timer;
	timer.start();

	for (;;){
		if (m_socket.bytesAvailable() > 0){
			m_buffer += m_socket.readAll();
		}

		if (m_socket.state() == QAbstractSocket::UnconnectedState){
			return true;
		}

		const int left = timeoutMs - int(timer.elapsed());
		if (left <= 0){
			return false;
		}

		m_socket.waitForReadyRead(qMin(left, 100));
	}
}


int CRawHttpClient::GetBufferedSize() const
{
	return int(m_buffer.size() + m_socket.bytesAvailable());
}


QByteArray CRawHttpClient::CreateRequest(
			const QByteArray& method,
			const QByteArray& target,
			const Headers& headers,
			const QByteArray& body,
			const QByteArray& version)
{
	QByteArray retVal = method + ' ' + target + ' ' + version + "\r\n";

	bool hasHost = false;
	bool hasLength = false;
	for (const QPair<QByteArray, QByteArray>& header : headers){
		const QByteArray name = header.first.toLower();
		hasHost = hasHost || name == "host";
		hasLength = hasLength || name == "content-length" || name == "transfer-encoding";

		retVal += header.first + ": " + header.second + "\r\n";
	}

	if (!hasHost){
		retVal += "Host: localhost\r\n";
	}

	if (!body.isEmpty() && !hasLength){
		retVal += "Content-Length: " + QByteArray::number(body.size()) + "\r\n";
	}

	retVal += "\r\n";
	retVal += body;

	return retVal;
}


QByteArray CRawHttpClient::CreateChunk(const QByteArray& data)
{
	return QByteArray::number(data.size(), 16) + "\r\n" + data + "\r\n";
}


QByteArray CRawHttpClient::CreateLastChunk()
{
	return "0\r\n\r\n";
}
