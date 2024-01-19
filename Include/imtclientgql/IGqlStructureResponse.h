#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtgql/IGqlPrimitiveTypeResponse.h>
#include <imtbase/IHierarchicalStructureInfo.h>


namespace imtclientgql
{


class IGqlStructureResponse : virtual public imtgql::IGqlPrimitiveTypeResponse
{
public:
    virtual imtbase::IHierarchicalStructureInfo::Ids GetNodeIds() const = 0;
    virtual imtbase::IHierarchicalStructureInfo::NodeInfo GetNodeInfo() const = 0;
};


} // namespace imtclientgql


