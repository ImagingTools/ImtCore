#pragma once


// Qt includes
#include <QtCore/QObject>

// ImtCore includes
#include <imtrest/IResponder.h>


namespace imtrest
{


class CHttpResponder: public QObject, virtual public IResponder
{
	Q_OBJECT
public:
	// reimplemented (IResponder)
	virtual bool SendResponse(const IResponse& response, QIODevice& device) const override;

protected:
	virtual bool WriteStatus(int statusCode,const QByteArray& statusCodeLiteral, QIODevice& outputDevice) const;
	virtual bool WriteHeader(const QByteArray& headerKey, const QByteArray& value, QIODevice& outputDevice) const;
	virtual bool WriteBody(const QByteArray& data, QIODevice& outputDevice) const;
};


} // namespace imtrest


