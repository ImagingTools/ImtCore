#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtgql/IGqlResponse.h>


namespace imtgql
{


class IGqlStructuredCollectionResponse : virtual public IGqlResponse
{
public:
	enum MetaInfoType
	{
		MIT_KEYWORDS = idoc::IDocumentMetaInfo::MIT_USER + 30000,
		MIT_VERSION
	};

	struct NodeInfo
	{
		QByteArray id;
		QString name;
		QString description;
		QByteArrayList path;

		QSharedPointer<idoc::IDocumentMetaInfo> elementMetaInfoPtr;
	};

	struct ObjectInfo : public NodeInfo
	{
		ObjectInfo()
			:version(-1)
		{
		}

		QByteArray typeId;
		QSharedPointer<idoc::IDocumentMetaInfo> dataMetaInfoPtr;

		/**
			The server assigns the version to the document to be used when updating the data.
			When we want to change a document or information about it on the server,
			this version must be passed in the request. The version is used to synchronize
			simultaneous access to a document by multiple clients.
		*/
		int version;
	};


	struct ElementInfo : public ObjectInfo
	{
		ElementInfo()
			:isNode(false)
		{
		}

		bool isNode;
	};

	typedef QVector<ElementInfo> ElementList;

	virtual bool GetNodeInfo(NodeInfo& out) const = 0;
	virtual bool GetObjectInfo(ObjectInfo& out) const = 0;
	virtual bool GetElementInfo(ElementInfo& out) const = 0;
	virtual bool GetElementList(ElementList& out) const = 0;
};


} // namespace imtgql


Q_DECLARE_METATYPE(imtgql::IGqlStructuredCollectionResponse::NodeInfo);
Q_DECLARE_METATYPE(imtgql::IGqlStructuredCollectionResponse::ObjectInfo);
Q_DECLARE_METATYPE(imtgql::IGqlStructuredCollectionResponse::ElementList);


