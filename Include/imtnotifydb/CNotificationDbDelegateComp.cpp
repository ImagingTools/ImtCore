// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotifydb/CNotificationDbDelegateComp.h>


// Qt includes
#include <QtCore/QTimeZone>
#include <QtSql/QSqlRecord>

// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtdb/imtdb.h>
#include <imtnotify/CNotification.h>
#include <imtnotify/NotificationSelection.h>
#include <imtnotifydb/NotificationRecord.h>


namespace
{


using namespace imtnotifydb::NotificationRecord;


const QString s_nothing = QStringLiteral("1 = 0");

// Bounds of a single request
const int s_maxSelectionSize = 1000;
const int s_maxFilterDepth = 4;
const int s_maxSearchWords = 10;


QByteArray GetIdParam(const iprm::IParamsSet& params, const QByteArray& paramId)
{
	iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&params, paramId, false);

	return idParamPtr.IsValid() ? idParamPtr->GetId() : QByteArray();
}


} // namespace


namespace imtnotifydb
{


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeableUniquePtr CNotificationDbDelegateComp::CreateObjectFromRecord(
			const QSqlRecord& record,
			const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	imtnotify::NotificationScope scope;
	scope.productId = imtdb::VariantToByteArray(record.value(QStringLiteral("ProductId")));
	scope.tenantId = imtdb::VariantToByteArray(record.value(QStringLiteral("TenantId")));
	scope.userId = imtdb::VariantToByteArray(record.value(QStringLiteral("RecipientId")));

	imtnotify::ObjectLinkInfo subject;
	subject.id = imtdb::VariantToByteArray(record.value(QStringLiteral("SubjectId")));
	subject.typeId = imtdb::VariantToByteArray(record.value(QStringLiteral("SubjectTypeId")));
	subject.name = record.value(QStringLiteral("SubjectName")).toString();

	imtnotify::CNotification* notificationPtr = new imtnotify::CNotification;
	notificationPtr->SetCategory(istd::IInformationProvider::InformationCategory(qBound(
				int(istd::IInformationProvider::IC_INFO),
				record.value(QStringLiteral("Severity")).toInt(),
				int(istd::IInformationProvider::IC_CRITICAL))));
	notificationPtr->SetDescription(record.value(QStringLiteral("Body")).toString());
	notificationPtr->SetActors(ActorsFromJson(record.value(QStringLiteral("Actors")).toString()));
	notificationPtr->SetArguments(ArgumentsFromJson(record.value(QStringLiteral("Arguments")).toString()));
	notificationPtr->SetTimeStamp(QDateTime::fromString(record.value(QStringLiteral("CreatedAt")).toString(), Qt::ISODateWithMs));
	notificationPtr->SetScope(scope);
	notificationPtr->SetTypeId(imtdb::VariantToByteArray(record.value(QStringLiteral("TypeId"))));
	notificationPtr->SetSubject(subject);
	notificationPtr->SetGroupKey(imtdb::VariantToByteArray(record.value(QStringLiteral("GroupKey"))));
	notificationPtr->SetGroupCount(record.value(QStringLiteral("GroupCount")).toInt());
	notificationPtr->SetDelivered(!record.value(QStringLiteral("DeliveredAt")).isNull());
	notificationPtr->SetRead(!record.value(QStringLiteral("ReadAt")).isNull());

	return istd::IChangeableUniquePtr(notificationPtr);
}


QByteArray CNotificationDbDelegateComp::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	// The type of a notification is its event type, it is not needed to select it
	return QByteArray();
}


