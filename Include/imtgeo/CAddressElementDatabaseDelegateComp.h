#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>

// NeoPro includes
#include <imtgeo/CAddressElementInfo.h>


namespace imtgeo
{


class CAddressElementDatabaseDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CAddressElementDatabaseDelegateComp)
		I_ASSIGN(m_adrElementInfoFactCompPtr, "AddressElemenInfo", "Factory used for creation of the new address elemen instance", true, "AddressElemenInfo");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeable* CreateObjectFromRecord(
				const QSqlRecord& record) const override;
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
				const istd::IChangeable& object,
				const ContextDescription& description,
				bool useExternDelegate = true) const override;
	virtual QByteArray CreateRenameObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& newObjectName) const override;
	virtual QByteArray CreateDescriptionObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& description) const override;
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
    virtual bool CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual QByteArray GetObjectTypeId(
			const QByteArray& objectId) const override;

private:
    I_FACT(imtgeo::IAddressElementInfo, m_adrElementInfoFactCompPtr);
};


} // namespace imtgeo


