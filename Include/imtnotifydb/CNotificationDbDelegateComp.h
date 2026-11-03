// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


namespace imtnotifydb
{


/**
	SQL delegate of the collection of the notifications, the table "UserNotifications" of PostgreSQL or SQLite.
	The collection only reads: every change goes through CSqlNotificationInboxControllerComp, so the writing queries are empty.
	A selection of a list is restricted to the inbox given by the parameters imtnotify::s_inbox*ParamId, without them it
	is empty; a selection by id is for the server only (the collection controller does not offer it to clients).
	Only the fields of imtnotify::NotificationField can be filtered; the text filter looks for every word in "SearchText".
	A selection never reads more than the newest s_maxSelectionSize rows.
	The creation script also creates the table of the dedup keys of the inbox controller.
*/
class CNotificationDbDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CNotificationDbDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeableUniquePtr CreateObjectFromRecord(
				const QSqlRecord& record,
				const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual QByteArray GetObjectTypeId(const QByteArray& objectId) const override;
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
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
	virtual NewObjectQuery CreateUpdateObjectQueryWithParameters(
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

protected:
	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual QString SqlEncode(const QString& sqlQuery) const override;
	virtual QByteArray GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateObjectFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& filterQuery) const override;
	virtual bool CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const override;
	virtual bool CreateTextFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& textFilterQuery) const override;
	virtual bool CreateTimeFilterQuery(
				const imtbase::ITimeFilterParam& timeFilter,
				QString& timeFilterQuery,
				const QString& timeFieldId = QStringLiteral(R"("CreatedAt")")) const override;
	virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const override;
	virtual bool CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const override;
	virtual bool CreateSortQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& sortQuery) const override;

private:
	QString CreateExpressionQuery(const imtbase::IComplexCollectionFilter::FilterExpression& expression, int depth) const;
	QString CreateFieldFilterQuery(const imtbase::IComplexCollectionFilter::FieldFilter& filter) const;
	QString CreateSearchQuery(const QString& text) const;
	QString CreateOrderQuery(const QByteArray& fieldId, bool ascending) const;
};


} // namespace imtnotifydb


