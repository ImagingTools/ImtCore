// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


namespace imtcache
{


/**
	Base for cache-table delegates.

	Cache tables are written only by the cache builders, never through imtbase::IObjectCollection
	CRUD, so every write-path query this delegate would have to produce is meaningless here. The six
	such methods that imtdb::CSqlDatabaseObjectDelegateCompBase leaves pure are answered once with an
	empty query, instead of every cache delegate carrying the same block of stubs.

	Subclasses implement only the read path: GetObjectTypeId(), GetObjectIdFromRecord() and
	CreateObjectFromRecord().
*/
class CCacheObjectDelegateCompBase: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	using BaseClass = imtdb::CSqlDatabaseObjectDelegateCompBase;

	I_BEGIN_BASE_COMPONENT(CCacheObjectDelegateCompBase)
	I_END_COMPONENT;

protected:
	// reimplemented (imtdb::IDatabaseObjectDelegate)
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
};


} // namespace imtcache
