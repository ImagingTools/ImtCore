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

CGqlObjectCollectionResponse::CGqlObjectCollectionResponse()
{
	m_isNodeInfoPresent = false;
	m_isObjectInfoPresent = false;
	m_isElementInfoPresent = false;
	m_isElementListPresent = false;
	m_isPrimitiveTypePresent = false;
}


// reimplemented (imtgql::IGqlStructuredCollectionResponse)

bool CGqlObjectCollectionResponse::GetNodeInfo(NodeInfo& out) const
{
	if (m_isNodeInfoPresent){
		out = m_elementInfo;

		return true;
	}

	return false;
}

bool CGqlObjectCollectionResponse::GetObjectInfo(ObjectInfo& out) const
{
	if (m_isObjectInfoPresent){
		out = m_elementInfo;

		return true;
	}

	return false;
}

bool CGqlObjectCollectionResponse::GetElementInfo(ElementInfo& out) const
{
	if (m_isElementInfoPresent) {
		out = m_elementInfo;

		return true;
	}

	return false;
}

bool CGqlObjectCollectionResponse::GetElementList(ElementList& out) const
{
	if (m_isElementListPresent) {
		out = m_elementList;

		return true;
	}

	return false;
}


// reimplemented (imtgql::IGqlResponse)

bool CGqlObjectCollectionResponse::GetValue(QVariant& out) const
{
	if (m_isPrimitiveTypePresent) {
		out = m_variant;

		return true;
	}

	return false;
}


// reimplemented (imtgql::IGqlResponse)

bool CGqlObjectCollectionResponse::IsSuccessfull() const
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
		ElementInfo itemInfo;

		for (QString id: itemIdsData){
			itemInfo.isNode = false;
			itemInfo.id = id.toLatin1();
			m_elementList.push_back(itemInfo);
		}
		m_isElementListPresent = true;
	}

	if (data.contains("objectData")){
		m_variant = data.value("objectData").toVariant();
		qDebug() << m_variant;
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
//			qDebug() << m_variant;
			m_isPrimitiveTypePresent = true;

			break;
		}
	}

}


// private methods

void CGqlObjectCollectionResponse::ParseFolderContentReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("getFolderContent").toObject();
	if (responseObject.contains("folder") && responseObject.value("folder").isObject()) {
		QJsonObject folder = responseObject.value("folder").toObject();

		m_isElementInfoPresent = true;
		m_isNodeInfoPresent = true;

		m_elementInfo.id = folder.value("id").toString().toLatin1();
		m_elementInfo.name = folder.value("name").toString().toLatin1();

		QJsonArray path = folder.value("fullPath").toArray();
		for (int i = 0; i < path.size(); i++) {
			QByteArray id = path[i].toObject().value("id").toString().toLatin1();
			//Q_ASSERT(i == 0 ? id.isEmpty() : !id.isEmpty());

//			m_elementInfo.path += id;
		}

		m_elementInfo.isNode = true;
	}

	if (responseObject.contains("subfolders") && responseObject.value("subfolders").isArray()) {
		QJsonArray arrayFolders = responseObject.value("subfolders").toArray();
		for (int i = 0; i < arrayFolders.count(); ++i) {
			m_isElementListPresent = true;

			QJsonObject jsonObject = arrayFolders[i].toObject();

			ElementInfo folderInfo;

			folderInfo.isNode = true;
			folderInfo.id = jsonObject.value("id").toString().toLatin1();
			folderInfo.name = jsonObject.value("name").toString();

			m_elementList.push_back(folderInfo);
		}
	}

	if (responseObject.contains("measurements") && responseObject.value("measurements").isArray()) {
		QJsonArray arrayDocuments = responseObject.value("measurements").toArray();
		for (int i = 0; i < arrayDocuments.count(); ++i) {
			m_isElementListPresent = true;

			QJsonObject jsonObject = arrayDocuments[i].toObject();

			ElementInfo documentInfo;

			documentInfo.isNode = false;
			documentInfo.id = jsonObject.value("id").toString().toLatin1();
			documentInfo.name = jsonObject.value("name").toString();
			documentInfo.typeId = "Measurement";
			documentInfo.version = -1;
			if (jsonObject.contains("versionInformation")){
				QJsonObject versionInformation = jsonObject.value("versionInformation").toObject();

				documentInfo.version = versionInformation.value("version").toInt();
			}

			documentInfo.elementMetaInfoPtr.reset(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>);
			QString timeStr = jsonObject.value("createdOn").toString();
			documentInfo.elementMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, QDateTime::fromString(timeStr, Qt::ISODate));
			timeStr = jsonObject.value("timestamp").toString();
			documentInfo.elementMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, QDateTime::fromString(timeStr, Qt::ISODate));

			QByteArray data = jsonObject.value("metadata").toString().toUtf8();
			QJsonObject metadata = QJsonDocument::fromJson(data).object();
			QJsonArray inkNamesArray = metadata.value("InkNames").toArray();
			QString inkNamesStr;
			for (int inkIndex = 0; inkIndex < inkNamesArray.count(); ++inkIndex) {
				inkNamesStr += inkNamesArray.at(inkIndex).toObject().value("Name").toString();
				if (inkIndex < inkNamesArray.count() - 1) {
					inkNamesStr += " ";
				}
			}
			QJsonArray measuringDevicesArray = metadata.value("MeasuringDevices").toArray();
			QStringList measuringDeviceList;
			QString measuringDevices;
			for (int deviceIndex = 0; deviceIndex < measuringDevicesArray.count(); ++deviceIndex) {
				QString device = measuringDevicesArray.at(deviceIndex).toString();
				if (!measuringDeviceList.contains(device)) {
					measuringDeviceList += device;
					measuringDevices += device;
					measuringDevices += " ";
				}
			}

			QJsonArray measurementConditionsArray = metadata.value("MeasurementConditions").toArray();
			QStringList measurementConditionList;
			QString measurementConditions;
			for (int conditionIndex = 0; conditionIndex < measurementConditionsArray.count(); ++conditionIndex) {
				QString condition = measurementConditionsArray.at(conditionIndex).toString();
				if (!measurementConditionList.contains(condition)) {
					measurementConditionList += condition;
					measurementConditions += condition;
					measurementConditions += " ";
				}
			}

			documentInfo.dataMetaInfoPtr.reset(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>);
			documentInfo.dataMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE, documentInfo.name);
			documentInfo.dataMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, documentInfo.elementMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME));
			documentInfo.dataMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, documentInfo.elementMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME));
			documentInfo.dataMetaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME, documentInfo.elementMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME));
			documentInfo.dataMetaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, documentInfo.elementMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME));

			m_elementList.push_back(documentInfo);
		}
	}
}


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
	if (responseObject.contains("succeeded") && responseObject.value("succeeded").isBool()) {
		m_isPrimitiveTypePresent = true;

		m_variant = responseObject.value("succeeded").toBool();
	}
}


