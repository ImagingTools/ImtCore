#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtgql/IGqlResponse.h>
#include <imtbase/IHierarchicalStructureInfo.h>


namespace imtclientgql
{


class IGqlStructureResponse : virtual public imtgql::IGqlResponse
{
public:
    virtual imtbase::IHierarchicalStructureInfo::Ids GetNodeIds() const = 0;
	virtual imtbase::IHierarchicalStructureInfo::ItemInfo GetItemInfo() const = 0;
};


} // namespace imtclientgql


