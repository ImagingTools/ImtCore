// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QAbstractListModel>
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QPair>


namespace imtrestdemo
{


/**
	Exchange of the demo client with the server as the application sees it: what it asked and what it got. The exact bytes of the
	exchange are in CWireLogModel, here are the parts that are read from the point of view of HTTP: the method, the URL, the headers,
	the bodies (the beginning of them), the status, the time and the result of the check of the body.
*/
struct SDemoExchange
{
	typedef QList<QPair<QByteArray, QByteArray>> Headers;

	int id = 0;
	QDateTime startTime;

	QByteArray method;
	QString url;
	Headers requestHeaders;
	qint64 requestBodySize = -1;
	QByteArray requestBodyPreview;
	qint64 requestSentSize = 0;

	/**
		The exchange of the raw socket: the request is the text that is sent as it is.
	*/
	bool isRaw = false;

	bool isFinished = false;
	int statusCode = 0;
	QByteArray reasonPhrase;
	Headers responseHeaders;
	qint64 responseSize = 0;
	QByteArray responsePreview;
	qint64 durationMs = 0;

	// The protocol, the cipher and the certificates of the server of a TLS connection, empty for http
	QString tlsText;
	QString errorText;

	/**
		Speed of the transfer in bytes per second: the current one while the exchange works (it is measured every half of a second),
		the average one of the whole exchange after it is finished.
	*/
	double requestSpeed = 0;
	double responseSpeed = 0;
	double requestAverageSpeed = 0;
	double responseAverageSpeed = 0;

	/**
		Result of the check of the body (the size, SHA-256, the pattern), empty if the body is not checked.
	*/
	QString verificationText;
};


/**
	Model of the exchanges of the demo client (the debug preview, the tab of the exchanges).
*/
class CTrafficLogModel: public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
	enum Role
	{
		R_ID = Qt::UserRole + 1,
		R_METHOD,
		R_URL,
		R_STATUS_CODE,
		R_SUMMARY,
		R_IS_FINISHED,
		R_HAS_ERROR,
		R_DURATION_MS,
		R_SPEED_TEXT
	};

	static constexpr int s_previewLimit = 4096;

	explicit CTrafficLogModel(QObject* parentPtr = nullptr);

	/**
		Add an exchange, its id is set.
		\return The id of the exchange.
	*/
	int AddExchange(const SDemoExchange& exchange);

	/**
		Get the exchange by its id.
		\return \c nullptr if there is no such exchange; the pointer is valid until the next change of the model.
	*/
	const SDemoExchange* FindExchange(int exchangeId) const;

	/**
		Replace the exchange (by its id) and tell the views.
	*/
	void UpdateExchange(const SDemoExchange& exchange);

	/**
		Get the exchange of a row.
	*/
	const SDemoExchange* GetExchangeAt(int row) const;

	Q_INVOKABLE void clear();

	/**
		The texts are made when they are asked, so after the change of the language the views must ask for them again.
	*/
	void Retranslate();

	/**
		Get the request as the text of the HTTP message with the body (the beginning of it).
		The library of the client adds the headers (\c Host, \c Connection, ...) and the framing: see the exact bytes in the wire log.
	*/
	Q_INVOKABLE QString requestText(int row) const;
	Q_INVOKABLE QString responseText(int row) const;

	/**
		Get the description of the exchange: the time, the sizes, the result of the check and the error.
	*/
	Q_INVOKABLE QString summaryText(int row) const;

	static QString FormatHeaders(const SDemoExchange::Headers& headers);

	/**
		Write a speed in bytes per second with the unit: 12.3 MiB/s.
	*/
	static QString FormatSpeed(double bytesPerSecond);

	// reimplemented (QAbstractItemModel)
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role) const override;
	virtual QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
	void countChanged();

private:
	int FindRow(int exchangeId) const;
	static QString GetSummary(const SDemoExchange& exchange);
	static QString GetSpeedText(const SDemoExchange& exchange);

private:
	QList<SDemoExchange> m_exchanges;
	int m_lastId = 0;
};


} // namespace imtrestdemo


