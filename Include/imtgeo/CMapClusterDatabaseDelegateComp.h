// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>



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


