#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtgql/IGqlResponse.h>


namespace imtclientgql
{

 
class IGqlObjectCollectionResponse : virtual public imtgql::IGqlResponse
{
public:
	struct ObjectInfo
	{
		ObjectInfo()
			:version(-1)
		{
		}

		QByteArray id;
		QByteArray typeId;
		QString name;
		QString description;

		QMap<int, QVariant> elementInfo;

		/**
			The server assigns the version to the document to be used when updating the data.
			When we want to change a document or information about it on the server,
			this version must be passed in the request. The version is used to synchronize
			simultaneous access to a document by multiple clients.
		*/
		int version;
	};
	typedef QList<ObjectInfo> ObjectList;

	virtual bool GetObjectInfo(ObjectInfo& out) const = 0;
	virtual bool GetObjectList(ObjectList& out) const = 0;
	virtual bool GetMetaInfo(idoc::IDocumentMetaInfo& out) const = 0;
	virtual bool DeSerializeObject(istd::IChangeable& object) const = 0;
};


} // namespace imtclientgql


