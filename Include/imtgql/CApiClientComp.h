#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QReadWriteLock>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

// ACF includes
#include <istd/TPointerVector.h>
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
		I_REGISTER_INTERFACE(IGqlClient);
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used for the communication with the server", true, "ProtocolEngine");
	I_END_COMPONENT

	CApiClientComp();

	// reimplemented (IGqlClient)
	virtual bool SendRequest(const IGqlRequest& request, ResponseHandler& responseHandler) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private Q_SLOTS:
	void OnReply();

private:
	I_REF(IClientProtocolEngine, m_protocolEngineCompPtr);

	struct Response
	{
		Response()
			:responseHandlerPtr(nullptr)
		{
		}

		istd::TSmartPtr<imtgql::IGqlRequest> requestPtr;
		IGqlClient::ResponseHandler* responseHandlerPtr;
	};

	typedef QMap<QNetworkReply*, Response> RequestMap;
	mutable RequestMap m_requestMap;
	mutable QReadWriteLock m_requestMapMutex;

	int m_timeout;
};


} // namespace imtgql


