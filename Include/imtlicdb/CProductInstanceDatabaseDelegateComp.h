#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>
#include <imtlic/IProductInstanceInfo.h>


namespace imtlicdb
{


class CProductInstanceDatabaseDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductInstanceDatabaseDelegateComp)
		I_ASSIGN(m_productInstanceFactCompPtr, "ProductInstanceFactory", "Factory used for creation of the new product instance", true, "ProductInstanceFactory");
		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Meta information creator for the product instance", true, "MetaInfoCreator");
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
	void GenerateDifferencesLicenses(const imtlic::IProductInstanceInfo* currentProductInstancePtr,
				const imtlic::IProductInstanceInfo* newProductInstancePtr,
				QByteArrayList& addLicenseInstances,
				QByteArrayList& removedLicenseInstances,
				QByteArrayList& updatedLicenseInstances) const;

	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual idoc::IDocumentMetaInfo* CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;

private:
	I_FACT(imtlic::IProductInstanceInfo, m_productInstanceFactCompPtr);
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);
};


} // namespace imtlicdb


