#include <imtgql/CGqlQuery.h>


namespace imtgql
{


imtgql::CGqlQuery::CGqlQuery(const QByteArray& commandId)
	:BaseClass(commandId)
{
}


// reimplemented (IGqlRequest)

imtgql::IGqlRequest::RequestType CGqlQuery::GetRequestType() const
{
	return RT_QUERY;
}


QByteArray CGqlQuery::GetQuery() const
{
	return "query " + m_commandId + " { " + m_commandId + QString(" { data { %1 } } }").arg(CreateFieldQueryPart()).toUtf8();
}


// protected methods

QString CGqlQuery::CreateFieldQueryPart() const
{
	QString fields;

	QByteArrayList fieldList = GetFields();

	for (int i = 0; i < fieldList.count(); ++i){
		fields += fieldList[i];

		if (i < fieldList.count() - 1){
			fields += ", ";
		}
	}

	return fields;
}


} // namespace imtgql


