#pragma once


// ImtCore includes
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtdb
{


class CRemoteDatabaseServerConnectionComp:
			public imtclientgql::CClientRequestManagerCompBase,
			virtual public IDatabaseServerConnectionChecker
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteDatabaseServerConnectionComp);
		I_REGISTER_INTERFACE(IDatabaseServerConnectionChecker)
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "Request command-ID", false, "GetDatabaseStatus");
	I_END_COMPONENT;

	// reimplemented (IDatabaseServerConnectionChecker)
	virtual bool IsDatabaseServerConnected(QString& errorMessage) const override;

private:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
};


} // namespace imtdb


