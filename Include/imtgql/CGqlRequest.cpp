// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgql/CGqlRequest.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlFieldFragment.h>


namespace imtgql
{


const QHash<QByteArray, IGqlRequest::RequestType> CGqlRequest::s_requestNameMap = {
	{ QByteArrayLiteral("query"), IGqlRequest::RT_QUERY },
	{ QByteArrayLiteral("mutation"), IGqlRequest::RT_MUTATION },
	{ QByteArrayLiteral("subscription"), IGqlRequest::RT_SUBSCRIPTION },
};


// public methods

CGqlRequest::CGqlRequest(RequestType requestType, const QByteArray& commandId)
	:m_commandId(commandId),
	m_requestType(requestType),
	m_startKey(false),
	m_startValue(false),
	m_startParams(false),
	m_startFields(false),
	m_startFragment(false),
	m_startArrayPrimitives(false),
	m_textString(false),
	m_activeGqlObjectPtr(nullptr),
	m_activeFieldObjectPtr(nullptr),
	m_gqlContextPtr(nullptr)
{
}


CGqlRequest::~CGqlRequest()
{
}


void CGqlRequest::AddParam(const QByteArray& paramId, const CGqlParamObject& param)
{
	m_params.InsertParam(paramId, param);
}


void CGqlRequest::AddField(const QByteArray& fieldId, const CGqlFieldObject& field)
{
	m_fields.InsertField(fieldId, field);
}


void CGqlRequest::AddSimpleField(const QByteArray &fieldId)
{
	m_fields.InsertField(fieldId);
}


void CGqlRequest::SetGqlContext(IGqlContextSharedPtr gqlContext)
{
	if (m_gqlContextPtr.GetPtr() != gqlContext.GetPtr()){
		istd::CChangeNotifier changeNotifier(this);

		m_gqlContextPtr = gqlContext;
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


QByteArray CGqlRequest::GetHeader(const QByteArray& headerId) const
{
	const IGqlContext* gqlContext = GetRequestContext();
	if (gqlContext != nullptr){
		IGqlContext::Headers headers = gqlContext->GetHeaders();

		return headers.value(headerId.toLower());
	}

	return QByteArray();
}


void CGqlRequest::SetHeader(const QByteArray& headerId, const QByteArray& value)
{
	if (m_gqlContextPtr.IsValid()){
		IGqlContext::Headers headers = m_gqlContextPtr->GetHeaders();
		headers.insert(headerId, value);

		m_gqlContextPtr->SetHeaders(headers);
	}
}


const CGqlFieldObject& CGqlRequest::GetFields() const
{
	return m_fields;
}


const CGqlParamObject& CGqlRequest::GetParams() const
{
	return m_params;
}


void CGqlRequest::SetParams(const CGqlParamObject& params)
{
	m_params = params;
}


const CGqlFieldObject* CGqlRequest::GetFieldObject(const QByteArray& fieldId) const
{
	return m_fields.GetFieldArgumentObjectPtr(fieldId);
}


const CGqlParamObject* CGqlRequest::GetParamObject(const QByteArray& paramId) const
{
	return m_params.GetParamArgumentObjectPtr(paramId);
}


const CGqlParamObject& CGqlRequest::GetVariables() const
{
	return m_variables;
}


const QString& CGqlRequest::GetOperationName() const
{
	return m_operationName;
}


void CGqlRequest::SetProtocolVersion(const QByteArray& protocolVersion)
{
	if (m_protocolVersion != protocolVersion){
		istd::CChangeNotifier notifier(this);

		m_protocolVersion = protocolVersion;
	}
}


QByteArray CGqlRequest::GetCommandId() const
{
	return m_commandId;
}


// reimplemented (IGqlRequest)

IGqlRequest::RequestType CGqlRequest::GetRequestType() const
{
	return m_requestType;
}


QByteArray CGqlRequest::GetQuery() const
{
	QByteArray fields = CreateQueryFields();
	QByteArray params = CreateQueryParams();

	QByteArray type;
	switch (m_requestType)
	{
	case RT_QUERY:
		type = QByteArrayLiteral("query");
		break;
	case RT_MUTATION:
		type = QByteArrayLiteral("mutation");
		break;
	case RT_SUBSCRIPTION:
		type = QByteArrayLiteral("subscription");
		break;
	default:
		Q_ASSERT(false);
		return QByteArray();
	}

	if (!params.isEmpty()){
		params.prepend('(').append(')');
	}

	QByteArray queryData = QByteArrayLiteral("{\"query\": \"") + type + ' ' + m_commandId + ' ' + '{' + m_commandId + params + fields + QByteArrayLiteral("}\"}");

	return queryData;
}


const IGqlContext* CGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr.GetPtr();
}


QByteArray CGqlRequest::GetProtocolVersion() const
{
	return m_protocolVersion;
}


bool CGqlRequest::ParseQuery(const QByteArray& query, qsizetype& errorPosition)
{
	if(query.isEmpty()){
		return false;
	}

	const QJsonDocument document = QJsonDocument::fromJson(query);
	if(document.isNull() || !document.isObject()){
		return false;
	}

	QJsonObject rootObject = document.object();
	QByteArray body = rootObject.value(QStringLiteral("query")).toString().toUtf8();
	if (body.isEmpty()) {
		return false;
	}

	// Extract and store the "operationName" key
	QJsonValue operationNameValue = rootObject.value(QStringLiteral("operationName"));
	if (operationNameValue.isString()) {
		m_operationName = operationNameValue.toString();
	} else {
		m_operationName.clear(); // Reset if not present
	}

	// Extract and populate the "variables"
	QJsonValue variablesValue = rootObject.value(QStringLiteral("variables"));
	m_variables.ResetData();
	if (variablesValue.isObject()) {
		ParseObjectParamPart(m_variables, variablesValue.toObject());
	}

	qsizetype index = body.indexOf('{');
	if (index == -1){
		return false;
	}

	const QByteArray header = body.left(index);
	const QList<QByteArray> headersData = header.split(' ');

	QByteArray type;
	for (const QByteArray& headerItem : headersData){
		if (!headerItem.isEmpty()){
			type = headerItem;

			break;
		}
	}

	if (!s_requestNameMap.contains(type)){
		return false;
	}

	m_requestType = s_requestNameMap[type];

	body = body.mid(index + 1);
	index = body.lastIndexOf('}');
	if (index == -1){
		return false;
	}

	body = body.left(index);

	bool endParams = false;
	bool startText = false;
	bool startBigText = false;
	bool startBackSlash = false;

	index = body.indexOf('(');
	if (index > -1){
		m_commandId = body.left(index);
		m_startParams = false;
	}
	else{
		index = body.indexOf('{');
		m_commandId = body.left(index);
		m_startParams = endParams = true;
	}

	m_commandId = m_commandId.simplified().replace(' ', "");
	body = body.remove(0, index);
	m_activeGqlObjectPtr = nullptr;
	m_activeFieldObjectPtr = nullptr;
	m_startFields = false;
	m_startArrayPrimitives = false;
	m_textString = false;
	m_startFragment = false;

	QByteArray text;
	for (qsizetype i = 0; i < body.length() ; ++i){
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
				m_activeGqlObjectPtr = &m_params;
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
					m_activeGqlObjectPtr = nullptr;
					m_activeFieldObjectPtr = &m_fields;
				}
			}
			break;

		case '}':
			if (startBigText){
				text.append(chr);
			}
			else{
				if (!text.isEmpty()){
					SetParseText(text);
					text.clear();
					startText = false;
				}
				if (m_activeGqlObjectPtr != nullptr){
					m_activeGqlObjectPtr = m_activeGqlObjectPtr->GetParentObject();
				}
				if (m_activeFieldObjectPtr != nullptr){
					m_activeFieldObjectPtr = m_activeFieldObjectPtr->GetParentObject();
				}
				if (m_activeGqlObjectPtr == nullptr && m_activeFieldObjectPtr == nullptr){
					if (m_startFields){
						if (!text.isEmpty()){
							SetParseObject(text);
						}

						return true;
					}

					errorPosition = i + index;

					return false;
				}
			}
			break;

		case ',':
			if (startBigText){
				text.append(chr);
			}
			else{
				if (!text.isEmpty()){
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
			if (!startText && !text.isEmpty()){
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
			m_protocolVersion = sourcePtr->m_protocolVersion;
			m_variables = sourcePtr->m_variables;
			m_operationName = sourcePtr->m_operationName;

			return true;
		}
	}

	return false;
}


istd::IChangeableUniquePtr CGqlRequest::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CGqlRequest(m_requestType));
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CGqlRequest::ResetData(istd::IChangeable::CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_commandId.clear();
	m_params.ResetData();
	m_fields.ResetData();
	m_protocolVersion.clear();
	m_variables.ResetData();
	m_operationName.clear();

	return true;
}


// protected methods

QByteArray CGqlRequest::CreateQueryFields() const
{
	QByteArray retVal = AddObjectFieldPart(m_fields);
	if (retVal.isEmpty()){
		retVal = QByteArrayLiteral("{}");
	}

	return retVal;
}


QByteArray CGqlRequest::CreateQueryParams() const
{
	QByteArray retVal = AddObjectParamPart(m_params);

	return retVal;
}


QByteArray CGqlRequest::AddObjectFieldPart(const CGqlFieldObject& gqlObject) const
{
	QByteArray retVal;

	QByteArrayList fieldIds = gqlObject.GetFieldIds();
	if (!fieldIds.isEmpty()){
		retVal.append(' ').append('{');

		for (int i = 0; i < fieldIds.count(); ++i){
			const QByteArray& fieldId = fieldIds[i];
			if (gqlObject.IsFragment(fieldId)){
				retVal += QByteArrayLiteral(" ... on ");
			}
			retVal += fieldId;
			if (gqlObject.IsObject(fieldId)){
				retVal += AddObjectFieldPart(*gqlObject.GetFieldArgumentObjectPtr(fieldId));
			}
			if (gqlObject.IsFragment(fieldId)){
				retVal += AddObjectFieldPart(*gqlObject.GetFragmentArgumentObjectPtr(fieldId));
			}
			if (i < fieldIds.count() - 1){
				retVal += ' ';
			}
		}

		retVal += '}';
	}

	return retVal;
}


QByteArray CGqlRequest::AddObjectParamPart(const CGqlParamObject& gqlObject) const
{
	QByteArray retVal;

	QByteArrayList paramIds = gqlObject.GetParamIds();
	for (qsizetype i = 0; i < paramIds.count(); ++i){
		const QByteArray& paramId = paramIds[i];

		if (gqlObject.IsObject(paramId)){
			retVal += paramId;
			retVal += QByteArrayLiteral(": {");
			retVal += AddObjectParamPart(*gqlObject.GetParamArgumentObjectPtr(paramId));
			retVal += '}';
		}
		else if (gqlObject.IsObjectList(paramId)){
			retVal += paramId + QByteArrayLiteral(" :[");
			qsizetype objectsCount = gqlObject.GetObjectsCount(paramId);
			for (qsizetype objectIndex = 0; objectIndex < objectsCount; objectIndex++){
				if (objectIndex > 0){
					retVal += ',';
				}
				retVal.append(' ').append('{');
				const CGqlParamObject* paramsObject = gqlObject.GetParamArgumentObjectPtr(paramId, objectIndex);
				if (paramsObject != nullptr){
					retVal += AddObjectParamPart(*paramsObject);
				}
				retVal += '}';
			}
			retVal.append(' ').append(']');
		}
		else {
			retVal += paramId;
			retVal.append(':').append(' ');
			QVariant value = gqlObject.GetParamArgumentValue(paramId);
			retVal += AddObjectParamValue(value);
		}
		if (i < paramIds.count() - 1){
			retVal.append(',').append(' ');
		}
	}

	return retVal;
}


QByteArray CGqlRequest::AddObjectParamValue(const QVariant& value) const
{
	QByteArray retVal;
#if QT_VERSION >= 0x060000
	int valueType = value.typeId();
#else
	int valueType = value.type();
#endif
	if (valueType == QMetaType::QVariantList || valueType == QMetaType::QStringList){
		QVariantList variantList = value.toList();
		retVal += '[';
		for (int i = 0; i < variantList.count(); i++){
			if (i > 0){
				retVal.append(',').append(' ');
			}
			retVal += AddObjectParamValue(variantList.value(i));
		}
		retVal += ']';
	}
	else if (valueType == QMetaType::QString || valueType == QMetaType::QByteArray){
		retVal += QByteArrayLiteral("\\\"");
		QByteArray data = value.toByteArray();
		data = data.replace("\\", "\\\\\\\\");
		data = data.replace("\"", "\\\\\\\"");
		data = data.replace('\n', "\\\\\\\n");
		data = data.replace('\r', "\\\\\\\r");
		data = data.replace('\t', "\\\\\\\t");
		retVal += data;
		retVal += QByteArrayLiteral("\\\"");
	}
	else if (value.canConvert<QStringList>()){
		QStringList stringList = value.value<QStringList>();
		retVal += '[';
		for (int i = 0; i < stringList.count(); i++){
			if (i > 0){
				retVal.append(',').append(' ');
			}
			retVal += AddObjectParamValue(stringList.value(i));
		}
		retVal += ']';
	}
	else if (value.canConvert<QList<QString>>()){
		QList<QString> stringList = value.value<QList<QString>>();
		retVal += '[';
		for (int i = 0; i < stringList.count(); i++){
			if (i > 0){
				retVal.append(',').append(' ');
			}
			retVal += AddObjectParamValue(stringList.value(i));
		}
		retVal += ']';
	}
	else if (value.canConvert<QList<int>>()){
		QList<int> intList = value.value<QList<int>>();
		retVal += '[';
		for (int i = 0; i < intList.count(); i++){
			if (i > 0){
				retVal.append(',').append(' ');
			}
			retVal += AddObjectParamValue(intList.value(i));
		}
		retVal += ']';
	}
	else if (value.canConvert<QList<double>>()){
		QList<double> intList = value.value<QList<double>>();
		retVal += '[';
		for (int i = 0; i < intList.count(); i++){
			if (i > 0){
				retVal.append(',').append(' ');
			}
			retVal += AddObjectParamValue(intList.value(i));
		}
		retVal += ']';
	}
	else if (value.canConvert<CGqlEnum>()){
		retVal += value.value<CGqlEnum>().GetValue();
	}
	else {
		QByteArray data = value.toByteArray();
		retVal += data;
	}

	return retVal;
}


void CGqlRequest::ParseObjectFieldPart(CGqlFieldObject& gqlObject, const QJsonObject& object) const
{
	QStringList keys = object.keys();
	for (const QString& key: std::as_const(keys)){
		if (object.value(key).isObject()){
			CGqlFieldObject* slaveGqlObjectPtr = gqlObject.CreateFieldObject(key.toUtf8());
			ParseObjectFieldPart(*slaveGqlObjectPtr, object.value(key).toObject());
		}
		else{
			gqlObject.InsertField(key.toUtf8());
		}
	}
}


void CGqlRequest::ParseObjectParamPart(CGqlParamObject& gqlObject, const QJsonObject& object) const
{
	for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
		const QString key = it.key().trimmed();
		const QJsonValue v = it.value();
		const QByteArray paramId = key.toUtf8();

		if (v.isObject()) {
			if (CGqlParamObject* slaveGqlObjectPtr = gqlObject.CreateParamObject(paramId)) {
				ParseObjectParamPart(*slaveGqlObjectPtr, v.toObject());
			}
		}
		else if (v.isArray()) {
			const QJsonArray arr = v.toArray();
			if (arr.isEmpty()) {
				gqlObject.InsertParam(paramId, QVariantList{});
			}
			else if (std::all_of(arr.begin(), arr.end(), [](const QJsonValue& e){ return e.isObject(); })) {
				QList<CGqlParamObject> objList;
				objList.reserve(arr.size());
				for (const QJsonValue& elem : arr) {
					CGqlParamObject child;
					ParseObjectParamPart(child, elem.toObject());
					objList.append(std::move(child));
				}
				gqlObject.InsertParam(paramId, objList);
			}
			else {
				gqlObject.InsertParam(paramId, arr.toVariantList());
			}
		}
		else if (v.isNull()) {
			gqlObject.InsertParam(paramId, QVariant());
		}
		else {
			if (v.isDouble()) {
				double d = v.toDouble();
				if (static_cast<double>(static_cast<qint64>(d)) == d) {
					gqlObject.InsertParam(paramId, QVariant(static_cast<qint64>(d)));
				} else {
					gqlObject.InsertParam(paramId, QVariant(d));
				}
			} else {
				gqlObject.InsertParam(paramId, v.toVariant());
			}
		}
	}
}


