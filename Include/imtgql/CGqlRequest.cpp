#include <imtgql/CGqlRequest.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtgql
{


// public methods

imtgql::CGqlRequest::CGqlRequest(RequestType requestType, const QByteArray& commandId)
	:m_commandId(commandId),
	m_requestType(requestType)
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


const QList<CGqlObject> *CGqlRequest::GetFields() const
{
	return &m_fields;
}


const QList<CGqlObject> *CGqlRequest::GetParams() const
{
	return &m_params;
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

	QByteArray type = "";
	switch (m_requestType)
	{
	case imtgql::IGqlRequest::RT_QUERY:
		type = "query";
		break;
	case imtgql::IGqlRequest::RT_MUTATION:
		type = "mutation";
		break;
	case imtgql::IGqlRequest::RT_SUBSCRIPTION:
		type = "subscription";
		break;
	default:
		Q_ASSERT(false);
		return QByteArray();
	}

	if (!params.isEmpty()) {
		params.prepend("(");
		params.append(")");
	}

	QByteArray queryData = "{\"query\": \"" + type + " " + m_commandId + " {" + m_commandId + params + " {" + fields + "}" + "}\"}";

	return queryData;
}



bool CGqlRequest::ParseQuery(const QByteArray &query, int& errorPosition)
{
	QJsonDocument document = QJsonDocument::fromJson(query);
	QByteArray body = document.object().value("query").toString().toUtf8();
	int index = body.indexOf("{");
	QByteArray header = body.left(index);
	QList<QByteArray> headersData = header.split(' ');
	QByteArray type;
	for (int i = 0; i < headersData.count(); ++i){
		if (!headersData[i].isEmpty()){
			type = headersData[i];
			break;
		}
	}
	if (type == "query"){
		m_requestType = imtgql::IGqlRequest::RT_QUERY;
	}
	else if (type == "mutation"){
		m_requestType = imtgql::IGqlRequest::RT_MUTATION;
	}
	else if (type == "subscription"){
		m_requestType = imtgql::IGqlRequest::RT_SUBSCRIPTION;
	}
	else{
		return false;
	}


	body = body.mid(index + 1);
	index = body.lastIndexOf("}");
	body = body.left(index);

	index = body.indexOf("(");
	m_commandId = body.left(index);
	body = body.remove(0,index);
	bool endParams = false;
	bool startText = false;
	bool startBigText = false;
	bool startBackSlash = false;
	QByteArray key;
	QByteArray value;
	QByteArray text;

	m_activeGqkObjectPtr = nullptr;
	m_startFields = false;
	m_startParams = false;


	for (int i = 0; i < body.length() ; ++i){
		char chr = body[i];
		switch (chr){
		case '(':
			if (startBigText){
				text.append(chr);
			}
			else{
				m_startParams = true;
				m_startKey = true;
			}
			break;

		case ')':
			if (startBigText){
				text.append(chr);
			}
			else{
				if (!text.isEmpty()) {
					SetParseText(text);
					text.clear();
					startText = false;
				}
				endParams = true;
			}
			break;

		case ' ':
			if (startBigText){
				text.append(chr);
			}
			else{
				startText = false;
			}
			break;

		case '{':
			if (startBigText){
				text.append(chr);
			}
			else{
				if (!text.isEmpty()) {
					SetParseObject(text);
					text.clear();
					startText = false;
				}
				m_startKey = true;
				if (m_startParams && endParams && !m_startFields){
					m_startFields = true;
				}
			}
			break;

		case '}':
			if (startBigText){
				text.append(chr);
			}
			else{
				if (m_activeGqkObjectPtr == nullptr){
					if (m_startFields == true){
						return true;
					}
					else{
						errorPosition = i + index;
						return false;
					}
				}
				if (!text.isEmpty()) {
					SetParseText(text);
					text.clear();
					startText = false;
				}
				m_activeGqkObjectPtr = m_activeGqkObjectPtr->GetParentObject();
			}
			break;

		case ',':
			if (startBigText){
				text.append(chr);
			}
			else{
				if (!text.isEmpty()) {
					SetParseText(text);
					text.clear();
					startText = false;
				}
				if (m_startParams){
					m_startKey = true;
					m_startValue = false;
				}
				else{
					errorPosition = i + index;
					return false;
				}
			}
			break;

		case ':':
			if (startBigText){
				text.append(chr);
			}
			else{
				startText = false;
				if (m_startParams){
					m_startValue = true;
				}
				else{
					errorPosition = i + index;
					return false;
				}
			}
			break;

		case '\\':
			if (startBigText){
				text.append(chr);
				startBackSlash = true;
			}
			else{
				errorPosition = i + index;
				return false;
			}
			break;

		case '"':
			if (startBackSlash){
				text.append(chr);
				startBackSlash = false;
				break;
			}

			if(!startText) {
				if (!text.isEmpty()) {
					SetParseText(text);
					text.clear();
				}
				startText = true;
				startBigText = true;
			}
			else {
				SetParseText(text);
				text.clear();
				startText = false;
				startBigText = false;
			}
			m_startKey = false;
			break;

		default:
			if (startText == false && !text.isEmpty()){
				SetParseText(text);
				text.clear();
				m_startKey = false;
			}
			text.append(chr);
			startText = true;
			break;
		}
	}


	return false;
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
	return SO_COPY | SO_RESET | SO_CLONE;
}


bool CGqlRequest::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CGqlRequest* sourcePtr = dynamic_cast<const CGqlRequest*>(&object);
	if (sourcePtr != nullptr){
		if (m_requestType == sourcePtr->GetRequestType()){
			istd::CChangeNotifier changeNotifier(this);

			m_commandId = sourcePtr->m_commandId;
			m_params = sourcePtr->m_params;
			m_fields = sourcePtr->m_fields;

			return true;
		}
	}

	return false;
}


