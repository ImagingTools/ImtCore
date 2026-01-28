#pragma once


// Qt includes
#include <QtNetwork/QAbstractSocket>

// ImtCore includes
#include <imthttp/ISender.h>

namespace imthttp
{


class CHttpSender: public QObject, virtual public ISender
{
	Q_OBJECT
public:
	CHttpSender(QAbstractSocket* tcpSocketPtr);
	// reimplemented (ISender)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& reguest) const override;

protected:
	virtual bool WriteStatus(int statusCode,const QByteArray& statusCodeLiteral, QAbstractSocket& socket) const;
	virtual bool WriteHeader(const QByteArray& headerKey, const QByteArray& value, QAbstractSocket& socket) const;
	virtual bool WriteBody(const QByteArray& data, QAbstractSocket& socket) const;

	QAbstractSocket* m_tcpSocketPtr;
};


} // namespace imthttp


