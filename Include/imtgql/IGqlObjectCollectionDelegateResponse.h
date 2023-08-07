#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <ilog/IMessageContainer.h>
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtgql/IGqlClient.h>


namespace imtgql
{


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

	QSharedPointer<idoc::IDocumentMetaInfo> collectionItemMetaInfoPtr;
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


struct ElementInfo: public ObjectInfo
{
	ElementInfo()
		:isNode(false)
	{
	}

	bool isNode;
};

typedef QVector<ElementInfo> ElementList;

class IResponseBase:
			virtual public IGqlClient::ResponseHandler,
			virtual public ilog::IMessageContainer
{
public:
	virtual bool GetResult() const = 0;
	virtual bool GetValue(QVariant& resultOut) const = 0;
};


} // namespace imtgql


Q_DECLARE_METATYPE(imtgql::NodeInfo);
Q_DECLARE_METATYPE(imtgql::ObjectInfo);
Q_DECLARE_METATYPE(imtgql::ElementInfo);
Q_DECLARE_METATYPE(imtgql::ElementList);


