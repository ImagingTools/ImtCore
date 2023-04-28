#pragma once

// mongocxx includes
#include <bsoncxx/json.hpp>
#include <mongocxx/cursor.hpp>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtmongo/IMongoDatabaseObjectDelegate.h>


namespace imtmongo
{


/**
	Common interface for a object collection query.
	\ingroup Collection
*/
class CMongoDatabaseObjectCollectionIterator: virtual public imtbase::IObjectCollectionIterator
{
	typedef imtbase::IObjectCollection::DataPtr DataPtr;
public:
	CMongoDatabaseObjectCollectionIterator(mongocxx::cursor& cur, IMongoDatabaseObjectDelegate* databaseDelegate);

	// reimplemented (imtbase::IObjectCollectionIterator)
	virtual bool Next() override;
	virtual bool Previous() override;
	virtual QByteArray GetObjectId() const override;
	virtual bool GetObjectData(imtbase::IObjectCollection::DataPtr& dataPtr) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo() const override;
	virtual QVariant GetElementInfo(QByteArray infoId) const override;
private:
	mongocxx::cursor m_cursor;
	mongocxx::cursor::iterator m_iterator;

	IMongoDatabaseObjectDelegate* m_databaseDelegate;

};


} // namespace imtmongo


