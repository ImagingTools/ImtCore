// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CTrafficLogModel.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>

// ImtCore includes
#include <imtrestdemo/CWireLogModel.h>


namespace imtrestdemo
{


namespace
{


QString FormatBodyPreview(const QByteArray& preview, qint64 totalSize)
{
	if (preview.isEmpty()){
		return QString();
	}

	QString retVal = CWireLogModel::EscapeBytes(preview);
	if (totalSize > preview.size()){
		retVal += QCoreApplication::translate("CTrafficLogModel", "\n... %1 more bytes are not shown").arg(totalSize - preview.size());
	}

	return retVal;
}


QString FormatSize(qint64 size)
{
	if (size < 0){
		return QCoreApplication::translate("CTrafficLogModel", "unknown");
	}

	if (size >= 1024 * 1024){
		return QCoreApplication::translate("CTrafficLogModel", "%1 MiB (%2 bytes)").arg(double(size) / (1024 * 1024), 0, 'f', 2).arg(size);
	}

	return QCoreApplication::translate("CTrafficLogModel", "%1 bytes").arg(size);
}


}


CTrafficLogModel::CTrafficLogModel(QObject* parentPtr)
	:QAbstractListModel(parentPtr)
{
}


int CTrafficLogModel::AddExchange(const SDemoExchange& exchange)
{
	SDemoExchange added = exchange;
	added.id = ++m_lastId;

	beginInsertRows(QModelIndex(), int(m_exchanges.size()), int(m_exchanges.size()));
	m_exchanges.append(added);
	endInsertRows();

	Q_EMIT countChanged();

	return added.id;
}


const SDemoExchange* CTrafficLogModel::FindExchange(int exchangeId) const
{
	const int row = FindRow(exchangeId);

	return row >= 0 ? &m_exchanges[row] : nullptr;
}


void CTrafficLogModel::UpdateExchange(const SDemoExchange& exchange)
{
	const int row = FindRow(exchange.id);
	if (row < 0){
		return;
	}

	m_exchanges[row] = exchange;

	const QModelIndex changedIndex = index(row);
	Q_EMIT dataChanged(changedIndex, changedIndex);
}


const SDemoExchange* CTrafficLogModel::GetExchangeAt(int row) const
{
	return (row >= 0 && row < m_exchanges.size()) ? &m_exchanges[row] : nullptr;
}


void CTrafficLogModel::Retranslate()
{
	if (!m_exchanges.isEmpty()){
		Q_EMIT dataChanged(index(0), index(m_exchanges.size() - 1));
	}
}


void CTrafficLogModel::clear()
{
	beginResetModel();
	m_exchanges.clear();
	endResetModel();

	Q_EMIT countChanged();
}


QString CTrafficLogModel::requestText(int row) const
{
	const SDemoExchange* exchangePtr = GetExchangeAt(row);
	if (exchangePtr == nullptr){
		return QString();
	}

	if (exchangePtr->isRaw){
		return QCoreApplication::translate("CTrafficLogModel", "Raw bytes that were sent:\n\n") + FormatBodyPreview(exchangePtr->requestBodyPreview, exchangePtr->requestBodySize);
	}

	const QUrl url(exchangePtr->url);
	QString target = url.path(QUrl::FullyEncoded);
	if (url.hasQuery()){
		target += '?' + url.query(QUrl::FullyEncoded);
	}

	QString retVal = QStringLiteral("%1 %2 HTTP/1.1\n").arg(QString::fromLatin1(exchangePtr->method), target);
	retVal += QStringLiteral("Host: %1\n").arg(url.authority());
	retVal += FormatHeaders(exchangePtr->requestHeaders);

	if (exchangePtr->requestBodySize >= 0){
		retVal += QCoreApplication::translate("CTrafficLogModel", "\n[body: %1]\n").arg(FormatSize(exchangePtr->requestBodySize));
		retVal += FormatBodyPreview(exchangePtr->requestBodyPreview, exchangePtr->requestBodySize);
	}

	return retVal;
}


QString CTrafficLogModel::responseText(int row) const
{
	const SDemoExchange* exchangePtr = GetExchangeAt(row);
	if (exchangePtr == nullptr){
		return QString();
	}

	if (!exchangePtr->isFinished){
		return QCoreApplication::translate("CTrafficLogModel", "(the response is not complete yet: %1 bytes received)").arg(exchangePtr->responseSize);
	}

	if (exchangePtr->statusCode == 0){
		if (exchangePtr->isRaw && !exchangePtr->responsePreview.isEmpty()){
			return QCoreApplication::translate("CTrafficLogModel", "Raw bytes that were received:\n\n") + FormatBodyPreview(exchangePtr->responsePreview, exchangePtr->responseSize);
		}

		return QCoreApplication::translate("CTrafficLogModel", "There is no response: %1").arg(exchangePtr->errorText);
	}

	QString retVal = QStringLiteral("HTTP %1 %2\n").arg(exchangePtr->statusCode).arg(QString::fromLatin1(exchangePtr->reasonPhrase));
	retVal += FormatHeaders(exchangePtr->responseHeaders);
	retVal += QCoreApplication::translate("CTrafficLogModel", "\n[body: %1]\n").arg(FormatSize(exchangePtr->responseSize));
	retVal += FormatBodyPreview(exchangePtr->responsePreview, exchangePtr->responseSize);

	return retVal;
}


QString CTrafficLogModel::summaryText(int row) const
{
	const SDemoExchange* exchangePtr = GetExchangeAt(row);

	return exchangePtr != nullptr ? GetSummary(*exchangePtr) : QString();
}


QString CTrafficLogModel::FormatSpeed(double bytesPerSecond)
{
	if (bytesPerSecond >= 1024.0 * 1024.0){
		return QCoreApplication::translate("CTrafficLogModel", "%1 MiB/s").arg(bytesPerSecond / (1024.0 * 1024.0), 0, 'f', 1);
	}

	if (bytesPerSecond >= 1024.0){
		return QCoreApplication::translate("CTrafficLogModel", "%1 KiB/s").arg(bytesPerSecond / 1024.0, 0, 'f', 1);
	}

	return QCoreApplication::translate("CTrafficLogModel", "%1 B/s").arg(bytesPerSecond, 0, 'f', 0);
}


QString CTrafficLogModel::FormatHeaders(const SDemoExchange::Headers& headers)
{
	QString retVal;
	for (const QPair<QByteArray, QByteArray>& header : headers){
		retVal += QString::fromLatin1(header.first) + QStringLiteral(": ") + QString::fromLatin1(header.second) + '\n';
	}

	return retVal;
}


// reimplemented (QAbstractItemModel)

int CTrafficLogModel::rowCount(const QModelIndex& parent) const
{
	return parent.isValid() ? 0 : int(m_exchanges.size());
}


QVariant CTrafficLogModel::data(const QModelIndex& index, int role) const
{
	const SDemoExchange* exchangePtr = index.isValid() ? GetExchangeAt(index.row()) : nullptr;
	if (exchangePtr == nullptr){
		return QVariant();
	}

	switch (role){
	case R_ID:
		return exchangePtr->id;
	case R_METHOD:
		return QString::fromLatin1(exchangePtr->method);
	case R_URL:
		return exchangePtr->url;
	case R_STATUS_CODE:
		return exchangePtr->statusCode;
	case R_SUMMARY:
		return GetSummary(*exchangePtr);
	case R_IS_FINISHED:
		return exchangePtr->isFinished;
	case R_HAS_ERROR:
		return !exchangePtr->errorText.isEmpty() || exchangePtr->statusCode >= 400;
	case R_DURATION_MS:
		return exchangePtr->durationMs;
	case R_SPEED_TEXT:
		return GetSpeedText(*exchangePtr);
	default:
		return QVariant();
	}
}


QHash<int, QByteArray> CTrafficLogModel::roleNames() const
{
	return {
		{R_ID, "exchangeId"},
		{R_METHOD, "method"},
		{R_URL, "url"},
		{R_STATUS_CODE, "statusCode"},
		{R_SUMMARY, "summary"},
		{R_IS_FINISHED, "isFinished"},
		{R_HAS_ERROR, "hasError"},
		{R_DURATION_MS, "durationMs"},
		{R_SPEED_TEXT, "speedText"}};
}


// private methods

int CTrafficLogModel::FindRow(int exchangeId) const
{
	// The ids grow, the newest exchanges are updated more often
	for (int row = int(m_exchanges.size()) - 1; row >= 0; --row){
		if (m_exchanges[row].id == exchangeId){
			return row;
		}
	}

	return -1;
}


QString CTrafficLogModel::GetSpeedText(const SDemoExchange& exchange)
{
	// The arrows: up is the request that goes to the server, down is the response
	const QString up = QString(QChar(0x2191));
	const QString down = QString(QChar(0x2193));

	QStringList parts;

	if (!exchange.isFinished){
		if (exchange.requestBodySize > 0 && exchange.requestSentSize < exchange.requestBodySize){
			parts << up + ' ' + FormatSpeed(exchange.requestSpeed);
		}

		if (exchange.responseSize > 0 || exchange.responseSpeed > 0){
			parts << down + ' ' + FormatSpeed(exchange.responseSpeed);
		}
	}
	else{
		if (exchange.requestAverageSpeed > 0 && exchange.requestBodySize > 0){
			parts << up + ' ' + FormatSpeed(exchange.requestAverageSpeed);
		}

		if (exchange.responseAverageSpeed > 0 && exchange.responseSize > 0){
			parts << down + ' ' + FormatSpeed(exchange.responseAverageSpeed);
		}
	}

	return parts.join(QStringLiteral("  "));
}


QString CTrafficLogModel::GetSummary(const SDemoExchange& exchange)
{
	QString retVal = QCoreApplication::translate("CTrafficLogModel", "Started: %1\n").arg(exchange.startTime.toString("hh:mm:ss.zzz"));

	if (exchange.isFinished){
		retVal += QCoreApplication::translate("CTrafficLogModel", "Time: %1 ms\n").arg(exchange.durationMs);
	}
	else{
		retVal += QCoreApplication::translate("CTrafficLogModel", "In progress\n");
	}

	retVal += QCoreApplication::translate("CTrafficLogModel", "Request body: %1, sent %2 bytes\n").arg(FormatSize(exchange.requestBodySize)).arg(exchange.requestSentSize);
	if (!exchange.isFinished && exchange.requestBodySize > 0){
		retVal += QCoreApplication::translate("CTrafficLogModel", "Sent: %1%\n").arg(100.0 * double(exchange.requestSentSize) / double(exchange.requestBodySize), 0, 'f', 1);
	}

	retVal += QCoreApplication::translate("CTrafficLogModel", "Response body: %1 received\n").arg(FormatSize(exchange.responseSize));

	if (!exchange.isFinished){
		retVal += QCoreApplication::translate("CTrafficLogModel", "Speed now: sending %1, receiving %2\n").arg(FormatSpeed(exchange.requestSpeed), FormatSpeed(exchange.responseSpeed));
	}
	else if (exchange.requestAverageSpeed > 0 || exchange.responseAverageSpeed > 0){
		retVal += QCoreApplication::translate("CTrafficLogModel", "Average speed: sending %1, receiving %2\n").arg(FormatSpeed(exchange.requestAverageSpeed), FormatSpeed(exchange.responseAverageSpeed));
	}

	if (!exchange.tlsText.isEmpty()){
		retVal += QCoreApplication::translate("CTrafficLogModel", "TLS: %1\n").arg(exchange.tlsText);
	}

	if (!exchange.verificationText.isEmpty()){
		retVal += QCoreApplication::translate("CTrafficLogModel", "Check of the body: %1\n").arg(exchange.verificationText);
	}

	if (!exchange.errorText.isEmpty()){
		retVal += QCoreApplication::translate("CTrafficLogModel", "Error: %1\n").arg(exchange.errorText);
	}

	return retVal;
}


} // namespace imtrestdemo
