#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>
#include <imtlic/IProductLicensingInfo.h>


namespace imtlicdb
{


class CProductsDatabaseDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductsDatabaseDelegateComp)
		I_ASSIGN(m_productFactCompPtr, "ProductFactory", "Factory used for creation of the new product instance", true, "ProductFactory");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeable* CreateObjectFromRecord(
				const QByteArray& typeId,
				const QSqlRecord& record,
				const QSqlQuery& query) const override;
	virtual NewObjectQuery CreateNewObjectQuery(
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
	virtual QByteArray CreateDescriptionObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& description) const override;
protected:
	void GenerateDifferences(
				const imtlic::IProductLicensingInfo* currentProductPtr,
				const imtlic::IProductLicensingInfo* newProductPtr,
				QByteArrayList& addLicenses,
				QByteArrayList& removedLicenses,
				QByteArrayList& updatedLicenses) const;

	void GenerateDifferences(const imtlic::ILicenseInfo* currentLicensePtr,
				const imtlic::ILicenseInfo* newLicensePtr,
				QByteArrayList& addFeatures,
				QByteArrayList& removedFeatures) const;
private:
	I_FACT(imtlic::IProductLicensingInfo, m_productFactCompPtr);
};


} // namespace imtlicdb


