// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>
#include <imtgeo/CAddressElementInfo.h>


namespace imtgeo
{


class CAddressElementDatabaseDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	using BaseClass = imtdb::CSqlDatabaseObjectDelegateCompBase;

	I_BEGIN_COMPONENT(CAddressElementDatabaseDelegateComp)
		I_ASSIGN(m_adressFactoryCompPtr, "AddressElemenInfo", "Factory used for creation of the new address elemen instance", true, "AddressElemenInfo");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual QByteArray GetObjectTypeId(const QByteArray& objectId) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRecord(
				const QSqlRecord& record,
				const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDeleteObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDeleteObjectSetQuery(
				const imtbase::IObjectCollection& collection,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				bool useExternDelegate = true) const override;
	virtual QByteArray CreateRenameObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& newObjectName,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDescriptionObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual QString GetBaseSelectionQuery() const override;
	virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const override;

private:
	I_FACT(imtgeo::IAddressElementInfo, m_adressFactoryCompPtr);
};


} // namespace imtgeo