QByteArray CNotificationDbDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	// By id for the server only, e.g. the publisher of the changes
	if (!objectId.isEmpty()){
		return QStringLiteral(R"(SELECT * FROM %1 WHERE "Id" = '%2')").arg(s_tableName, SqlEncode(QString::fromUtf8(objectId))).toUtf8();
	}

	if (paramsPtr == nullptr || count == 0){
		return QByteArray();
	}

	QString filterQuery;
	QString sortQuery;
	if (!CreateFilterQuery(*paramsPtr, filterQuery)){
		return QByteArray();
	}

	iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(paramsPtr, QByteArrayLiteral("ComplexFilter"), false);
	iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(paramsPtr, QByteArrayLiteral("Filter"), false);
	if (complexFilterParamPtr.IsValid()){
		CreateSortQuery(*complexFilterParamPtr, sortQuery);
	}
	else if (collectionFilterParamPtr.IsValid()){
		CreateSortQuery(*collectionFilterParamPtr, sortQuery);
	}

	if (sortQuery.isEmpty()){
		sortQuery = CreateOrderQuery(imtnotify::NotificationField::s_createdAt, false);
	}

	// The collection pages an unbounded selection itself; a selection never reads more than the newest rows
	QByteArray paginationQuery;
	if (!CreatePaginationQuery(offset, (count < 0 || count > s_maxSelectionSize) ? s_maxSelectionSize : count, paginationQuery)){
		return QByteArray();
	}

	// The page is cut after the sorting in both databases
	return QStringLiteral("%1 %2 %3 %4")
				.arg(GetBaseSelectionQuery(), filterQuery, sortQuery, QString::fromUtf8(paginationQuery))
				.toUtf8();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CNotificationDbDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* /*valuePtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	// Notifications are stored by the inbox controller
	return NewObjectQuery();
}


QByteArray CNotificationDbDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	return QByteArray();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CNotificationDbDelegateComp::CreateUpdateObjectQueryWithParameters(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	// Notifications are changed by the inbox controller
	return NewObjectQuery();
}


QByteArray CNotificationDbDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArrayList& /*objectIds*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	// Notifications are removed by the inbox controller, which tells the sessions
	return QByteArray();
}


QByteArray CNotificationDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CNotificationDbDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CNotificationDbDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


// protected methods

// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

QString CNotificationDbDelegateComp::SqlEncode(const QString& sqlQuery) const
{
	// The values are in single quotes; the base would also change the semicolon, so keys and words with it would not match
	QString retVal = sqlQuery;
	retVal.replace(QLatin1Char('\''), QStringLiteral("''"));

	return retVal;
}


QByteArray CNotificationDbDelegateComp::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	// Without an inbox nothing is counted
	if (paramsPtr == nullptr){
		return QStringLiteral("SELECT COUNT(*) FROM %1 WHERE %2").arg(s_tableName, s_nothing).toUtf8();
	}

	return BaseClass::GetCountQuery(paramsPtr);
}


bool CNotificationDbDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& /*filterParams*/, QString& /*filterQuery*/) const
{
	// The "ObjectFilter" of the base names the columns directly; the fields are filtered by the complex filter only
	return true;
}


bool CNotificationDbDelegateComp::CreateObjectFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& filterQuery) const
{
	filterQuery = CreateExpressionQuery(collectionFilter.GetFilterExpression(), 0);

	return true;
}


bool CNotificationDbDelegateComp::CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const
{
	textFilterQuery = CreateSearchQuery(collectionFilter.GetTextFilter());

	return true;
}


bool CNotificationDbDelegateComp::CreateTextFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& textFilterQuery) const
{
	textFilterQuery = CreateSearchQuery(collectionFilter.GetTextFilter());

	return true;
}


bool CNotificationDbDelegateComp::CreateTimeFilterQuery(
			const imtbase::ITimeFilterParam& timeFilter,
			QString& timeFilterQuery,
			const QString& /*timeFieldId*/) const
{
	timeFilterQuery.clear();

	// The units are counted in UTC
	QDateTime begin;
	QDateTime end;
	imtdb::GetTimeFilterBounds(timeFilter, QDateTime::currentDateTimeUtc(), begin, end);

	QStringList conditions;
	if (begin.isValid()){
		conditions << QStringLiteral(R"("CreatedAt" >= '%1')").arg(TimeToText(begin));
	}

	if (end.isValid()){
		conditions << QStringLiteral(R"("CreatedAt" < '%1')").arg(TimeToText(end));
	}

	timeFilterQuery = conditions.join(QStringLiteral(" AND "));

	return true;
}


