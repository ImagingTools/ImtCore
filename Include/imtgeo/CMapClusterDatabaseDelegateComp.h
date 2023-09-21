#pragma once


// ImtCore includes
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>
#include <imtgeo/ICluster.h>



namespace imtgeo
{


class CMapClusterDatabaseDelegateComp: public imtdb::CSqlJsonDatabaseDelegateComp
{
public:
    typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

    I_BEGIN_COMPONENT(CMapClusterDatabaseDelegateComp)
    I_END_COMPONENT

    virtual bool CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;

private:
};


} // namespace imtgeo


