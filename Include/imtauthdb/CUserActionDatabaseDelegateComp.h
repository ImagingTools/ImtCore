#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


namespace imtauthdb
{


class CUserActionDatabaseDelegateComp: public imtdb::CSqlDatabaseDocumentDelegateComp
{
public:
	typedef imtdb::CSqlDatabaseDocumentDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CUserActionDatabaseDelegateComp)
	I_END_COMPONENT

protected:
	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const override;
	virtual QByteArray CreateJoinTablesQuery() const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	virtual bool TableExists(const QString& tableName) const;
};


} // namespace imtauthdb