QString CNotificationDbDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	const QByteArray userId = GetIdParam(filterParams, imtnotify::s_inboxUserIdParamId);
	if (userId.isEmpty()){
		return s_nothing;
	}

	const QString productId = SqlEncode(QString::fromUtf8(GetIdParam(filterParams, imtnotify::s_inboxProductIdParamId)));
	const QString tenantId = SqlEncode(QString::fromUtf8(GetIdParam(filterParams, imtnotify::s_inboxTenantIdParamId)));

	// The inbox of the session and the personal notifications
	return QStringLiteral(
				R"("ProductId" IN ('%1', '') AND "TenantId" IN ('%2', '') AND "RecipientId" = '%3')")
				.arg(productId, tenantId, SqlEncode(QString::fromUtf8(userId)));
}


bool CNotificationDbDelegateComp::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
{
	sortQuery.clear();

	const QByteArrayList sortingInfoIds = collectionFilter.GetSortingInfoIds();
	const imtbase::ICollectionFilter::SortingOrder sortingOrder = collectionFilter.GetSortingOrder();
	if (!sortingInfoIds.isEmpty() && sortingOrder != imtbase::ICollectionFilter::SO_NO_ORDER){
		sortQuery = CreateOrderQuery(sortingInfoIds.first(), sortingOrder == imtbase::ICollectionFilter::SO_ASC);
	}

	return true;
}


bool CNotificationDbDelegateComp::CreateSortQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& sortQuery) const
{
	sortQuery.clear();

	for (const imtbase::IComplexCollectionFilter::FieldInfo& info : collectionFilter.GetFields()){
		if (info.metaInfo.sortingOrder != imtbase::IComplexCollectionFilter::SO_NO_ORDER){
			sortQuery = CreateOrderQuery(info.id, info.metaInfo.sortingOrder == imtbase::IComplexCollectionFilter::SO_ASC);
			if (!sortQuery.isEmpty()){
				break;
			}
		}
	}

	return true;
}


// private methods

QString CNotificationDbDelegateComp::CreateExpressionQuery(const imtbase::IComplexCollectionFilter::FilterExpression& expression, int depth) const
{
	if (depth > s_maxFilterDepth){
		return s_nothing;
	}

	QStringList parts;
	for (const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : expression.fieldFilters){
		parts << CreateFieldFilterQuery(fieldFilter);
	}

	for (const imtbase::IComplexCollectionFilter::FilterExpression& subExpression : expression.filterExpressions){
		const QString subQuery = CreateExpressionQuery(subExpression, depth + 1);
		if (!subQuery.isEmpty()){
			parts << subQuery;
		}
	}

	if (parts.isEmpty()){
		return QString();
	}

	const QString logicalOperation = (expression.logicalOperation == imtbase::IComplexCollectionFilter::LO_OR) ? QStringLiteral(" OR ") : QStringLiteral(" AND ");

	return QLatin1Char('(') + parts.join(logicalOperation) + QLatin1Char(')');
}


