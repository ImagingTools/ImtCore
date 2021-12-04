#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdb/IDatabaseObjectDelegate.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtlic/IProductLicensingInfo.h>


namespace imtlicdb
{


class CProductsDatabaseDelegateComp:
			public icomp::CComponentBase,
			virtual public imtdb::IDatabaseObjectDelegate
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductsDatabaseDelegateComp)
		I_REGISTER_INTERFACE(imtdb::IDatabaseObjectDelegate);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine used for querying the feature info", true, "DatabaseEngine");
		I_ASSIGN(m_productFactCompPtr, "ProductFactory", "Factory used for creation of the new product instance", true, "ProductFactory");
	I_END_COMPONENT

	// reimplemented (imtdb::IDatabaseObjectDelegate)
	virtual istd::IChangeable* CreateObjectFromRecord(
				const QByteArray& typeId,
				const QSqlRecord& record,
				QString& objectName,
				QString& objectDescription,
				QDateTime& lastModified,
				QDateTime& added) const override;
	virtual QByteArray CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr) const override;
	virtual QByteArray CreateDeleteObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object) const override;
	virtual QByteArray CreateRenameObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& newObjectName) const override;
private:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_FACT(imtlic::IProductLicensingInfo, m_productFactCompPtr);
};


} // namespace imtlicdb


