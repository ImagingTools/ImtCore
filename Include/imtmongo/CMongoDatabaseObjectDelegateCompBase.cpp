#include <imtmongo/CMongoDatabaseObjectDelegateCompBase.h>

// mongocxx includes
#include <bsoncxx/string/to_string.hpp>

// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtmongo
{


// public methods


// reimplemented (imtdb::IDatabaseObjectDelegate)

const iprm::IOptionsList* CMongoDatabaseObjectDelegateCompBase::GetObjectTypeInfos() const
{
	return m_typesCompPtr.GetPtr();
}


QByteArray CMongoDatabaseObjectDelegateCompBase::GetObjectTypeId(const QByteArray& objectId) const
{
	if (m_typesCompPtr.IsValid()){
		if (m_typesCompPtr->GetOptionsCount() == 1){

			return m_typesCompPtr->GetOptionId(0);
		}
	}

	return QByteArray();
}


bsoncxx::document::view_or_value CMongoDatabaseObjectDelegateCompBase::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	bsoncxx::document::view_or_value retVal;

	return retVal;
}


bsoncxx::document::view_or_value CMongoDatabaseObjectDelegateCompBase::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
//	if (!objectId.isEmpty()){
//		return QString("SELECT * FROM \"%1\" WHERE \"%2\" = '%3'")
//					.arg(qPrintable(*m_tableNameAttrPtr))
//					.arg(qPrintable(*m_objectIdColumnAttrPtr))
//					.arg(qPrintable(objectId))
//					.toLocal8Bit();
//	}
//	else{
//		QString sortQuery;
//		QString filterQuery;
//		if (count == 0){
//			return QByteArray();
//		}
//		if (paramsPtr != nullptr){
//			if (!CreateFilterQuery(*paramsPtr, filterQuery)){
//				return QByteArray();
//			}

//			iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(paramsPtr, "Filter");
//			if (collectionFilterParamPtr.IsValid()){
//				if (!CreateSortQuery(*collectionFilterParamPtr, sortQuery)){
//					return QByteArray();
//				}
//			}
//		}

//		QByteArray paginationQuery;
//		if (!CreatePaginationQuery(offset, count, paginationQuery)){
//			return QByteArray();
//		}

//		QString baseSelelectionQuery = GetBaseSelectionQuery();

//		// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
//		QString retVal = "(" + baseSelelectionQuery;
//		retVal += QString(" ") + filterQuery;
//		retVal += QString(" ") + qPrintable(paginationQuery) + ")";
//		retVal += QString(" ") + sortQuery;

//		return retVal.toLocal8Bit();
//	}

	return {};
}




bsoncxx::document::view_or_value CMongoDatabaseObjectDelegateCompBase::CreateResetQuery(const imtbase::IObjectCollection& /*collection*/) const
{
	bsoncxx::document::view_or_value retVal;

	return retVal;
}


bsoncxx::document::view_or_value CMongoDatabaseObjectDelegateCompBase::CreateDataMetaInfoQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/) const
{
	bsoncxx::document::view_or_value retVal;

	return retVal;
}


bsoncxx::document::view_or_value CMongoDatabaseObjectDelegateCompBase::CreateCollectionItemMetaInfoQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const idoc::IDocumentMetaInfo* /*collectionItemMetaInfoPtr*/) const
{
	bsoncxx::document::view_or_value retVal;

	return retVal;
}


// protected methods

QString CMongoDatabaseObjectDelegateCompBase::GetBaseSelectionQuery() const
{

	return "";
}


idoc::IDocumentMetaInfo* CMongoDatabaseObjectDelegateCompBase::CreateCollectionItemMetaInfo(const QByteArray& /*typeId*/) const
{

	return new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>;
}




idoc::MetaInfoPtr CMongoDatabaseObjectDelegateCompBase::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{

	return idoc::MetaInfoPtr();
}



bool CMongoDatabaseObjectDelegateCompBase::CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const
{

	return true;
}


bool CMongoDatabaseObjectDelegateCompBase::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{

	return true;
}


bool CMongoDatabaseObjectDelegateCompBase::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{

	return true;
}


bool CMongoDatabaseObjectDelegateCompBase::CreateTextFilterQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){

		return true;
	}

//	QString textFilter = collectionFilter.GetTextFilter();
//	if (!textFilter.isEmpty()){
//        textFilterQuery = QString("\"%1\" ILIKE '%%2%'").arg(qPrintable(filteringColumnIds.first())).arg(textFilter);

//		for (int i = 1; i < filteringColumnIds.count(); ++i){
//			textFilterQuery += " OR ";

//            textFilterQuery += QString("\"%1\" ILIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
//		}
//	}

	return true;
}


bool CMongoDatabaseObjectDelegateCompBase::CreateSortQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& sortQuery) const
{

	return true;
}


// reimplemented (imtdb::IDatabaseObjectDelegate)

istd::IChangeable* CMongoDatabaseObjectDelegateCompBase::CreateObjectFromRecord(const bsoncxx::document::view& doc) const
{
	if (!m_databaseEngineCompPtr.IsValid()){

		return nullptr;
	}

	return nullptr;
}


QByteArray CMongoDatabaseObjectDelegateCompBase::GetObjectIdFromRecord(const bsoncxx::document::view& doc) const
{
	bsoncxx::document::element elem = doc["_id"];
	if (elem.type() == bsoncxx::type::k_oid) {
		const char* oid = elem.get_oid().value.bytes();
		QByteArray id(oid);

		return id;
	}

	return QByteArray();
}


bool CMongoDatabaseObjectDelegateCompBase::CreateObjectInfoFromRecord(
			const bsoncxx::document::view& doc,
			idoc::MetaInfoPtr& objectMetaInfoPtr,
			idoc::MetaInfoPtr& collectionItemMetaInfoPtr) const
{

	return false;
}


QVariant CMongoDatabaseObjectDelegateCompBase::GetElementInfoFromRecord(const bsoncxx::document::view& doc, const QString& infoId) const
{
	bsoncxx::document::element elem = doc[infoId.toStdString()];
	bsoncxx::type type = elem.type();
	//swith по типам
	if(type == bsoncxx::type::k_oid){
		const char* oid = elem.get_oid().value.bytes();
		return QByteArray(oid);
	}
	if(type == bsoncxx::type::k_bool){
		const bool val = elem.get_bool().value;
		return val;
	}
	if(type == bsoncxx::type::k_int32){
		const int val = elem.get_int32().value;
		return val;
	}
	if(type == bsoncxx::type::k_int64){
		const int64_t val = elem.get_int64().value;
		return val;
	}
	if(type == bsoncxx::type::k_double){
		const double val = elem.get_double().value;
		return val;
	}
	if(type == bsoncxx::type::k_string){
		QString val = QString::fromStdString(bsoncxx::string::to_string(elem.get_string().value));
		return val;
	}

	return QVariant();
}


} // namespace imtmongo


