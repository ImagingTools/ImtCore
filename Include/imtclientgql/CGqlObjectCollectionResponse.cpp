#include <imtclientgql/CGqlObjectCollectionResponse.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/IGqlRequest.h>
#include <imtgql/IGqlResponse.h>


namespace imtclientgql
{


// public methods

CGqlObjectCollectionResponse::CGqlObjectCollectionResponse(const imtgql::CGqlRequest& request)
	: m_request(request),
	m_isNodeInfoPresent(false),
	m_isObjectInfoPresent(false),
	m_isElementInfoPresent(false),
	m_isElementListPresent(false),
	m_isPrimitiveTypePresent(false)
{
}


// reimplemented (imtgql::IGqlStructuredCollectionResponse)

bool CGqlObjectCollectionResponse::GetObjectInfo(ObjectInfo& out) const
{
	if (m_isObjectInfoPresent){
		out = m_objectInfo;

		return true;
	}

	return false;
}


bool CGqlObjectCollectionResponse::GetObjectList(ObjectList& out) const
{
	if (m_isElementListPresent){
		out = m_objectList;

		return true;
	}

	return false;
}


bool CGqlObjectCollectionResponse::GetMetaInfo(idoc::IDocumentMetaInfo& out) const
{
	return false;
}


bool CGqlObjectCollectionResponse::DeSerializeObject(istd::IChangeable& object) const
{
	return false;
}


// reimplemented (imtgql::IGqlResponse)

QVariant CGqlObjectCollectionResponse::GetResult() const
{
	if (m_isPrimitiveTypePresent){
		return m_variant;
	}

	return QVariant();
}


// reimplemented (imtgql::IGqlResponse)

bool CGqlObjectCollectionResponse::IsSuccessful() const
{
	return !m_json.object().contains("errors");
}


// reimplemented (imtclientgql::IGqlClient::ResponseHandler)

void CGqlObjectCollectionResponse::OnReply(const imtgql::IGqlRequest& request, const QByteArray& replyData)
{
	m_data = replyData;
	m_commandId = request.GetCommandId();

	m_json = QJsonDocument::fromJson(replyData);

	QJsonObject data = m_json.object().value("data").toObject();

	if (data.contains(m_commandId)){
		data = data.value(m_commandId).toObject();
	}

	if (data.contains("itemsCount")){
		m_variant = data.value("itemsCount").toVariant();
		m_isPrimitiveTypePresent = true;
	}

	if (data.contains("itemIds")){
		QStringList itemIdsData = data.value("itemIds").toString().split(";");
		ObjectInfo itemInfo;

		for (QString id: itemIdsData){
			itemInfo.id = id.toLatin1();
			m_objectList.push_back(itemInfo);
		}
		m_isElementListPresent = true;
	}

	if (data.contains("objectData")){
		m_variant = data.value("objectData").toVariant();
		m_isPrimitiveTypePresent = true;
	}

	if (data.contains("renameNotification") || data.contains("setDescriptionNotification")){
		m_variant = true;
		m_isPrimitiveTypePresent = true;
	}

	QByteArrayList keys;
	keys << "info";
	keys << "metaInfo";
	keys << "dataMetaInfo";
	keys << "addedNotification";
	keys << "removedNotification";
	keys << "updatedNotification";
	keys << "updatedCollectionNotification";
	keys << "items";

	for (QByteArray key: keys){
		if (data.contains(key)){
			m_variant = data;
			m_isPrimitiveTypePresent = true;

			break;
		}
	}

}


// private methods

void CGqlObjectCollectionResponse::ParseCreateFolderReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("createFolder").toObject();
	if (responseObject.contains("id") && responseObject.value("id").isString()){
		m_isPrimitiveTypePresent = true;

		m_variant = responseObject.value("id").toString().toLatin1();
	}
}

void CGqlObjectCollectionResponse::ParseDeleteFolderReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("deleteFolder").toObject();
	if (responseObject.contains("succeeded") && responseObject.value("succeeded").isBool()){
		m_isPrimitiveTypePresent = true;

		m_variant = responseObject.value("succeeded").toBool();
	}
}


void CGqlObjectCollectionResponse::ParseAddMeasurementReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("addMeasurement").toObject();
	if (responseObject.contains("data") && responseObject.value("data").isObject()){
		QJsonObject data = responseObject.value("data").toObject();
		if (data.contains("id") && data.value("id").isString()){
			m_isPrimitiveTypePresent = true;

			m_variant = data.value("id").toString().toLatin1();
		}
	}
}


void CGqlObjectCollectionResponse::ParseReplaceMeasurementReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("replaceMeasurement").toObject();
	if (responseObject.contains("id") && responseObject.value("id").isString()){
		m_isPrimitiveTypePresent = true;

		m_variant = responseObject.value("id").toString().toLatin1();
	}
}


void CGqlObjectCollectionResponse::ParseRenameMeasurementReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("renameMeasurement").toObject();
	if (responseObject.contains("id") && responseObject.value("id").isString()){
		m_isPrimitiveTypePresent = true;

		m_variant = responseObject.value("id").toString().toLatin1();
	}
}


void CGqlObjectCollectionResponse::ParseDeleteMeasurementReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("deleteMeasurement").toObject();
	if (responseObject.contains("succeeded") && responseObject.value("succeeded").isBool()){
		m_isPrimitiveTypePresent = true;

		m_variant = responseObject.value("succeeded").toBool();
	}
}


void CGqlObjectCollectionResponse::ParseGetMeasurementUploadUrlsReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("getMeasurementUploadUrls").toObject();
	if (responseObject.contains("uploadUrls") && responseObject.value("uploadUrls").isArray()){
		QJsonArray uploadUrls = responseObject.value("uploadUrls").toArray();
		QStringList urls;

		for (int i = 0; i < uploadUrls.count(); i++){
			QJsonObject uploadUrl = uploadUrls[i].toObject();
			if (uploadUrl.contains("url") && uploadUrl.value("url").isString()){
				m_isPrimitiveTypePresent = true;

				urls += uploadUrl.value("url").toString();
			}
		}

		m_variant = urls;
	}
}


void CGqlObjectCollectionResponse::ParseGetMeasurementDownloadUrlsReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("getMeasurementDownloadUrl").toObject();
	if (responseObject.contains("url") && responseObject.value("url").isString()){
		m_isPrimitiveTypePresent = true;

		QStringList urls;
		urls += responseObject.value("url").toString();

		m_variant = urls;
	}
}


} // namespace gmgaws


