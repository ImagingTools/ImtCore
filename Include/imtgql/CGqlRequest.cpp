#include <imtgql/CGqlRequest.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtgql/CGqlContext.h>


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


void CGqlRequest::AddParam(const QByteArray &paramId, const CGqlObject &param)
{
	m_params.InsertField(paramId, param);
}


void CGqlRequest::AddField(const QByteArray &fieldId, const CGqlObject &field)
{
	m_fields.InsertField(fieldId, field);
}


void CGqlRequest::AddSimpleField(const QByteArray &fieldId)
{
	m_fields.InsertField(fieldId);
}


void CGqlRequest::SetGqlContext(const IGqlContext* gqlContext)
{
	if (m_gqlContextPtr.get() != gqlContext){
		istd::CChangeNotifier changeNotifier(this);

		m_gqlContextPtr.reset(const_cast<IGqlContext*>(gqlContext));
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


QByteArray CGqlRequest::GetHeader(QByteArray headerId) const
{
	const IGqlContext* gqlContext = GetRequestContext();
	if (gqlContext != nullptr){
		IGqlContext::Headers headers = gqlContext->GetHeaders();
		return headers.value(headerId);
	}

	return QByteArray();
}


const CGqlObject& CGqlRequest::GetFields() const
{
	return m_fields;
}


const CGqlObject& CGqlRequest::GetParams() const
{
	return m_params;
}

void CGqlRequest::SetParams(const CGqlObject& params)
{
	m_params = params;
}

const CGqlObject* CGqlRequest::GetFieldObject(const QByteArray& fieldId) const
{
	return m_fields.GetFieldArgumentObjectPtr(fieldId);
}


const CGqlObject* CGqlRequest::GetParamObject(const QByteArray& paramId) const
{
	return m_params.GetFieldArgumentObjectPtr(paramId);
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

	QByteArray queryData = "{\"query\": \"" + type + " " + m_commandId + " {" + m_commandId + params + fields + "}\"}";

	return queryData;
}


const IGqlContext* CGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr.get();
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
	// QByteArray key;
	// QByteArray value;
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
	m_commandId = m_commandId.replace("\n","");
	m_commandId = m_commandId.replace("\t","");
	body = body.remove(0,index);
	m_activeGqlObjectPtr = nullptr;
	m_startFields = false;
	m_startArrayPrimitives = false;
	m_textString = false;

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
				m_textString = false;
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
				m_startArrayPrimitives = false;
				if (m_startParams && endParams && !m_startFields){
					m_startFields = true;
					// m_activeGqlObjectPtr = nullptr;
					m_activeGqlObjectPtr = &m_fields;
				}
			}
			break;

		case '}':
			if (startBigText){
				text.append(chr);
			}
			else{
				if (!text.isEmpty()) {
					SetParseText(text);
					text.clear();
					startText = false;
				}
				if (m_activeGqlObjectPtr != nullptr){
					m_activeGqlObjectPtr = m_activeGqlObjectPtr->GetParentObject();
				}
				if (m_activeGqlObjectPtr == nullptr){
					if (m_startFields == true){
						if (!text.isEmpty()){
							SetParseObject(text);
						}
						return true;
					}
					else{
						errorPosition = i + index;
						return false;
					}
				}
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
					StartArray(text);
					text.clear();
				}
				m_startArrayPrimitives = true;
				startText = false;
				m_startKey = true;
			}
			break;
		
		case ']':
			if (startBigText){
				text.append(chr);
			}
			else{
				if (!text.isEmpty()) {
					SetParseText(text);
					text.clear();
				}
				if (!CloseArray()){
					errorPosition = i + index;

					return false;
				}
				m_startArrayPrimitives = false;
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
				m_textString = true;
			}
			else {
				SetParseText(text);
				text.clear();
				startText = false;
				startBigText = false;
				m_textString = false;
			}
			if (!m_startFields){
				m_startKey = false;
			}
			break;

		case '\n': case '\r': case '\t':
			if (startBigText){
				text.append(chr);
			}
			if (!text.isEmpty() && m_startFields){
				SetParseText(text);
				text.clear();
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
	m_params.ResetData();
	m_fields.ResetData();

	m_gqlContextPtr.reset();

	return true;
}


// protected methods

QByteArray CGqlRequest::CreateQueryFields() const
{
	QByteArray retVal;
	retVal = AddObjectFieldPart(m_fields);
	if (retVal.isEmpty()){
		retVal = "{}";
	}

	return retVal;
}


QByteArray CGqlRequest::CreateQueryParams() const
{
	QByteArray retVal;
	retVal = AddObjectParamPart(m_params);

	return retVal;
}


