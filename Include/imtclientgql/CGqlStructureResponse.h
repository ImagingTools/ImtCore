//#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlStructuredCollectionResponse.h>
#include <imtgql/IGqlPrimitiveTypeResponse.h>


namespace imtclientgql
{


class CGqlStructureResponse:
			public ilog::CMessageContainer,
			virtual public imtgql::IGqlPrimitiveTypeResponse,
			virtual public imtgql::IGqlStructuredCollectionResponse
{
public:
	CGqlStructureResponse();

	void SetRequest(const imtgql::CGqlRequest& request);

	// reimplemented (imtgql::IGqlStructuredCollectionResponse)
	virtual bool GetNodeInfo(NodeInfo& out) const override;
	virtual bool GetObjectInfo(ObjectInfo& out) const override;
	virtual bool GetElementInfo(ElementInfo& out) const override;
	virtual bool GetElementList(ElementList& out) const override;

	// reimplemented (imtgql::IGqlPrimitiveTypeResponse)
	virtual bool GetValue(QVariant& out) const override;

	// reimplemented (imtgql::IGqlResponse)
	virtual bool IsSuccessfull() const override;

	// reimplemented (imtclientgql::IGqlClient::ResponseHandler)
	virtual void OnReply(const imtgql::IGqlRequest& request, const QByteArray& replyData) override;

private:
	void ParseFolderContentReply();
	void ParseCreateFolderReply();
	void ParseDeleteFolderReply();
	void ParseAddMeasurementReply();
	void ParseReplaceMeasurementReply();
	void ParseRenameMeasurementReply();
	void ParseDeleteMeasurementReply();
	void ParseGetMeasurementUploadUrlsReply();
	void ParseGetMeasurementDownloadUrlsReply();

private:
	imtgql::CGqlRequest m_request;

	QByteArray m_data;
	QByteArray m_commandId;
	QJsonDocument m_json;

	QVariant m_variant;

	bool m_isNodeInfoPresent;
	bool m_isPrimitiveTypePresent;
};


} // namespace gmgaws


