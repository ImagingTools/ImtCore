// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>
#include <imtbase/IObjectCollection.h>
#include <imtgql/IGqlContext.h>


namespace imtdeskdb
{


class CSupportTicketDbDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CSupportTicketDbDelegateComp)
		I_ASSIGN(m_ticketFactCompPtr, "TicketFactory", "Factory used for creation of new ticket instances", true, "TicketFactory");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Users collection for DB-level visibility filtering", false, "Users");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = -1,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
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
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				bool useExternDelegate = true) const override;
	virtual QByteArray CreateDeleteObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArrayList& objectIds,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDeleteObjectSetQuery(
				const imtbase::IObjectCollection& collection,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
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
	virtual bool SetCollectionItemMetaInfoFromRecord(
				const QSqlRecord& record,
				idoc::IDocumentMetaInfo& metaInfo) const override;
	
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;

private:
	QString UtcNow() const;
	QString EscapeSql(const QString& value) const;
	QString EscapeSqlLikePattern(const QString& value) const;
	bool IsSqliteDatabase() const;
	QString CreateVisibilityCondition(const imtgql::IGqlContext* contextPtr) const;

	I_FACT(imtdesk::ISupportTicket, m_ticketFactCompPtr);
	I_FACT(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtdeskdb
