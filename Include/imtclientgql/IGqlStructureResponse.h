#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtgql/IGqlPrimitiveTypeResponse.h>
#include <imtbase/ICollectionStructureInfo.h>


namespace imtclientgql
{


class IGqlStructureResponse : virtual public imtgql::IGqlPrimitiveTypeResponse
{
public:
    virtual imtbase::ICollectionStructureInfo::Ids GetNodeIds() const = 0;
    virtual imtbase::ICollectionStructureInfo::NodeInfo GetNodeInfo() const = 0;
};


} // namespace imtclientgql