QByteArray CGqlRequest::AddObjectFieldPart(const CGqlObject &gqlObject) const
{
	QByteArray retVal;

	QByteArrayList fieldIds = gqlObject.GetFieldIds();
	if (!fieldIds.isEmpty()){
		retVal += " {";

		for (int i = 0; i < fieldIds.count(); ++i){
			const QByteArray& fieldId = fieldIds[i];
			retVal += fieldId;
			if (gqlObject.IsObject(fieldId)){
				retVal += AddObjectFieldPart(*gqlObject.GetFieldArgumentObjectPtr(fieldId));
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


	QByteArrayList fieldIds = gqlObject.GetFieldIds();
	for (int i = 0; i < fieldIds.count(); ++i) {
		const QByteArray& fieldId = fieldIds[i];

		if (gqlObject.IsObject(fieldId)) {
			retVal += fieldId;
			retVal += ": {";
			retVal += AddObjectParamPart(*gqlObject.GetFieldArgumentObjectPtr(fieldId));
			retVal += "}";
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


	return retVal;
}


QByteArray CGqlRequest::AddObjectParamValue(const QVariant &value) const
{
	QByteArray retVal;
#if QT_VERSION >= 0x060000
	int valueType = value.typeId();
#else
	int valueType = value.type();
#endif
	if (valueType == QMetaType::QVariantList || valueType == QMetaType::QStringList){
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
	else if (valueType == QMetaType::QString || valueType == QMetaType::QByteArray){
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
	for (const QString& key: keys){
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
	for (const QString& key : keys){
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
		// CGqlObject gqlObject;
		// if (m_activeGqlObjectPtr == nullptr || m_activeGqlObjectPtr->GetParentObject() == nullptr){
		// 	m_fields.InsertField(commandId, gqlObject);
		// 	m_activeGqlObjectPtr = const_cast<CGqlObject*>(m_fields.GetFieldArgumentObjectPtr(commandId));
		// }
		// else{
		// 	m_activeGqlObjectPtr = m_activeGqlObjectPtr->CreateFieldObject(commandId);
		// }
		if (m_activeGqlObjectPtr != nullptr){
			m_activeGqlObjectPtr = m_activeGqlObjectPtr->CreateFieldObject(commandId);
		}
	}
	else if (m_startParams){
		CGqlObject newObject;
		if (m_activeGqlObjectPtr == nullptr){
			m_params.InsertField(commandId, newObject);
			m_activeGqlObjectPtr = const_cast<CGqlObject*>(m_params.GetFieldArgumentObjectPtr(commandId));
		}
		else{
			QByteArray lastArrayId;
			if (!m_activeArrayIds.isEmpty()){
				lastArrayId = m_activeArrayIds.last();
			}
			if (!lastArrayId.isEmpty() && !m_objectArrayList.isEmpty() && m_objectArrayList.last() == m_activeGqlObjectPtr){
				m_activeGqlObjectPtr = m_activeGqlObjectPtr->AppendFieldToArray(lastArrayId, newObject);
				m_activeGqlObjectPtr = m_activeGqlObjectPtr->CreateFieldObject(commandId);
				m_currentField = commandId;
			}
			else {
				m_activeGqlObjectPtr = m_activeGqlObjectPtr->CreateFieldObject(commandId);
			}
		}
	}

}


void CGqlRequest::SetParseText(const QByteArray &text)
{
	if (m_activeGqlObjectPtr == nullptr){
		SetParseObject(text);

		return;
	}
	QByteArray lastArrayId;
	if (!m_activeArrayIds.isEmpty()){
		lastArrayId = m_activeArrayIds.last();
	}
	if (!lastArrayId.isEmpty() && m_startArrayPrimitives){

		QVariant value = m_activeGqlObjectPtr->GetFieldArgumentValue(lastArrayId);
		QVariantList variantList;
		if (!value.isNull()){
			variantList = value.toList();
		}
		if (m_textString){
			QString stringValue = text;
			variantList.append(stringValue);
		}
		else if (text == "true"){
			variantList.append(QVariant(true));
		}
		else if (text == "false"){
			variantList.append(QVariant(false));
		}
		else {
			bool ok;
			if (text.contains('.')){
				double doubleValue = text.toDouble(&ok);
				variantList.append(doubleValue);
			}
			else{
				int intValue = text.toInt(&ok);
				variantList.append(intValue);
			}
		}
		m_activeGqlObjectPtr->InsertField(lastArrayId, variantList);

		return;
	}
	else if (!lastArrayId.isEmpty() &&  !m_objectArrayList.isEmpty() && m_objectArrayList.last() == m_activeGqlObjectPtr){
		CGqlObject newObject;
		newObject.InsertField(text);
		m_activeGqlObjectPtr = m_activeGqlObjectPtr->AppendFieldToArray(lastArrayId, newObject);
		m_currentField = text;
	}
	else if (m_startKey){
		m_activeGqlObjectPtr->InsertField(text);
		m_currentField = text;
	}
	else{
		QVariant value;
		if (m_textString){
			QString stringValue = text;
			value = stringValue;
		}
		else if (text == "true"){
			value = true;
		}
		else if (text == "false"){
			value = false;
		}
		else {
			bool ok;
			if (text.contains('.')){
				double doubleValue = text.toDouble(&ok);
				value = doubleValue;
			}
			else{
				int intValue = text.toInt(&ok);
				if (QString::number(intValue) == text){
					value = intValue;
				}
				else {
					value = text;
				}

			}
		}
		m_activeGqlObjectPtr->InsertField(m_currentField, value);
	}

}


void CGqlRequest::StartArray(const QByteArray& text)
{
	if (m_activeGqlObjectPtr != nullptr && m_startKey){
		QByteArray lastArrayId;
		if (!m_activeArrayIds.isEmpty()){
			lastArrayId = m_activeArrayIds.last();
		}
		if (!lastArrayId.isEmpty() && !m_startArrayPrimitives && m_activeGqlObjectPtr->IsObjectList(lastArrayId)){
			CGqlObject newObject;
			m_activeGqlObjectPtr = m_activeGqlObjectPtr->AppendFieldToArray(lastArrayId, newObject);
		}
		QList<CGqlObject> objectList;
		m_activeGqlObjectPtr->InsertField(text, objectList);
		m_currentField = text;
		m_activeArrayIds.append(text);
		m_objectArrayList.append(m_activeGqlObjectPtr);
	}
}


bool CGqlRequest::CloseArray()
{
	bool retVal = false;

	if (!m_activeArrayIds.isEmpty()){
		m_activeArrayIds.removeLast();
		m_objectArrayList.removeLast();
		retVal = true;
	}

	return retVal;
}


} // namespace imtgql


