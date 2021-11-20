#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdb/IDatabaseObjectDelegate.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtlicdb
{


class CFeaturePackageDatabaseDelegateComp:
			public icomp::CComponentBase,
			virtual public imtdb::IDatabaseObjectDelegate
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFeaturePackageDatabaseDelegateComp)
		I_REGISTER_INTERFACE(imtdb::IDatabaseObjectDelegate);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine used for querying the feature info", true, "DatabaseEngine");
	I_END_COMPONENT

	// reimplemented (imtdb::IDatabaseObjectDelegate)
	virtual istd::IChangeable* CreateObjectFromRecord(
				const QByteArray& typeId,
				const QSqlRecord& record,
				QString& objectName,
				QString& objectDescription) const override;
	virtual QByteArray CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr) const override;
	virtual QByteArray CreateDeleteObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId) const override;

private:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
};


} // namespace imtlicdb


