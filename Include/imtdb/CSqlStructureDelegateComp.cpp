#include <imtdb/CSqlStructureDelegateComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <istd/CCrcCalculator.h>
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtdb/imtdb.h>
#include <imtbase/ICollectionFilter.h>


namespace imtdb
{


// protected methods

QByteArray CSqlStructureDelegateComp::CreateInsertNewNodeQuery(
	const QString& name,
	const QString& description,
	const Id& proposedNodeId,
	const Id& parentNodeId,
	const idoc::IDocumentMetaInfo* metaInfoPtr,
	const imtbase::IOperationContext* operationContextPtr)
{
	QJsonObject nodeInfo;
	QJsonArray metaInfos = CtreateJsonFromMetaInfo(metaInfoPtr);
	nodeInfo.insert("metaInfo", metaInfos);
	QJsonDocument document;
	document.setObject(nodeInfo);
	QByteArray documentContent = document.toJson(QJsonDocument::Compact);
	QString queryString = QString("INSERT INTO public.\"Nodes\"(\"NodeId\", \"ParentId\", \"Name\", \"Description\", \"NodeInfo\", \"LastModified\") VALUES ('%1', '%2', '%3', '%4', '%5', '%6');")
				.arg(qPrintable(proposedNodeId))
				.arg(qPrintable(parentNodeId))
				.arg(name)
				.arg(description)
				.arg(SqlEncode(documentContent))
				.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

	return queryString.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateSetNodeNameQuery(
	const Id& nodeId,
	const QString& name,
	const imtbase::IOperationContext* operationContextPtr)
{
	QString query = QString("UPDATE  public.\"Nodes\" SET \"Name\" = '%1' WHERE \"NodeId\" = '%2';")
		.arg(name).arg(qPrintable(nodeId));

	return query.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateSetNodeDescriptionQuery(
	const Id& nodeId,
	const QString& description,
	const imtbase::IOperationContext* operationContextPtr)
{
	QString query = QString("UPDATE  public.\"Nodes\" SET \"Description\" = '%1' WHERE \"NodeId\" = '%2';")
						.arg(description).arg(qPrintable(nodeId));

	return query.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateSetNodeMetaInfoQuery(
	const Id& nodeId,
	const idoc::IDocumentMetaInfo* metaInfoPtr,
	const imtbase::IOperationContext* operationContextPtr)
{
	QJsonArray metaInfos = CtreateJsonFromMetaInfo(metaInfoPtr);
	QJsonDocument document;
	document.setArray(metaInfos);
	QByteArray documentContent = document.toJson(QJsonDocument::Compact);
	QString query = QString("UPDATE  public.\"Nodes\" SET \"NodeInfo\"=jsonb_set(\"NodeInfo\"::jsonb, '{metainfo}', '%1') WHERE \"NodeId\" = '%2';")
						.arg(qPrintable(documentContent)).arg(qPrintable(nodeId));

	return query.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateMoveNodeQuery(
	const Id& nodeId,
	const Id& parentNodeId,
	const imtbase::IOperationContext* operationContextPtr)
{
	QString query = QString("UPDATE  public.\"Nodes\" SET \"ParentId\"='%1' WHERE \"NodeId\" = '%2';")
						.arg(qPrintable(parentNodeId)).arg(qPrintable(nodeId));

	return query.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateRemoveNodeQuery(
	const Id& nodeId,
	const imtbase::IOperationContext* operationContextPtr)
{
	QString query = QString("DELETE FROM public.\"Nodes\" WHERE \"NodeId\" = '%1';")
						.arg(qPrintable(nodeId));

	return query.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateInsertNewObjectQuery(
	const Id& objectId,
	const Id& nodeId,
	const imtbase::IOperationContext* operationContextPtr)
{
	QString queryString = QString("INSERT INTO public.\"NodeDocuments\"(\"DocumentId\", \"NodeId\") VALUES ('%1', '%2');")
							  .arg(qPrintable(objectId))
							  .arg(qPrintable(nodeId));

	return queryString.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateMoveObjectQuery(
	const Id& objectId,
	const Id& sourceNodeId,
	const Id& targetNodeId,
	const imtbase::IOperationContext* operationContextPtr)
{
	QString query;

	if (!targetNodeId.isEmpty()){
		query = QString(
					"UPDATE  public.\"NodeDocuments\" SET \"NodeId\" = '%3' "
					"FROM (SELECT count(*) FROM \"Nodes\" WHERE \"NodeId\" = '%3') AS subquery "
					"WHERE \"DocumentId\"='%1' "
					"AND \"NodeId\" = '%2' "
					"AND  subquery.count = 1;"
					).arg(qPrintable(objectId)).arg(qPrintable(sourceNodeId)).arg(qPrintable(targetNodeId));
	}
	else{
		query = QString(
					"UPDATE  public.\"NodeDocuments\" SET \"NodeId\" = '%3' "
					"FROM (SELECT 1 AS count) AS subquery "
					"WHERE \"DocumentId\"='%1' "
					"AND \"NodeId\" = '%2' "
					"AND  subquery.count = 1;"
					).arg(qPrintable(objectId)).arg(qPrintable(sourceNodeId)).arg(qPrintable(targetNodeId));
	}


	return query.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateRemoveObjectQuery(
	const Id& objectId,
	const Id& nodeId,
	const imtbase::IOperationContext* operationContextPtr)
{
	QString query = QString("DELETE FROM public.\"NodeDocuments\" WHERE \"DocumentId\"='%1' AND \"NodeId\" = '%2';")
						.arg(qPrintable(objectId)).arg(qPrintable(nodeId));

	return query.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateGetNodeCountQuery(const iprm::IParamsSet* selectionParamsPtr) const
{
	QString filterQuery;
	if (selectionParamsPtr != nullptr){
		if (!CreateFilterQuery(*selectionParamsPtr, filterQuery)){
			return QByteArray();
		}
	}

	QString baseSelelectionQuery = "SELECT count(\"NodeId\") FROM public.\"Nodes\"";

	// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
	QString retVal = "(" + baseSelelectionQuery;
	retVal += QString(" ") + filterQuery;

	return retVal.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateGetNodeIdsQuery(
	int offset,
	int count,
	const iprm::IParamsSet* selectionParamsPtr) const
{
	QString sortQuery;
	QString filterQuery;
	if (count == 0){
		return QByteArray();
	}
	if (selectionParamsPtr != nullptr){
		if (!CreateFilterQuery(*selectionParamsPtr, filterQuery)){
			return QByteArray();
		}

		iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(selectionParamsPtr, "Filter");
		if (collectionFilterParamPtr.IsValid()){
			if (!CreateSortQuery(*collectionFilterParamPtr, sortQuery)){
				return QByteArray();
			}
		}
	}

	QByteArray paginationQuery;
	if (!CreatePaginationQuery(offset, count, paginationQuery)){
		return QByteArray();
	}

	QString baseSelelectionQuery = "SELECT * FROM public.\"Nodes\"";

	// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
	QString retVal = "(" + baseSelelectionQuery;
	retVal += QString(" ") + filterQuery;
	retVal += QString(" ") + qPrintable(paginationQuery) + ")";
	retVal += QString(" ") + sortQuery;

	return retVal.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateGetObjectIdsQuery(
	int offset,
	int count,
	const iprm::IParamsSet* selectionParamsPtr) const
{
	QString sortQuery;
	QString filterQuery;
	if (count == 0){
		return QByteArray();
	}
	if (selectionParamsPtr != nullptr){
		if (!CreateFilterQuery(*selectionParamsPtr, filterQuery)){
			return QByteArray();
		}

		iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(selectionParamsPtr, "Filter");
		if (collectionFilterParamPtr.IsValid()){
			if (!CreateSortQuery(*collectionFilterParamPtr, sortQuery)){
				return QByteArray();
			}
		}
	}

	QByteArray paginationQuery;
	if (!CreatePaginationQuery(offset, count, paginationQuery)){
		return QByteArray();
	}

	QString baseSelelectionQuery = "SELECT \"NodeId\" FROM public.\"Nodes\"";

	// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
	QString retVal = "(" + baseSelelectionQuery;
	retVal += QString(" ") + filterQuery;
	retVal += QString(" ") + qPrintable(paginationQuery) + ")";
	retVal += QString(" ") + sortQuery;

	return retVal.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateGetNodeInfoQuery(const Id& nodeId) const
{
	QString query = QString("SELECT \"NodeId\", \"Name\", \"Description\" FROM public.\"Nodes\";")
						.arg(qPrintable(nodeId));

	return query.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateGetNodePathQuery(const Id& nodeId) const
{
	QString query = QString("SELECT * FROM get_node_path('%1');")
						.arg(qPrintable(nodeId));

	return query.toUtf8();
}


QByteArray CSqlStructureDelegateComp::CreateGetObjectParentNodeIdsQuery(const Id& objectId) const
{
	QString query = QString("SELECT \"NodeId\" WHERE \"DocumentId\"=\"%1\";")
						.arg(qPrintable(objectId));

	return query.toUtf8();
}


QJsonArray CSqlStructureDelegateComp::CtreateJsonFromMetaInfo(const idoc::IDocumentMetaInfo* metaInfoPtr)
{
	QJsonArray metaInfoArray;
	if (metaInfoPtr != nullptr){
		QSet<int> metaInfoTypes = metaInfoPtr->GetMetaInfoTypes();
		for (int type = 0; type < metaInfoTypes.count(); type++){
			QVariant metaInfo = metaInfoPtr->GetMetaInfo(type);
			QJsonObject metaInfoObject;
			metaInfoObject.insert("type", type);
			metaInfoObject.insert("typeName", metaInfo.typeName());
		#if QT_VERSION >= 0x060000
			if (metaInfo.typeId() == QMetaType::QDateTime || metaInfo.typeId() == QMetaType::QDate){
		#else
			if (metaInfo.type() == QMetaType::QDateTime || metaInfo.type() == QMetaType::QDate){
		#endif
				metaInfoObject.insert("value", metaInfo.toDateTime().toString(Qt::ISODate));
			}
			else{
				metaInfoObject.insert("value", metaInfo.toString());
			}
			metaInfoArray.append(metaInfoObject);
		}
	}

	return 	metaInfoArray;
}



bool CSqlStructureDelegateComp::CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const
{
	paginationQuery.clear();

	if (offset >= 0 && count > 0){
		paginationQuery = QString("OFFSET %1 ROWS FETCH NEXT %2 ROWS ONLY").arg(offset).arg(count).toUtf8();
	}

	return true;
}


bool CSqlStructureDelegateComp::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	bool retVal = true;

	QString objectFilterQuery;
	retVal = CreateObjectFilterQuery(filterParams, objectFilterQuery);
	if (!retVal){
		return false;
	}

	QString textFilterQuery;
	iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
	if (collectionFilterParamPtr.IsValid()){
		retVal = CreateTextFilterQuery(*collectionFilterParamPtr, textFilterQuery);
		if (!retVal){
			return false;
		}
	}

	if (!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty()){
		filterQuery = " WHERE ";
	}

	filterQuery += objectFilterQuery;
	if (!objectFilterQuery.isEmpty() && !textFilterQuery.isEmpty()){
		filterQuery += " AND ";
	}

	if (!textFilterQuery.isEmpty()){
		filterQuery += "(" + textFilterQuery + ")";
	}

	return true;
}


bool CSqlStructureDelegateComp::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{
	QString objectFilterQuery;
	iprm::TParamsPtr<iprm::IParamsSet> objectFilterParamPtr(&filterParams, "ObjectFilter");
	if (objectFilterParamPtr.IsValid()){
		iprm::IParamsSet::Ids paramIds = objectFilterParamPtr->GetParamIds();
		if (!paramIds.isEmpty()){
#if QT_VERSION < 0x060000
			QByteArrayList idsList(paramIds.toList());
#else
			QByteArrayList idsList(paramIds.cbegin(), paramIds.cend());
#endif
			QByteArray key = idsList[0];

			iprm::TParamsPtr<iprm::ITextParam> textParamPtr(objectFilterParamPtr.GetPtr(), key);
			if (!textParamPtr.IsValid()){
				return false;
			}

			QString value = textParamPtr->GetText();

			filterQuery = QString("%1 = '%2'").arg(qPrintable(key)).arg(value);
		}
	}

	return true;
}


bool CSqlStructureDelegateComp::CreateTextFilterQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){
		return true;
	}

	QString textFilter = collectionFilter.GetTextFilter();
	if (!textFilter.isEmpty()){
		textFilterQuery = QString("\"%1\" ILIKE '%%2%'").arg(qPrintable(filteringColumnIds.first())).arg(textFilter);

		for (int i = 1; i < filteringColumnIds.count(); ++i){
			textFilterQuery += " OR ";

			textFilterQuery += QString("\"%1\" ILIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
		}
	}

	return true;
}


bool CSqlStructureDelegateComp::CreateSortQuery(
	const imtbase::ICollectionFilter& collectionFilter,
	QString& sortQuery) const
{
	QByteArray columnId;
	QByteArray sortOrder;

	if (!collectionFilter.GetSortingInfoIds().isEmpty()){
		columnId = collectionFilter.GetSortingInfoIds().first();
	}

	switch (collectionFilter.GetSortingOrder()){
	case imtbase::ICollectionFilter::SO_ASC:
		sortOrder = "ASC";
		break;
	case imtbase::ICollectionFilter::SO_DESC:
		sortOrder = "DESC";
		break;
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
	}

	return true;
}



} // namespace imtdb


