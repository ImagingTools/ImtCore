#pragma once


// ImtCore includes
#include <imtauthdb/CPersonalAccessTokenDatabaseDelegateComp.h>


namespace imtauthdb
{


class CSqlitePersonalAccessTokenDatabaseDelegateComp: public imtauthdb::CPersonalAccessTokenDatabaseDelegateComp
{
public:
	typedef imtauthdb::CPersonalAccessTokenDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CSqlitePersonalAccessTokenDatabaseDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual QByteArray CreateTableQuery(const QByteArray& typeId) const override;
};


} // namespace imtauthdb


