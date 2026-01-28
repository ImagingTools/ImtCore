#pragma once


// Qt includes
#include <QtNetwork/QAbstractSocket>

// ImtCore includes
#include <imthttp/ISender.h>

namespace imthttp
{


class CTcpSender: public QObject, virtual public ISender
{
	Q_OBJECT
public:
    CTcpSender(QAbstractSocket* tcpSocketPtr);
	// reimplemented (ISender)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& reguest) const override;

protected:
    virtual bool WriteBody(const QByteArray& data, QAbstractSocket& socket) const;

	QAbstractSocket* m_tcpSocketPtr;
};


} // namespace imthttp


