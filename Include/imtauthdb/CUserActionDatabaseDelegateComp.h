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
		I_ASSIGN_MULTI_0(m_databaseDelegatesCompPtr, "DatabaseDelegates", "Database delegates", false);
	I_END_COMPONENT

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual istd::IChangeableUniquePtr CreateObjectFromRecord(
				const QSqlRecord& record,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;

private:
	I_MULTIREF(imtdb::ISqlDatabaseObjectDelegate, m_databaseDelegatesCompPtr);
};


} // namespace imtauthdb


