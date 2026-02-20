// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqliteJsonDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtcol/CDocumentCollectionFilter.h>


namespace imtdb
{


// public methods

// reimplemented (imtdb::CSqlJsonDatabaseDelegateComp)

QByteArray CSqliteJsonDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return GetObjectSelectionQuery(objectId, paramsPtr);
	}

	QString sortQuery;
	QString filterQuery;
	if (count == 0){
		return QByteArray();
	}
	if (paramsPtr != nullptr){
		if (!CreateFilterQuery(*paramsPtr, filterQuery)){
			return QByteArray();
		}

		iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(paramsPtr, "Filter");
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

	QString baseSelelectionQuery = GetBaseSelectionQuery();

	// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
	QString retVal = baseSelelectionQuery;
	retVal += QString(" ") + filterQuery;
	retVal += QString(" ") + sortQuery;
	retVal += QString(" ") + qPrintable(paginationQuery);

	return retVal.toUtf8();
}


bool CSqliteJsonDatabaseDelegateComp::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
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
	case imtbase::ICollectionFilter::SO_NO_ORDER:
		break;
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		if (columnId == "LastModified" || columnId == "Added"){
			sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
		else{
			sortQuery = QString("ORDER BY json_extract(\"Document\",'$.%1') %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
	}

	return true;
}


bool CSqliteJsonDatabaseDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (!paramIds.isEmpty()){
#if QT_VERSION < 0x060000
		QByteArrayList idsList(paramIds.toList());
#else
		QByteArrayList idsList(paramIds.cbegin(), paramIds.cend());
#endif
		for (int i = 0; i < idsList.size(); i++){
			QByteArray key = idsList[i];

			const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
			if (textParamPtr == nullptr){
				return false;
			}

			if (i > 0){
				filterQuery += " AND ";
			}

			QString value = textParamPtr->GetText();
			filterQuery += QString("json_extract(\"Document\",'$.%1') = '%2'").arg(qPrintable(key)).arg(value);
		}
	}

	return true;
}


bool CSqliteJsonDatabaseDelegateComp::CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){
		return true;
	}

	QString textFilter = collectionFilter.GetTextFilter();
	if (!textFilter.isEmpty()){
		textFilterQuery = QString("json_extract(\"Document\",'$.%1') LIKE '%%2%'").arg(qPrintable(filteringColumnIds.first())).arg(textFilter);

		for (int i = 1; i < filteringColumnIds.count(); ++i){
			textFilterQuery += " OR ";

			textFilterQuery += QString("json_extract(\"Document\",'$.%1') LIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
		}
	}

	return true;
}


bool CSqliteJsonDatabaseDelegateComp::CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const
{
	paginationQuery.clear();

	if (offset >= 0 && count > 0){
		paginationQuery = QString("LIMIT %1 OFFSET %2").arg(count).arg(offset).toUtf8();
	}

	return true;
}


bool CSqliteJsonDatabaseDelegateComp::CreateTimeFilterQuery(const imtbase::ITimeFilterParam& /*timeFilter*/, QString& /*timeFilterQuery*/, const QString& /*fieldId*/) const
{
	return true;
}


QByteArray CSqliteJsonDatabaseDelegateComp::GetObjectSelectionQuery(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr) const
{
	QString stateDocumentFilter;
	if (paramsPtr != nullptr){
		iprm::IParamsSet::Ids paramIds = paramsPtr->GetParamIds();
		if (paramIds.contains("DocumentFilter")){
			iprm::TParamsPtr<imtcol::IDocumentCollectionFilter> documentFilterParamPtr(paramsPtr, "DocumentFilter");
			if (documentFilterParamPtr.IsValid()){
				imtcol::IDocumentCollectionFilter::DocumentStates states = documentFilterParamPtr->GetDocumentStates();

				if (states.contains(imtcol::IDocumentCollectionFilter::DS_ACTIVE)){
					stateDocumentFilter += QString("\"IsActive\" = true");
				}

				if (states.contains(imtcol::IDocumentCollectionFilter::DS_INACTIVE)){
					if (!stateDocumentFilter.isEmpty()){
						stateDocumentFilter += QString(" OR ");
					}

					stateDocumentFilter += QString("\"IsActive\" = false");
				}
			}
		}
	}

	if (stateDocumentFilter.isEmpty()){
		stateDocumentFilter = QString("\"IsActive\" = true");
	}

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	return QString("SELECT * FROM %0\"%1\" WHERE (%2) AND \"%3\" = '%4' ORDER BY \"RevisionNumber\" DESC;")
		.arg(schemaPrefix)
		.arg(qPrintable(*m_tableNameAttrPtr))
		.arg(stateDocumentFilter)
		.arg(qPrintable(*m_objectIdColumnAttrPtr))
		.arg(qPrintable(objectId)).toUtf8();
}


} // namespace imtdb


