#pragma once


// Qt includes
#include <QtNetwork/QAbstractSocket>

// ImtCore includes
#include <imtrest/ISender.h>

namespace imtrest
{


class CHttpSender: public QObject, virtual public ISender
{
	Q_OBJECT
public:
	// reimplemented (IResponder)
	virtual bool SendResponse(const IResponse& response) const override;
	virtual bool SendRequest(const IRequest& reguest) const override;

protected:
	virtual bool WriteStatus(int statusCode,const QByteArray& statusCodeLiteral, QAbstractSocket& socket) const;
	virtual bool WriteHeader(const QByteArray& headerKey, const QByteArray& value, QAbstractSocket& socket) const;
	virtual bool WriteBody(const QByteArray& data, QAbstractSocket& socket) const;
};


} // namespace imtrest


