#pragma once


// ImtCore includes
#include <imtdb/CSqliteJsonDatabaseDelegateComp.h>


namespace imtauthdb
{


class CSqliteRoleDatabaseDelegateComp: public imtdb::CSqliteJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqliteJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CSqliteRoleDatabaseDelegateComp)
	I_END_COMPONENT

protected:
	// reimplemented (imtdb::CSqliteJsonDatabaseDelegateComp)
	virtual bool SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;
};


} // namespace imtauthdb


