// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CPath.h>


namespace imtoas
{



QString CPath::GetId() const
{
	return m_id;
}


void CPath::SetId(const QString& id)
{
	if (id != m_id){
		m_id = id;
	}
}


QString CPath::GetReference() const
{
	return m_reference;
}


void CPath::SetReference(const QString& reference)
{
	if (reference != m_reference){
		m_reference = reference;
	}
}


QString CPath::GetSummary() const
{
	return m_summary;
}


void CPath::SetSummary(const QString& summary)
{
	if (summary != m_summary){
		m_summary = summary;
	}
}


QString CPath::GetDescription() const
{
	return m_description;
}


void CPath::SetDescription(const QString& description)
{
	if (description != m_description){
		m_description = description;
	}
}


COperation CPath::GetGet() const
{
	return m_get;
}


void CPath::SetGet(const COperation& get)
{
	if (get != m_get){
		m_get = get;
	}
}


COperation CPath::GetPut() const
{
	return m_put;
}


void CPath::SetPut(const COperation& put)
{
	if (put != m_put){
		m_put = put;
	}
}


COperation CPath::GetPost() const
{
	return m_post;
}


void CPath::SetPost(const COperation& post)
{
	if (post != m_post){
		m_post = post;
	}
}


COperation CPath::GetDelete() const
{
	return m_delete;
}


void CPath::SetDelete(const COperation& deleted)
{
	if (deleted != m_delete){
		m_delete = deleted;
	}
}


COperation CPath::GetOptions() const
{
	return m_options;
}


void CPath::SetOptions(const COperation& options)
{
	if (options != m_options){
		m_options = options;
	}
}


COperation CPath::GetHead() const
{
	return m_head;
}


void CPath::SetHead(const COperation& head)
{
	if (head != m_head){
		m_head = head;
	}
}


COperation CPath::GetPatch() const
{
	return m_patch;
}


void CPath::SetPatch(const COperation& patch)
{
	if (patch != m_patch){
		m_patch = patch;
	}
}


COperation CPath::GetTrace() const
{
	return m_trace;
}


void CPath::SetTrace(const COperation& trace)
{
	if (trace != m_trace){
		m_trace = trace;
	}
}


QList<CServer> CPath::GetServers() const
{
	return m_servers;
}


void CPath::SetServers(const QList<CServer>& servers)
{
	if (servers != m_servers){
		m_servers = servers;
	}
}


QList<CParameter> CPath::GetParameters() const
{
	return m_parameters;
}


void CPath::SetParameters(const QList<CParameter>& parameters)
{
	if (parameters != m_parameters){
		m_parameters = parameters;
	}
}


bool CPath::ReadFromJsonObject(CPath& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant referenceData = jsonObject.value("$ref").toVariant();
	if (!referenceData.isNull()){
		object.SetReference(referenceData.toString());
	}

	QVariant summaryData = jsonObject.value("summary").toVariant();
	if (!summaryData.isNull()){
		object.SetSummary(summaryData.toString());
	}

	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	if (jsonObject.contains("get")){
		COperation get;
		const bool isGetReaded = COperation::ReadFromJsonObject(get, jsonObject["get"].toObject(), globalObject);
		if (!isGetReaded){
			return false;
		}
		object.SetGet(get);
	}

	if (jsonObject.contains("put")){
		COperation put;
		const bool isPutReaded = COperation::ReadFromJsonObject(put, jsonObject["put"].toObject(), globalObject);
		if (!isPutReaded){
			return false;
		}
		object.SetPut(put);
	}

	if (jsonObject.contains("post")){
		COperation post;
		const bool isPostReaded = COperation::ReadFromJsonObject(post, jsonObject["post"].toObject(), globalObject);
		if (!isPostReaded){
			return false;
		}
		object.SetPost(post);
	}

	if (jsonObject.contains("delete")){
		COperation deleteOperation;
		const bool isDeleteReaded = COperation::ReadFromJsonObject(deleteOperation, jsonObject["delete"].toObject(), globalObject);
		if (!isDeleteReaded){
			return false;
		}
		object.SetDelete(deleteOperation);
	}

	if (jsonObject.contains("options")){
		COperation options;
		const bool isOptionsReaded = COperation::ReadFromJsonObject(options, jsonObject["options"].toObject(), globalObject);
		if (!isOptionsReaded){
			return false;
		}
		object.SetOptions(options);
	}

	if (jsonObject.contains("head")){
		COperation head;
		const bool isHeadReaded = COperation::ReadFromJsonObject(head, jsonObject["head"].toObject(), globalObject);
		if (!isHeadReaded){
			return false;
		}
		object.SetHead(head);
	}

	if (jsonObject.contains("patch")){
		COperation patch;
		const bool isPatchReaded = COperation::ReadFromJsonObject(patch, jsonObject["patch"].toObject(), globalObject);
		if (!isPatchReaded){
			return false;
		}
		object.SetPatch(patch);
	}

	if (jsonObject.contains("trace")){
		COperation trace;
		const bool isTraceReaded = COperation::ReadFromJsonObject(trace, jsonObject["trace"].toObject(), globalObject);
		if (!isTraceReaded){
			return false;
		}
		object.SetTrace(trace);
	}

	if (jsonObject.contains("servers")){
		const QJsonArray serversArray = jsonObject["servers"].toArray();
		qsizetype serversCount = serversArray.size();
		QList<CServer> serversList;
		for (int serversIndex = 0; serversIndex < serversCount; ++serversIndex){
			CServer servers;
			if (!CServer::ReadFromJsonObject(servers, serversArray[serversIndex].toObject(), globalObject)){
				return false;
			}
			serversList << servers;
		}
		object.SetServers(serversList);
	}

	if (jsonObject.contains("parameters")){
		const QJsonArray parametersArray = jsonObject["parameters"].toArray();
		qsizetype parametersCount = parametersArray.size();
		QList<CParameter> parametersList;
		for (int parametersIndex = 0; parametersIndex < parametersCount; ++parametersIndex){
			CParameter parameters;
			if (!CParameter::ReadFromJsonObject(parameters, parametersArray[parametersIndex].toObject(), globalObject)){
				return false;
			}
			parametersList << parameters;
		}
		object.SetParameters(parametersList);
	}

	return true;
}


bool CPath::operator==(const CPath& other) const
{
	bool retVal = true;
	retVal = retVal && (m_id == other.m_id);
	retVal = retVal && (m_reference == other.m_reference);
	retVal = retVal && (m_summary == other.m_summary);
	retVal = retVal && (m_description == other.m_description);
	retVal = retVal && (m_get == other.m_get);
	retVal = retVal && (m_put == other.m_put);
	retVal = retVal && (m_post == other.m_post);
	retVal = retVal && (m_delete == other.m_delete);
	retVal = retVal && (m_options == other.m_options);
	retVal = retVal && (m_head == other.m_head);
	retVal = retVal && (m_patch == other.m_patch);
	retVal = retVal && (m_trace == other.m_trace);
	retVal = retVal && (m_servers == other.m_servers);
	retVal = retVal && (m_parameters == other.m_parameters);

	return retVal;
}


} // namespace imtoas

