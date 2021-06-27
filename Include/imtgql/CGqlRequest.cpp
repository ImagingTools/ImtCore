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


void CGqlRequest::AddParam(const CGqlObject &param)
{
	m_params.append(param);
}


void CGqlRequest::AddField(const CGqlObject &field)
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


// reimplemented (iser::IObject)

QByteArray CGqlRequest::GetFactoryId() const
{
	return m_commandId;
}


// reimplemented (iser::ISerializable)

bool CGqlRequest::Serialize(iser::IArchive& /*archive*/)
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
	const CGqlRequest* sourcePtr = dynamic_cast<const CGqlRequest*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_requestType = sourcePtr->m_requestType;
		m_commandId = sourcePtr->m_commandId;
		m_params = sourcePtr->m_params;
		m_fields = sourcePtr->m_fields;

		return true;
	}

	return false;
}


istd::IChangeable *CGqlRequest::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::TDelPtr<CGqlRequest> clonePtr(new CGqlRequest());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CGqlRequest::ResetData(istd::IChangeable::CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_commandId.clear();
	m_params.clear();
	m_fields.clear();

	return true;
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
		if (gqlObject.IsObject(fieldId)){
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

		if (gqlObject.IsObject(fieldId)){
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


