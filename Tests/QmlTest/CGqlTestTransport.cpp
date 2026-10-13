// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CGqlTestTransport.h"


// Qt includes
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtgql/CGqlEnum.h>
#include <imtgql/CGqlFieldObject.h>
#include <imtgql/CGqlParamObject.h>
#include <imtgql/CGqlRequest.h>


namespace imtqmltest
{


namespace
{


const QByteArray s_defaultResponseBody = QByteArrayLiteral("{\"data\":{}}");
const int s_defaultResponseStatus = 200;


class CGqlTestReply: public QNetworkReply
{
public:
	CGqlTestReply(QNetworkAccessManager::Operation operation, const QNetworkRequest& request, int status, const QByteArray& data, QObject* parentPtr)
		:QNetworkReply(parentPtr),
		m_data(data),
		m_offset(0)
	{
		setRequest(request);
		setUrl(request.url());
		setOperation(operation);
		setAttribute(QNetworkRequest::HttpStatusCodeAttribute, status);
		setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/json"));
		setHeader(QNetworkRequest::ContentLengthHeader, m_data.size());
		open(QIODevice::ReadOnly);

		// a real reply never finishes before the caller has connected to it
		QTimer::singleShot(0, this, [this](){
			setFinished(true);
			Q_EMIT metaDataChanged();
			Q_EMIT readyRead();
			Q_EMIT finished();
		});
	}

	// reimplemented (QNetworkReply)
	virtual void abort() override
	{
	}

	// reimplemented (QIODevice)
	virtual qint64 bytesAvailable() const override
	{
		return m_data.size() - m_offset + QIODevice::bytesAvailable();
	}