void CGqlObjectCollectionResponse::ParseAddMeasurementReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("addMeasurement").toObject();
	if (responseObject.contains("data") && responseObject.value("data").isObject()) {
		QJsonObject data = responseObject.value("data").toObject();
		if (data.contains("id") && data.value("id").isString()) {
			m_isPrimitiveTypePresent = true;

			m_variant = data.value("id").toString().toLatin1();
		}
	}
}


void CGqlObjectCollectionResponse::ParseReplaceMeasurementReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("replaceMeasurement").toObject();
	if (responseObject.contains("id") && responseObject.value("id").isString()) {
		m_isPrimitiveTypePresent = true;

		m_variant = responseObject.value("id").toString().toLatin1();
	}
}


void CGqlObjectCollectionResponse::ParseRenameMeasurementReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("renameMeasurement").toObject();
	if (responseObject.contains("id") && responseObject.value("id").isString()) {
		m_isPrimitiveTypePresent = true;

		m_variant = responseObject.value("id").toString().toLatin1();
	}
}


void CGqlObjectCollectionResponse::ParseDeleteMeasurementReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("deleteMeasurement").toObject();
	if (responseObject.contains("succeeded") && responseObject.value("succeeded").isBool()) {
		m_isPrimitiveTypePresent = true;

		m_variant = responseObject.value("succeeded").toBool();
	}
}


void CGqlObjectCollectionResponse::ParseGetMeasurementUploadUrlsReply()
{
	QJsonObject responseObject = m_json.object().value("data").toObject().value("getMeasurementUploadUrls").toObject();
	if (responseObject.contains("uploadUrls") && responseObject.value("uploadUrls").isArray()) {
		QJsonArray uploadUrls = responseObject.value("uploadUrls").toArray();
		QStringList urls;

		for (int i = 0; i < uploadUrls.count(); i++) {
			QJsonObject uploadUrl = uploadUrls[i].toObject();
			if (uploadUrl.contains("url") && uploadUrl.value("url").isString()) {
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
	if (responseObject.contains("url") && responseObject.value("url").isString()) {
		m_isPrimitiveTypePresent = true;

		QStringList urls;
		urls += responseObject.value("url").toString();

		m_variant = urls;
	}
}


} // namespace gmgaws


