#pragma once


// Qt includes
#include <QtCore/QObject>

// ImtCore includes
#include <imtrest/IResponse.h>


namespace imtrest
{


class CWebSocketResponse: public QObject, virtual public IResponse
{
	Q_OBJECT
public:
	CWebSocketResponse(
			int statusCode,
			const QByteArray& data,
			const QByteArray& dataTypeId,
			QObject& socketObject,
			const IProtocolEngine& engine);

	// reimplemented (IResponse)
	virtual int GetStatusCode() const override;
	virtual const QByteArray& GetData() const override;
	virtual QByteArray GetDataTypeId() const override;
	virtual Headers GetHeaders() const override;

	// reimplemented (INetworkObject)
	virtual const IProtocolEngine& GetProtocolEngine() const override;
	virtual QObject& GetSocketObject() const override;

	// reimplemented (istd::IChangeable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	const IProtocolEngine& m_engine;
	QObject& m_socket;

	Headers m_headers;
	int m_statusCode;
	QByteArray m_data;
	QByteArray m_dataTypeId;
};


} // namespace imtrest