istd::IChangeable* CGqlRequest::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::TDelPtr<CGqlRequest> clonePtr(new CGqlRequest(m_requestType));
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

	QByteArrayList fieldIds = gqlObject.GetFieldIds();
	if (!fieldIds.isEmpty()){
		retVal += " {";

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
	}

	return retVal;
}


QByteArray CGqlRequest::AddObjectParamPart(const CGqlObject &gqlObject) const
{
	QByteArray retVal;
	QByteArray objectId = gqlObject.GetId();

	if (objectId.isEmpty() == false) {
		retVal += objectId;
		retVal += ": {";
	}

	QByteArrayList fieldIds = gqlObject.GetFieldIds();
	for (int i = 0; i < fieldIds.count(); ++i) {
		const QByteArray& fieldId = fieldIds[i];

		if (gqlObject.IsObject(fieldId)) {
			retVal += AddObjectParamPart(*gqlObject.GetFieldArgumentObjectPtr(fieldId));
		}
		else {
			retVal += fieldId;
			retVal += ": ";
			QVariant value = gqlObject.GetFieldArgumentValue(fieldId);
			int valueType = value.type();
			bool isString = (valueType == QVariant::String) || (valueType == QVariant::ByteArray);
			if (isString) {
				retVal += "\\\"";
			}
			retVal += value.toString();
			if (isString) {
				retVal += "\\\"";
			}

		}
		if (i < fieldIds.count() - 1) {
			retVal += ", ";
		}
	}

	if (objectId.isEmpty() == false) {
		retVal += "}";
	}

	return retVal;
}


void CGqlRequest::ParceObjectFieldPart(CGqlObject &gqlObject, const QJsonObject &object) const
{
	QStringList keys = object.keys();
	for (QString key: keys){
		if (object.value(key).isObject()){
			CGqlObject* slaveGqlObject = new CGqlObject(key.toUtf8());
			ParceObjectFieldPart(*slaveGqlObject, object.value(key).toObject());
			gqlObject.InsertFieldObject(slaveGqlObject);
		}
		else{
			gqlObject.InsertField(key.toUtf8());
		}
	}
}


void CGqlRequest::ParceObjectParamPart(CGqlObject &gqlObject, const QJsonObject &object) const
{
	QStringList keys = object.keys();
	for (QString key: keys){
		if (object.value(key).isObject()){
			CGqlObject* slaveGqlObject = new CGqlObject(key.toUtf8());
			ParceObjectParamPart(*slaveGqlObject, object.value(key).toObject());
			gqlObject.InsertFieldObject(slaveGqlObject);
		}
		else{
			gqlObject.InsertField(key.toUtf8());
			gqlObject.InsertFieldArgument(key.toUtf8(), object.value(key).toString().toUtf8());
		}
	}
}


void CGqlRequest::SetParseObject(const QByteArray &commandId)
{
	if (m_startFields){
		CGqlObject gqlObject(commandId);
		if (m_activeGqkObjectPtr == nullptr || m_activeGqkObjectPtr->GetParentObject() == nullptr){
			m_fields.append(gqlObject);
			m_activeGqkObjectPtr = &m_fields[m_fields.count() - 1];
		}
		else{
			CGqlObject* slaveGqlObject = new CGqlObject(commandId);
			m_activeGqkObjectPtr->InsertFieldObject(slaveGqlObject);
			m_activeGqkObjectPtr = slaveGqlObject;
		}
	}
	else if (m_startParams){
		CGqlObject gqlObject(commandId);
		if (m_activeGqkObjectPtr == nullptr || m_activeGqkObjectPtr->GetParentObject() == nullptr){
			m_params.append(gqlObject);
			m_activeGqkObjectPtr = &m_params[m_params.count() - 1];
		}
		else{
			CGqlObject* slaveGqlObject = new CGqlObject(commandId);
			m_activeGqkObjectPtr->InsertFieldObject(slaveGqlObject);
			m_activeGqkObjectPtr = slaveGqlObject;
		}
	}

}


void CGqlRequest::SetParseText(const QByteArray &text)
{
	if (m_activeGqkObjectPtr == nullptr){
		SetParseObject("");
	}
	if (m_startKey){
		m_activeGqkObjectPtr->InsertField(text);
		m_currentField = text;
	}
	else{
		m_activeGqkObjectPtr->InsertFieldArgument(m_currentField, text);
	}

}


} // namespace imtgql


