// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/IClientProtocolEngine.h>


namespace imtclientgql
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
	virtual GqlResponsePtr SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParamPtr = nullptr) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtclientgql::IClientProtocolEngine, m_protocolEngineCompPtr);
	I_ATTR(double, m_timeoutAttrPtr);

	int m_timeout;
};


} // namespace imtclientgql


