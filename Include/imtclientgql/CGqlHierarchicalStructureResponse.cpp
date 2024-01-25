#include <imtclientgql/CGqlHierarchicalStructureResponse.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/IGqlResponse.h>


namespace imtclientgql
{


// public methods

void CGqlHierarchicalStructureResponse::SetRequest(const imtgql::CGqlRequest& request)
{
	m_request = request;
}


// reimplemented (imtgql::IGqlResponse)

bool CGqlHierarchicalStructureResponse::IsSuccessful() const
{
	return !m_json.object().contains("errors");
}


QVariant CGqlHierarchicalStructureResponse::GetResult() const
{
	return m_variant;
}


// reimplemented (imtclientgql::IGqlClient::ResponseHandler)

void CGqlHierarchicalStructureResponse::OnReply(const imtgql::IGqlRequest& request, const QByteArray& replyData)
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
	}

	if (data.contains("itemIds")){
		QStringList itemIdsData = data.value("itemIds").toString().split(";");
//		ElementInfo itemInfo;

//		for (QString id: itemIdsData){
//			itemInfo.isNode = false;
//			itemInfo.id = id.toLatin1();
//			// m_elementList.push_back(itemInfo);
//		}
		// m_isElementListPresent = true;
	}

	if (data.contains("objectData")){
		m_variant = data.value("objectData").toVariant();
		qDebug() << m_variant;
	}

	if (data.contains("renameNotification") || data.contains("setDescriptionNotification")){
		m_variant = true;
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

			break;
		}
	}
}


void CGqlHierarchicalStructureResponse::ProcessInsertNewNodeReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessSetNodeNameReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessSetNodeDescriptionReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessSetNodeMetaInfoReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessMoveNodeReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessRemoveNodeReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessInsertNewLeafReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessMoveLeafReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessRemoveLeafReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessGetItemCountReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessGetItemIdsReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessGetItemInfosReply()
{

}


void CGqlHierarchicalStructureResponse::ProcessGetItemPathReply()
{

}


} // namespace imtclientgql


