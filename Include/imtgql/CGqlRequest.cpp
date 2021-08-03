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


bool CGqlRequest::ParseQuery(const QByteArray &query)
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
	index = body.indexOf("{");
	QByteArray commands = body.left(index);
	while (body.at(0) == ' '){
		body.remove(0,1);
	}
	index = body.indexOf(" ");
	m_commandId = body.left(index);
	body = body.mid(index + 1);
	index = body.indexOf("{");
	QByteArray commandParams = body.left(index);
	body = body.remove(0,index);
	while (commandParams.at(0) == ' '){
		commandParams = commandParams.remove(0,1);
	}
	while (commandParams.back() == ' '){
		commandParams.resize(commandParams.length() - 1);
	}
	if (!commandParams.isEmpty()){
		commandParams.front() = '{';
		commandParams.back() = '}';
		QJsonDocument paramsDocument = QJsonDocument::fromJson(commandParams);
		if (paramsDocument.isObject()){
			QJsonObject object = paramsDocument.object();
			QStringList keys = object.keys();
			for (QString key: keys){
				CGqlObject gqlObject(key.toUtf8());
				if (object.value(key).isObject()){
					ParceObjectParamPart(gqlObject, object.value(key).toObject());
				}
				AddParam(gqlObject);
			}
		}
	}
	for (int i = 0; i < 3; i++){
		body = body.replace("  "," ");
	}
	body = body.replace("{",":{");
	body = body.replace(" {",":{");
	body = body.replace(" }","}");
	body = body.replace("} ","}#");
	body = body.replace(" ",":\"\",");
	body = body.replace("}#","} ");
	QJsonDocument bodyDocument = QJsonDocument::fromJson(body);
	if (bodyDocument.isObject()){
		QJsonObject object = bodyDocument.object();
		QStringList keys = object.keys();
		for (QString key: keys){
			CGqlObject gqlObject(key.toUtf8());
			if (object.value(key).isObject()){
				ParceObjectFieldPart(gqlObject, object.value(key).toObject());
			}
			AddField(gqlObject);
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


} // namespace imtgql


