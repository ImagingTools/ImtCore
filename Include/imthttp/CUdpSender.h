#pragma once


// Qt includes
#include <QtNetwork/QUdpSocket>

// ImtCore includes
#include <imthttp/ISender.h>
#include <imthttp/CUdpRequest.h>


namespace imthttp
{


class CUdpSender: public QObject, virtual public ISender
{
	Q_OBJECT
public:
	CUdpSender(CUdpRequest* request);

	// reimplemented (ISender)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& reguest) const override;

Q_SIGNALS:
    void sended(QByteArray requestId) const;
private:
	QUdpSocket* m_socket;
	QHostAddress m_address;
	unsigned int m_port = 0;
    QByteArray m_requestId;
};


} // namespace imthttp


