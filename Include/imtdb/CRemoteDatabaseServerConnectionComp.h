#pragma once


// ImtCore includes
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <imtgql/TClientRequestManagerCompWrap.h>


namespace imtdb
{


class CRemoteDatabaseServerConnectionComp:
			public imtgql::CClientRequestManagerCompBase,
			virtual public IDatabaseServerConnectionChecker
{
public:
	typedef imtgql::CClientRequestManagerCompBase BaseClass;

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


