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

CGqlHierarchicalStructureResponse::CGqlHierarchicalStructureResponse(const imtgql::CGqlRequest& request)
	:m_request(request)
{
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

		if (m_commandId == "InsertNewNode"){
			ProcessInsertNewNodeReply();
		}
		else if (m_commandId == "SetNodeName"){
			ProcessSetNodeNameReply();
		}
		else if (m_commandId == "SetNodeDescription"){
			ProcessSetNodeDescriptionReply();
		}
		else if (m_commandId == "SetNodeMetaInfo"){
			ProcessSetNodeMetaInfoReply();
		}
		else if (m_commandId == "MoveNode"){
			ProcessMoveNodeReply();
		}
		else if (m_commandId == "RemoveNode"){
			ProcessRemoveNodeReply();
		}
		else if (m_commandId == "InsertNewLeaf"){
			ProcessInsertNewLeafReply();
		}
		else if (m_commandId == "MoveLeaf"){
			ProcessMoveLeafReply();
		}
		else if (m_commandId == "RemoveLeaf"){
			ProcessRemoveLeafReply();
		}
		else if (m_commandId == "GetItemCount"){
			ProcessGetItemCountReply();
		}
		else if (m_commandId == "GetItemIds"){
			ProcessGetItemIdsReply();
		}
		else if (m_commandId == "GetItemInfos"){
			ProcessGetItemInfosReply();
		}
		else if (m_commandId == "GetItemPath"){
			ProcessGetItemPathReply();
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


