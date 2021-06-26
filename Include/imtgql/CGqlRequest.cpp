#include <imtgql/CGqlRequest.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtgql
{


// public methods

imtgql::CGqlRequest::CGqlRequest(RequestType requestType, const QByteArray& commandId)
	:m_commandId(commandId), m_requestType(requestType)
{
}

void CGqlRequest::addParam(const CGqlObject &param)
{
	m_params.append(param);
}

void CGqlRequest::addField(const CGqlObject &field)
{
	m_fields.append(field);
}


QByteArray CGqlRequest::GetCommandId() const
{
	return m_commandId;
}


// reimplemented (IGqlRequest)

imtgql::IGqlRequest::RequestType CGqlRequest::GetRequestType() const
{
	return m_requestType;
}


QByteArray CGqlRequest::GetQuery() const
{
	QByteArray fields = CreateQueryFields();
	QByteArray params = CreateQueryParams();
	QByteArray type = "query";
	if (m_requestType == RT_MUTATION){
		type = "mutation";
	}
	if (!params.isEmpty()){
		params.prepend("(");
		params.append(")");
	}

	QByteArray queryData = "{\"query\": \"" + type + ": " + m_commandId + " { " + m_commandId + params + " {" + fields + " }" + "}\"}";
	return queryData;
}

QByteArrayList CGqlRequest::GetFields() const
{
	return QByteArrayList();
}

QByteArrayList CGqlRequest::GetFieldArguments(const QByteArray &fieldId) const
{
	return QByteArrayList();
}


// reimplemented (iser::IObject)

QByteArray CGqlRequest::GetFactoryId() const
{
	return m_commandId;
}


// reimplemented (iser::ISerializable)

bool CGqlRequest::Serialize(iser::IArchive &archive)
{
	return false;
}


// reimplemented (istd::IChangeable)

int CGqlRequest::GetSupportedOperations() const
{
	return SO_COPY | SO_RESET;
}


bool CGqlRequest::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CGqlRequestBase* sourcePtr = dynamic_cast<const CGqlRequestBase*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		return true;
	}

	return false;
}


bool CGqlRequest::ResetData(istd::IChangeable::CompatibilityMode mode)
{
	return false;
}


// protected methods

QByteArray CGqlRequest::CreateQueryFields() const
{
	QByteArray retVal;

	for (int i = 0; i < m_fields.count(); ++i){
		const CGqlObject& object = m_fields[i];
		retVal += AddObjectFieldPart(object);

		if (i < m_fields.count() - 1){
			retVal += " ";
		}
	}

	return retVal;
}


QByteArray CGqlRequest::CreateQueryParams() const
{
	QByteArray retVal;

	for (int i = 0; i < m_params.count(); ++i){
		const CGqlObject& object = m_params[i];
		retVal += AddObjectParamPart(object);

		if (i < m_fields.count() - 1){
			retVal += ", ";
		}
	}

	return retVal;
}


QByteArray CGqlRequest::AddObjectFieldPart(const CGqlObject &gqlObject) const
{
	QByteArray retVal;
	retVal += gqlObject.GetId();

	retVal += " {";
	QByteArrayList fieldIds = gqlObject.GetFieldIds();
	for (int i = 0; i < fieldIds.count(); ++i){
		const QByteArray& fieldId = fieldIds[i];
		if (gqlObject.isObject(fieldId)){
			retVal += AddObjectFieldPart(*gqlObject.GetFieldArgumentObjectPtr(fieldId));
		}
		else{
			retVal += fieldId;
		}
		if (i < fieldIds.count() - 1){
			retVal += " ";
		}
	}

	retVal += "}";

	return retVal;
}


QByteArray CGqlRequest::AddObjectParamPart(const CGqlObject &gqlObject) const
{
	QByteArray retVal;
	retVal += gqlObject.GetId();

	retVal += ": {";
	QByteArrayList fieldIds = gqlObject.GetFieldIds();
	for (int i = 0; i < fieldIds.count(); ++i){
		const QByteArray& fieldId = fieldIds[i];

		if (gqlObject.isObject(fieldId)){
			retVal += AddObjectParamPart(*gqlObject.GetFieldArgumentObjectPtr(fieldId));
		}
		else{
			retVal += fieldId;
			retVal += ": ";
			QVariant value = gqlObject.GetFieldArgumentValue(fieldId);
			if (value.type() == QVariant::Int){
				retVal += QByteArray::number(value.toInt());
			}
			else{
				retVal += "\\\"";
				retVal += value.toByteArray();
				retVal += "\\\"";
			}

		}
		if (i < fieldIds.count() - 1){
			retVal += ", ";
		}
	}

	retVal += "}";

	return retVal;
}


} // namespace imtgql