void CGqlRequest::SetParseObject(const QByteArray& commandId)
{
	if (m_startFields){
		if (m_activeFieldObjectPtr != nullptr){
			if (m_startFragment){
				m_activeFieldObjectPtr = m_activeFieldObjectPtr->CreateFragmentObject(commandId);
				m_startFragment = false;
			}
			else{
				m_activeFieldObjectPtr = m_activeFieldObjectPtr->CreateFieldObject(commandId);
			}
		}
	}
	else if (m_startParams){
		CGqlParamObject newObject;
		if (m_activeGqlObjectPtr == nullptr){
			m_params.InsertParam(commandId, newObject);
			m_activeGqlObjectPtr = const_cast<CGqlParamObject*>(m_params.GetParamArgumentObjectPtr(commandId));
			m_currentField = commandId;
		}
		else{
			QByteArray lastArrayId;
			if (!m_activeArrayIds.isEmpty()){
				lastArrayId = m_activeArrayIds.last();
			}
			if (!lastArrayId.isEmpty() && !m_objectArrayList.isEmpty() && m_objectArrayList.last() == m_activeGqlObjectPtr){
				m_activeGqlObjectPtr = m_activeGqlObjectPtr->AppendParamToArray(lastArrayId, newObject);
				m_activeGqlObjectPtr = m_activeGqlObjectPtr->CreateParamObject(commandId);
				m_currentField = commandId;
			}
			else {
				m_activeGqlObjectPtr = m_activeGqlObjectPtr->CreateParamObject(commandId);
				m_currentField = commandId;
			}
		}
	}

}