QString CNotificationDbDelegateComp::CreateFieldFilterQuery(const imtbase::IComplexCollectionFilter::FieldFilter& filter) const
{
	const imtbase::IComplexCollectionFilter::FieldOperation operation = filter.filterOperation;
	const bool isEqual = (operation == imtbase::IComplexCollectionFilter::FO_EQUAL);
	const bool isNotEqual = (operation == imtbase::IComplexCollectionFilter::FO_NOT_EQUAL);

	// A field or an operation the table does not know selects nothing, so a typo never widens the selection
	if (filter.fieldId == imtnotify::NotificationField::s_severity && (isEqual || isNotEqual)){
		bool isNumber = false;
		const int severity = filter.filterValue.toInt(&isNumber);
		if (!isNumber){
			return s_nothing;
		}

		return QStringLiteral(R"("Severity" %1 %2)").arg(isEqual ? QStringLiteral("=") : QStringLiteral("<>"), QString::number(severity));
	}

	if ((filter.fieldId == imtnotify::NotificationField::s_isRead || filter.fieldId == imtnotify::NotificationField::s_isDelivered) && (isEqual || isNotEqual)){
		const QString column = (filter.fieldId == imtnotify::NotificationField::s_isRead) ? QStringLiteral(R"("ReadAt")") : QStringLiteral(R"("DeliveredAt")");
		const bool isSet = (filter.filterValue.toBool() == isEqual);

		return column + (isSet ? QStringLiteral(" IS NOT NULL") : QStringLiteral(" IS NULL"));
	}

	const QMap<QByteArray, QString> textColumns = {
		{imtnotify::NotificationField::s_typeId, QStringLiteral(R"("TypeId")")},
		{imtnotify::NotificationField::s_productId, QStringLiteral(R"("ProductId")")},
		{imtnotify::NotificationField::s_tenantId, QStringLiteral(R"("TenantId")")},
		{imtnotify::NotificationField::s_groupKey, QStringLiteral(R"("GroupKey")")}};
	if (textColumns.contains(filter.fieldId) && (isEqual || isNotEqual)){
		return QStringLiteral("%1 %2 '%3'").arg(textColumns[filter.fieldId], isEqual ? QStringLiteral("=") : QStringLiteral("<>"), SqlEncode(filter.filterValue.toString()));
	}

	if (filter.fieldId == imtnotify::NotificationField::s_createdAt){
		const QDateTime time = QDateTime::fromString(filter.filterValue.toString(), Qt::ISODateWithMs);
		if (!time.isValid()){
			return s_nothing;
		}

		QString comparison;
		switch (operation){
		case imtbase::IComplexCollectionFilter::FO_LESS:
			comparison = QStringLiteral("<");
			break;
		case imtbase::IComplexCollectionFilter::FO_GREATER:
			comparison = QStringLiteral(">");
			break;
		case imtbase::IComplexCollectionFilter::FO_NOT_LESS:
			comparison = QStringLiteral(">=");
			break;
		case imtbase::IComplexCollectionFilter::FO_NOT_GREATER:
			comparison = QStringLiteral("<=");
			break;
		default:
			return s_nothing;
		}

		// The column holds ISO 8601 UTC text, which sorts as the time
		return QStringLiteral(R"("CreatedAt" %1 '%2')").arg(comparison, TimeToText(time));
	}

	return s_nothing;
}


QString CNotificationDbDelegateComp::CreateSearchQuery(const QString& text) const
{
	QStringList conditions;

	// Every word has to be found; "SearchText" is lowered by Qt, since SQLite lowers only ASCII
	const QStringList words = text.toLower().split(QLatin1Char(' '), Qt::SkipEmptyParts).mid(0, s_maxSearchWords);
	for (QString word : words){
		word.replace(QLatin1Char('\\'), QStringLiteral("\\\\"));
		word.replace(QLatin1Char('%'), QStringLiteral("\\%"));
		word.replace(QLatin1Char('_'), QStringLiteral("\\_"));

		// The pattern is concatenated: a literal '%<number>%' would be taken for a placeholder by QString::arg
		conditions << QStringLiteral(R"("SearchText" LIKE '%' || '%1' || '%' ESCAPE '\')").arg(SqlEncode(word));
	}

	return conditions.join(QStringLiteral(" AND "));
}


QString CNotificationDbDelegateComp::CreateOrderQuery(const QByteArray& fieldId, bool ascending) const
{
	QString column;
	if (fieldId == imtnotify::NotificationField::s_createdAt){
		column = QStringLiteral(R"("CreatedAt")");
	}
	else if (fieldId == imtnotify::NotificationField::s_severity){
		column = QStringLiteral(R"("Severity")");
	}
	else{
		return QString();
	}

	const QString order = ascending ? QStringLiteral("ASC") : QStringLiteral("DESC");

	// The newest first among equal values, and "Id" keeps the pages stable
	return QStringLiteral(R"(ORDER BY %1 %2, "CreatedAt" DESC, "Id" DESC)").arg(column, order);
}


} // namespace imtnotifydb


