#include <imtgql/CGqlQuery.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtgql
{


// public methods

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
	QByteArray queryData = QString(" { data { %1 } } }" + QByteArray("\"") + " }").arg(CreateFieldQueryPart()).toUtf8();

	return QByteArray("{") + QByteArray("\"query\"") + ":" + "\"query " + m_commandId + " { " + m_commandId + CreateQueryParams().toUtf8() + queryData;
}


// reimplemented (istd::IChangeable)

int CGqlQuery::GetSupportedOperations() const
{
	return BaseClass::GetSupportedOperations() | SO_CLONE;
}


istd::IChangeable* CGqlQuery::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CGqlQuery> clonePtr(new CGqlQuery);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
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


QString CGqlQuery::CreateQueryParams() const
{
	if (m_paramsMap.isEmpty()){
		return QString();
	}

	QString queryParamsString;

	queryParamsString += "(";

	QByteArrayList parameterIds = m_paramsMap.keys();

	for (int i = 0; i < parameterIds.count(); ++i){
		QByteArray parameterId = parameterIds[i];
		queryParamsString += parameterId;
		queryParamsString += ": ";
		queryParamsString += "{";

		Arguments arguments = m_paramsMap[parameterId].arguments;
		for (int argIndex = 0; argIndex < arguments.count(); ++argIndex){
			queryParamsString += arguments[argIndex].key;
			queryParamsString += ": ";
			queryParamsString += "\\\"" + arguments[argIndex].value.toString() + "\\\"";
		}

		queryParamsString += "}";

		if (i < parameterIds.count() - 1){
			queryParamsString += ", ";
		}
	}

	queryParamsString += ")";

	return queryParamsString;
}


} // namespace imtgql