void CGqlRequest::SetParseText(const QByteArray& text)
{
	if (text == QByteArrayLiteral("...")){
		m_startFragment = true;

		return;
	}
	if (m_startFragment){
		return;
	}

	if (m_activeFieldObjectPtr == nullptr && m_activeGqlObjectPtr == nullptr) {
		SetParseObject(text);
		return;
	}

	QByteArray lastArrayId;
	if (!m_activeArrayIds.isEmpty()){
		lastArrayId = m_activeArrayIds.last();
	}
	if (!lastArrayId.isEmpty() && m_startArrayPrimitives){

		QVariant value = m_activeGqlObjectPtr->GetParamArgumentValue(lastArrayId);
		QVariantList variantList;
		if (!value.isNull()){
			variantList = value.toList();
		}
		if (m_textString){
			QString stringValue = text;
			variantList.append(stringValue);
		}
		else if (text == QByteArrayLiteral("true")){
			variantList.append(QVariant(true));
		}
		else if (text == QByteArrayLiteral("false")){
			variantList.append(QVariant(false));
		}
		else {
			bool ok;
			if (text.contains('.')){
				double doubleValue = text.toDouble(&ok);
				variantList.append(doubleValue);
			}
			else{
				qint64 intValue = text.toLongLong(&ok);
				variantList.append(intValue);
			}
		}
		m_activeGqlObjectPtr->InsertParam(lastArrayId, variantList);

		return;
	}

	if (!lastArrayId.isEmpty() && !m_objectArrayList.isEmpty() && m_objectArrayList.last() == m_activeGqlObjectPtr) {
		CGqlParamObject newObject;
		m_activeGqlObjectPtr = m_activeGqlObjectPtr->AppendParamToArray(lastArrayId, newObject);
		m_currentField = text;
	}
	else if (m_startKey){
		if (m_activeFieldObjectPtr != nullptr){
			m_activeFieldObjectPtr->InsertField(text);
		}

		m_currentField = text;
	}
	else {
		// variable substitution
		// If text starts with '$' and we aren't in a quoted string, resolve from m_variables
		if (m_startValue && !m_textString && !text.isEmpty() && text[0] == '$' && m_activeGqlObjectPtr != nullptr) {
			if (!m_currentField.isEmpty()) {
				QByteArray varId = text.mid(1);
				if (m_variables.ContainsParam(varId)) {

					if (m_variables.IsObject(varId)) {
						const CGqlParamObject* varObj = m_variables.GetParamArgumentObjectPtr(varId);
						if (varObj) {
							m_activeGqlObjectPtr->InsertParam(m_currentField, *varObj);
						}
					}
					else if (m_variables.IsObjectList(varId)) {
						QList<CGqlParamObject> objList;
						auto ptrList = m_variables.GetParamArgumentObjectPtrList(varId);
						for (auto* param : std::as_const(ptrList)) {
							if (param) {
								objList.append(*param);
							}
						}
						m_activeGqlObjectPtr->InsertParam(m_currentField, objList);
					}
					else {
						m_activeGqlObjectPtr->InsertParam(m_currentField, m_variables.GetParamArgumentValue(varId));
					}

					m_currentField.clear();
					m_startValue = false;
					return;
				}
			}
		}

		QVariant value;
		if (m_textString) {
			value = QString::fromUtf8(text);
		}
		else if (text == QByteArrayLiteral("true")){
			value = true;
		}
		else if (text == QByteArrayLiteral("false")){
			value = false;
		}
		else if (text == QByteArrayLiteral("null")) {
			value = QVariant();
		}
		else {
			bool ok;
			if (text.contains('.')){
				double doubleValue = text.toDouble(&ok);
				value = doubleValue;
			}
			else{
				qint64 intValue = text.toLongLong(&ok);
				if (ok && QString::number(intValue).toUtf8() == text) {
					value = intValue;
				}
				else {
					value = text;
				}
			}
		}

		if (!m_currentField.isEmpty() && m_activeGqlObjectPtr != nullptr) {
			m_activeGqlObjectPtr->InsertParam(m_currentField, value);
		}

		m_currentField.clear();
		m_startValue = false;
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
			CGqlParamObject newObject;
			m_activeGqlObjectPtr = m_activeGqlObjectPtr->AppendParamToArray(lastArrayId, newObject);
		}
		QList<CGqlParamObject> objectList;
		m_activeGqlObjectPtr->InsertParam(text, objectList);
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


