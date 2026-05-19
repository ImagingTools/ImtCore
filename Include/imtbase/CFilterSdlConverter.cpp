// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CFilterSdlConverter.h>


// ImtCore includes
#include <imtbase/CFilterSerializer.h>


namespace imtbase
{


QJsonObject CFilterSdlConverter::ToSdlJson(const CFilter& filter)
{
    return CFilterSerializer::ToQmlJson(filter);
}


bool CFilterSdlConverter::FromSdlJson(const QJsonObject& filterModel, CFilter& filter)
{
    return CFilterSerializer::FromJson(filterModel, filter);
}


bool CFilterSdlConverter::FromQmlModel(const QVariant& filterModel, CFilter& filter)
{
    if (!filterModel.canConvert<QVariantMap>()){
        return false;
    }

    return FromSdlJson(QJsonObject::fromVariantMap(filterModel.toMap()), filter);
}


} // namespace imtbase
