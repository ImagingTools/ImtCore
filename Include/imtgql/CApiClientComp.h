#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgql/IGqlClient.h>
#include <imtgql/IClientProtocolEngine.h>


namespace imtgql
{


class CApiClientComp:
			public ilog::CLoggerComponentBase,
			virtual public IGqlClient
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CApiClientComp);
		I_REGISTER_INTERFACE(IGqlClient);
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used for the communication with the server", true, "ProtocolEngine");
		I_ASSIGN(m_timeoutAttrPtr, "Timeout", "Request timeout in seconds", true, 30.0);
	I_END_COMPONENT

	CApiClientComp();

	// reimplemented (IGqlClient)
	virtual bool SendRequest(const IGqlRequest& request, ResponseHandler& responseHandler) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private:
	I_REF(IClientProtocolEngine, m_protocolEngineCompPtr);
	I_ATTR(double, m_timeoutAttrPtr);

	int m_timeout;
};


} // namespace imtgql


