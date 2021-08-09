#pragma once


// ACF includes
#include <icomp/CComponentBase.h>


// ImtCore includes
#include <imtdb/IDatabaseObjectDelegate.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtlicdb
{


/**
	Common interface for a database object delegate.
	The delegate is responsible for the binding of the database layer to the object representation in the C++ data layer (ORM).
*/
class CFeatureInfoObjectDelegateComp: public icomp::CComponentBase, virtual public imtdb::IDatabaseObjectDelegate
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureInfoObjectDelegateComp)
		I_REGISTER_INTERFACE(imtdb::IDatabaseObjectDelegate);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine used for querying the feature info", true, "DatabaseEngine");
	I_END_COMPONENT

	// reimplemented (imtdb::IDatabaseObjectDelegate)
	virtual istd::IChangeable* CreateObjectFromRecord(const QByteArray& typeId, const QSqlRecord& record, QString& objectName, QString& objectDescription) const override;

private:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
};


} // namespace imtlicdb


