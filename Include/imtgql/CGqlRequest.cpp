#include <imtgql/CGqlRequest.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QDebug>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtgql
{


// public methods

imtgql::CGqlRequest::CGqlRequest(RequestType requestType, const QByteArray& commandId)
	:m_commandId(commandId),
	m_requestType(requestType),
	m_activeGqlObjectPtr(nullptr),
	m_gqlContextPtr(nullptr)
{
}


imtgql::CGqlRequest::~CGqlRequest()
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


void CGqlRequest::AddSimpleField(const QByteArray &fieldId)
{
	CGqlObject gqlObject(fieldId);
	AddField(gqlObject);
}


void CGqlRequest::SetGqlContext(const IGqlContext *gqlContext)
{
	if (m_gqlContextPtr != gqlContext){
		istd::CChangeNotifier changeNotifier(this);

		m_gqlContextPtr = const_cast<IGqlContext*>(gqlContext);
	}
}


void CGqlRequest::SetRequestType(RequestType requestType)
{
	m_requestType = requestType;
}


void CGqlRequest::SetCommandId(const QByteArray& commandId)
{
	m_commandId = commandId;
}


const QList<CGqlObject> CGqlRequest::GetFields() const
{
	return m_fields;
}


const QList<CGqlObject> CGqlRequest::GetParams() const
{
	return m_params;
}


const CGqlObject* CGqlRequest::GetField(const QByteArray& fieldId) const
{
	for (const CGqlObject& gqlObject : m_fields){
		QByteArray objectId = gqlObject.GetId();
		if (objectId == fieldId){
			return &gqlObject;
		}
	}

	return nullptr;
}


const CGqlObject* CGqlRequest::GetParam(const QByteArray& paramId) const
{
	for (const CGqlObject& gqlObject : m_params){
		QByteArray objectId = gqlObject.GetId();
		if (objectId == paramId){
			return &gqlObject;
		}
	}

	return nullptr;
}


void CGqlRequest::SetField(const CGqlObject& gqlObject)
{
	QByteArray paramId = gqlObject.GetId();

	bool isFound = false;
	for (int i = 0; i < m_fields.count(); i++){
		QByteArray objectId = m_fields[i].GetId();
		if (objectId == paramId){
			isFound = true;
			m_fields[i] = gqlObject;
			break;
		}
	}

	if (!isFound){
		AddField(gqlObject);
	}
}


void CGqlRequest::SetParam(const CGqlObject& gqlObject)
{
	QByteArray paramId = gqlObject.GetId();

	bool isFound = false;
	for (int i = 0; i < m_params.count(); i++){
		QByteArray objectId = m_params[i].GetId();
		if (objectId == paramId){
			isFound = true;
			m_params[i] = gqlObject;
			break;
		}
	}

	if (!isFound){
		AddParam(gqlObject);
	}
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


IGqlContext* CGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr;
}


bool CGqlRequest::ParseQuery(const QByteArray &query, int& errorPosition)
{
	QJsonDocument document = QJsonDocument::fromJson(query);
	QByteArray body = document.object().value("query").toString().toUtf8();
	int index = body.indexOf("{");
	QByteArray header = body.left(index);
	QList<QByteArray> headersData = header.split(' ');
	QByteArray type;
	bool endParams = false;
	bool startText = false;
	bool startBigText = false;
	bool startBackSlash = false;
	bool startArray = false;
	QByteArray key;
	QByteArray value;
	QByteArray text;

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
	if (index > -1){
		m_commandId = body.left(index);
		m_startParams = false;
	}
	else{
		index = body.indexOf("{");
		m_commandId = body.left(index);
		m_startParams = endParams = true;
	}
	m_commandId = m_commandId.replace(" ","");
	body = body.remove(0,index);
	m_activeGqlObjectPtr = nullptr;
	m_startFields = false;

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
				if (m_activeGqlObjectPtr == nullptr){
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
				m_activeGqlObjectPtr = m_activeGqlObjectPtr->GetParentObject();
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
				if (startBackSlash){
					text.append(chr);
					startBackSlash = false;
				}
				else{
					startBackSlash = true;
				}
			}
			else{
				errorPosition = i + index;
				return false;
			}
			break;
		
		case '[':
			if (startBigText){
				text.append(chr);
			}
			else{
				if (!text.isEmpty()) {
					SetParseText(text);
					text.clear();
				}
				startText = true;
				startBigText = true;
				startArray = true;
			}
			break;
		
		case ']':
			if (startArray){
				QString str = text;
				QStringList list = str.split(',');
				m_activeGqlObjectPtr->InsertField(m_currentField, QVariant(list));
				text.clear();
				startText = false;
				startBigText = false;
				startArray = false;
			}
			else{
				if (startBigText || startText){
					text.append(chr);
				}
			}

			break;

		case '"':
			if (startBackSlash){
				text.append(chr);
				startBackSlash = false;
				break;
			}

			if (startArray){
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
			if (!m_startFields){
				m_startKey = false;
			}
			break;

		default:
			if (startText == false && !text.isEmpty()){
				SetParseText(text);
				text.clear();
				if (!m_startFields){
					m_startKey = false;
				}
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
			m_gqlContextPtr = sourcePtr->m_gqlContextPtr;

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
	m_gqlContextPtr = nullptr;

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
		else if (gqlObject.IsObjectList(fieldId)){
			retVal += fieldId + " :[";
			int objectsCount = gqlObject.GetObjectsCount(fieldId);
			for (int objectIndex = 0; objectIndex < objectsCount; objectIndex++){
				if (objectIndex > 0){
					retVal += ",";
				}
				retVal += " {";
				const CGqlObject* paramsObject = gqlObject.GetFieldArgumentObjectPtr(fieldId, objectIndex);
				if (paramsObject != nullptr){
					retVal += AddObjectParamPart(*paramsObject);
				}
				retVal += "}";
			}
			retVal += " ]";
		}
		else {
			retVal += fieldId;
			retVal += ": ";
			QVariant value = gqlObject.GetFieldArgumentValue(fieldId);
			retVal += AddObjectParamValue(value);
		}
		if (i < fieldIds.count() - 1){
			retVal += ", ";
		}
	}

	if (objectId.isEmpty() == false){
		retVal += "}";
	}

	return retVal;
}


QByteArray CGqlRequest::AddObjectParamValue(const QVariant &value) const
{
	QByteArray retVal;
	int valueType = value.type();
	if (valueType == QVariant::List || valueType == QVariant::StringList){
		QVariantList variantList = value.toList();
		retVal += "[";
		for (int i = 0; i < variantList.count(); i++){
			if (i > 0){
				retVal += ", ";
			}
			retVal += AddObjectParamValue(variantList.value(i));
		}
		retVal += "]";
	}
	else if (valueType == QVariant::String || valueType == QVariant::ByteArray){
		retVal += "\\\"";
		QByteArray data = value.toByteArray();
		data = data.replace("\\", "\\\\\\\\");
		data = data.replace("\"", "\\\\\\\"");
		data = data.replace("\n", "\\\\\\\n");
		data = data.replace("\r", "\\\\\\\r");
		data = data.replace("\t", "\\\\\\\t");
		retVal += data;
		retVal += "\\\"";
	}
	else if (value.canConvert<QStringList>()){
		QStringList stringList = value.value<QStringList>();
		retVal += "[";
		for (int i = 0; i < stringList.count(); i++){
			if (i > 0){
				retVal += ", ";
			}
			retVal += AddObjectParamValue(stringList.value(i));
		}
		retVal += "]";
	}
	else if (value.canConvert<QList<QString>>()){
		QList<QString> stringList = value.value<QList<QString>>();
		retVal += "[";
		for (int i = 0; i < stringList.count(); i++){
			if (i > 0){
				retVal += ", ";
			}
			retVal += AddObjectParamValue(stringList.value(i));
		}
		retVal += "]";
	}
	else if (value.canConvert<imtgql::CGqlEnum>()){
		retVal += value.value<imtgql::CGqlEnum>().GetValue();
	}
	else {
		QByteArray data = value.toByteArray();
		retVal += data;
	}

	return retVal;
}


void CGqlRequest::ParceObjectFieldPart(CGqlObject &gqlObject, const QJsonObject &object) const
{
	QStringList keys = object.keys();
	for (QString key: keys){
		if (object.value(key).isObject()){
			CGqlObject* slaveGqlObjectPtr = gqlObject.CreateFieldObject(key.toUtf8());
			ParceObjectParamPart(*slaveGqlObjectPtr, object.value(key).toObject());
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
			CGqlObject* slaveGqlObjectPtr = gqlObject.CreateFieldObject(key.toUtf8());
			ParceObjectParamPart(*slaveGqlObjectPtr, object.value(key).toObject());
		}
		else{
			gqlObject.InsertField(key.toUtf8());
			QJsonValue jsonValue = object.value(key);
			if (jsonValue.isString()){
				gqlObject.InsertField(key.toUtf8(), jsonValue.toString());
			}
			else{
				gqlObject.InsertField(key.toUtf8(), jsonValue);
			}
		}
	}
}


void CGqlRequest::SetParseObject(const QByteArray &commandId)
{
	if (m_startFields){
		CGqlObject gqlObject(commandId);
		if (m_activeGqlObjectPtr == nullptr || m_activeGqlObjectPtr->GetParentObject() == nullptr){
			m_fields.append(gqlObject);
			m_activeGqlObjectPtr = &m_fields[m_fields.count() - 1];
		}
		else{
			m_activeGqlObjectPtr = m_activeGqlObjectPtr->CreateFieldObject(commandId);
		}
	}
	else if (m_startParams){
		CGqlObject gqlObject(commandId);
		if (m_activeGqlObjectPtr == nullptr/* || m_activeGqlObjectPtr->GetParentObject() == nullptr*/){
			m_params.append(gqlObject);
			m_activeGqlObjectPtr = &m_params[m_params.count() - 1];
		}
		else{
			m_activeGqlObjectPtr = m_activeGqlObjectPtr->CreateFieldObject(commandId);
		}
	}

}


void CGqlRequest::SetParseText(const QByteArray &text)
{
	if (m_activeGqlObjectPtr == nullptr){
		SetParseObject("");
	}
	if (m_startKey){
		m_activeGqlObjectPtr->InsertField(text);
		m_currentField = text;
	}
	else{
		m_activeGqlObjectPtr->InsertField(m_currentField, QVariant(text));
	}

}


} // namespace imtgql


