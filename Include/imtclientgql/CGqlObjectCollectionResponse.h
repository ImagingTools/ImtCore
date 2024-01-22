#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtclientgql/IGqlObjectCollectionResponse.h>
#include <imtgql/CGqlRequest.h>


namespace imtclientgql
{


class CGqlObjectCollectionResponse:
			public ilog::CMessageContainer,
			virtual public imtclientgql::IGqlObjectCollectionResponse
{
public:
	CGqlObjectCollectionResponse() = delete;
	CGqlObjectCollectionResponse(const imtgql::CGqlRequest& request);

	// reimplemented (imtclientgql::IGqlObjectCollectionResponse)
	virtual bool GetObjectInfo(ObjectInfo& out) const override;
	virtual bool GetObjectList(ObjectList& out) const override;
	virtual bool GetMetaInfo(idoc::IDocumentMetaInfo& out) const;
	virtual bool DeSerializeObject(istd::IChangeable& object) const;

	// reimplemented (imtgql::IGqlResponse)
	virtual bool IsSuccessful() const override;
	virtual QVariant GetResult() const override;
	
	// reimplemented (imtclientgql::IGqlClient::ResponseHandler)
	virtual void OnReply(const imtgql::IGqlRequest& request, const QByteArray& replyData) override;

private:
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

	ObjectInfo m_objectInfo;
	ObjectList m_objectList;
	QVariant m_variant;

	bool m_isNodeInfoPresent;
	bool m_isObjectInfoPresent;
	bool m_isElementInfoPresent;
	bool m_isElementListPresent;
	bool m_isPrimitiveTypePresent;
};


} // namespace gmgaws


