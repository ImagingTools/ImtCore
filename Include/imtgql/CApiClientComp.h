#pragma once


// Qt includes
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

// ACF includes
#include <istd/TPointerVector.h>
#include <ibase/IProgressManager.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgql/IGqlClient.h>
#include <imtgql/IClientProtocolEngine.h>


namespace imtgql
{


class CApiClientComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public IGqlClient
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CApiClientComp);
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used for the communication with the server", true, "ProtocolEngine");
	I_END_COMPONENT

	CApiClientComp();

	// reimplemented (IGqlClient)
	virtual bool SendRequest(const IGqlRequest& request, const QByteArray& data, ResponseHandler& responseHandler) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private Q_SLOTS:
	void OnReply();

private:
	I_REF(IClientProtocolEngine, m_protocolEngineCompPtr);

	struct RequestInfo
	{
		RequestInfo()
			:replyPtr(nullptr),
			responseHandlerPtr(nullptr)
		{
		}

		QNetworkReply* replyPtr;
		IGqlClient::ResponseHandler* responseHandlerPtr;
	};

	typedef QVector<RequestInfo> RequestInfos;

	mutable RequestInfos m_requestInfos;

	int m_timeout;

	ibase::IProgressManager* m_progressManagerPtr;
};


} // namespace imtgql


