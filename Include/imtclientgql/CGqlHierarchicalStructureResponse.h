//#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtbase/IHierarchicalStructureInfo.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlResponse.h>


namespace imtclientgql
{


class CGqlHierarchicalStructureResponse:
			public ilog::CMessageContainer,
			virtual public imtgql::IGqlResponse
{
public:
	CGqlHierarchicalStructureResponse() = delete;
	CGqlHierarchicalStructureResponse(const imtgql::CGqlRequest& request);

	// reimplemented (imtgql::IGqlResponse)
	virtual bool IsSuccessful() const override;
	virtual QVariant GetResult() const override;

	// reimplemented (imtclientgql::IGqlClient::ResponseHandler)
	virtual void OnReply(const imtgql::IGqlRequest& request, const QByteArray& replyData) override;

private:
	void ProcessInsertNewNodeReply();
	void ProcessSetNodeNameReply();
	void ProcessSetNodeDescriptionReply();
	void ProcessSetNodeMetaInfoReply();
	void ProcessMoveNodeReply();
	void ProcessRemoveNodeReply();
	void ProcessInsertNewLeafReply();
	void ProcessMoveLeafReply();
	void ProcessRemoveLeafReply();
	void ProcessGetItemCountReply();
	void ProcessGetItemIdsReply();
	void ProcessGetItemInfosReply();
	void ProcessGetItemPathReply();

private:
	imtgql::CGqlRequest m_request;

	QByteArray m_data;
	QByteArray m_commandId;
	QJsonDocument m_json;

	QVariant m_variant;
};


} // namespace gmgaws