	virtual bool isSequential() const override
	{
		return true;
	}

protected:
	// reimplemented (QIODevice)
	virtual qint64 readData(char* dataPtr, qint64 maxSize) override
	{
		const qint64 size = qMin(maxSize, qint64(m_data.size() - m_offset));
		if (size <= 0){
			return -1;
		}

		memcpy(dataPtr, m_data.constData() + m_offset, size);
		m_offset += size;

		return size;
	}

private:
	QByteArray m_data;
	qint64 m_offset;
};


class CGqlTestNetworkAccessManager: public QNetworkAccessManager
{
public:
	CGqlTestNetworkAccessManager(CGqlTestTransport& transport, QObject* parentPtr)
		:QNetworkAccessManager(parentPtr),
		m_transport(transport)
	{
	}

protected:
	// reimplemented (QNetworkAccessManager)
	virtual QNetworkReply* createRequest(Operation operation, const QNetworkRequest& request, QIODevice* outgoingDataPtr) override
	{
		if (operation != PostOperation){
			return QNetworkAccessManager::createRequest(operation, request, outgoingDataPtr);
		}

		const QByteArray requestBody = (outgoingDataPtr != nullptr) ? outgoingDataPtr->readAll() : QByteArray();

		QVariantMap requestHeaders;
		for (const QByteArray& headerId: request.rawHeaderList()){
			requestHeaders.insert(QString::fromUtf8(headerId).toLower(), QString::fromUtf8(request.rawHeader(headerId)));
		}

		m_transport.RegisterRequest(requestBody, requestHeaders);

		return new CGqlTestReply(operation, request, m_transport.GetResponseStatus(), m_transport.GetResponseBody().toUtf8(), this);
	}

private:
	CGqlTestTransport& m_transport;
};


QVariantMap CreateFieldsMap(const imtgql::CGqlFieldObject& fieldObject)
{
	QVariantMap retVal;

	for (const QByteArray& fieldId: fieldObject.GetFieldIds()){
		const imtgql::CGqlFieldObject* childObjectPtr = fieldObject.GetFieldArgumentObjectPtr(fieldId);
		if (childObjectPtr != nullptr){
			retVal.insert(QString::fromUtf8(fieldId), CreateFieldsMap(*childObjectPtr));
		}
		else{
			retVal.insert(QString::fromUtf8(fieldId), true);
		}
	}

	return retVal;
}


QVariant CreateParamValue(const QVariant& value)
{
	if (!value.isValid() || value.isNull()){
		return QVariant::fromValue(nullptr);
	}

	if (value.typeId() == qMetaTypeId<imtgql::CGqlEnum>()){
		imtgql::CGqlEnum enumValue = value.value<imtgql::CGqlEnum>();

		return QString::fromUtf8(enumValue.GetValue());
	}

	if (value.typeId() == QMetaType::QVariantList){
		QVariantList retVal;
		for (const QVariant& item: value.toList()){
			retVal.append(CreateParamValue(item));
		}

		return retVal;
	}

	return value;
}


QVariantMap CreateParamsMap(const imtgql::CGqlParamObject& paramObject)
{
	QVariantMap retVal;

	for (const QByteArray& paramId: paramObject.GetParamIds()){
		const QString key = QString::fromUtf8(paramId);

		if (paramObject.IsObject(paramId)){
			retVal.insert(key, CreateParamsMap(*paramObject.GetParamArgumentObjectPtr(paramId)));
		}
		else if (paramObject.IsObjectList(paramId)){
			QVariantList objectList;
			for (const imtgql::CGqlParamObject* itemPtr: paramObject.GetParamArgumentObjectPtrList(paramId)){
				if (itemPtr == nullptr || itemPtr->IsNull()){
					objectList.append(QVariant::fromValue(nullptr));
				}
				else{
					objectList.append(CreateParamsMap(*itemPtr));
				}
			}

			retVal.insert(key, objectList);
		}
		else{
			retVal.insert(key, CreateParamValue(paramObject.GetParamArgumentValue(paramId)));
		}
	}

	return retVal;
}


QString GetRequestTypeName(imtgql::IGqlRequest::RequestType requestType)
{
	switch (requestType){
	case imtgql::IGqlRequest::RT_QUERY:
		return QStringLiteral("query");
	case imtgql::IGqlRequest::RT_MUTATION:
		return QStringLiteral("mutation");
	case imtgql::IGqlRequest::RT_SUBSCRIPTION:
		return QStringLiteral("subscription");
	default:
		return QString();
	}
}


} // namespace


// public methods of the class CGqlTestTransport

CGqlTestTransport::CGqlTestTransport(QObject* parentPtr)
	:QObject(parentPtr),
	m_requestCount(0),
	m_responseBody(s_defaultResponseBody),
	m_responseStatus(s_defaultResponseStatus)
{
}


int CGqlTestTransport::GetRequestCount() const
{
	return m_requestCount;
}


QString CGqlTestTransport::GetLastRequestBody() const
{
	return QString::fromUtf8(m_lastRequestBody);
}


QVariantMap CGqlTestTransport::GetLastRequestHeaders() const
{
	return m_lastRequestHeaders;
}


QString CGqlTestTransport::GetResponseBody() const
{
	return QString::fromUtf8(m_responseBody);
}


void CGqlTestTransport::SetResponseBody(const QString& responseBody)
{
	const QByteArray body = responseBody.toUtf8();
	if (m_responseBody != body){
		m_responseBody = body;

		Q_EMIT responseChanged();
	}
}


int CGqlTestTransport::GetResponseStatus() const
{
	return m_responseStatus;
}


void CGqlTestTransport::SetResponseStatus(int responseStatus)
{
	if (m_responseStatus != responseStatus){
		m_responseStatus = responseStatus;

		Q_EMIT responseChanged();
	}
}


void CGqlTestTransport::RegisterRequest(const QByteArray& requestBody, const QVariantMap& requestHeaders)
{
	m_lastRequestBody = requestBody;
	m_lastRequestHeaders = requestHeaders;
	++m_requestCount;

	Q_EMIT requestReceived();
}


void CGqlTestTransport::reset()
{
	m_requestCount = 0;
	m_lastRequestBody.clear();
	m_lastRequestHeaders.clear();
	Q_EMIT requestReceived();

	SetResponseBody(QString::fromUtf8(s_defaultResponseBody));
	SetResponseStatus(s_defaultResponseStatus);
}


QVariantMap CGqlTestTransport::parseRequest(const QString& requestBody) const
{
	imtgql::CGqlRequest request;
	qsizetype errorPosition = -1;
	const bool isParsed = request.ParseQuery(requestBody.toUtf8(), errorPosition);

	QVariantMap retVal;
	retVal.insert(QStringLiteral("parsed"), isParsed);
	retVal.insert(QStringLiteral("errorPosition"), errorPosition);
	if (isParsed){
		retVal.insert(QStringLiteral("requestType"), GetRequestTypeName(request.GetRequestType()));
		retVal.insert(QStringLiteral("commandId"), QString::fromUtf8(request.GetCommandId()));
		retVal.insert(QStringLiteral("fields"), CreateFieldsMap(request.GetFields()));
		retVal.insert(QStringLiteral("params"), CreateParamsMap(request.GetParams()));
	}

	return retVal;
}


QVariantMap CGqlTestTransport::parseLastRequest() const
{
	return parseRequest(GetLastRequestBody());
}


// public methods of the class CGqlTestNetworkAccessManagerFactory

CGqlTestNetworkAccessManagerFactory::CGqlTestNetworkAccessManagerFactory(CGqlTestTransport& transport)
	:m_transport(transport)
{
}


// reimplemented (QQmlNetworkAccessManagerFactory)

QNetworkAccessManager* CGqlTestNetworkAccessManagerFactory::create(QObject* parentPtr)
{
	return new CGqlTestNetworkAccessManager(m_transport, parentPtr);
}


} // namespace imtqmltest


