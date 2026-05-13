// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCluster.h"


// ACF includes
#include <istd/CChangeNotifier.h>


using namespace std;

namespace imtgeo
{

CCluster::CCluster()
{

}

CCluster::~CCluster()
{

}

QByteArrayList CCluster::GetChildrenIds() const
{
    return m_childrenIds;
}

void CCluster::SetChildrenIds(QByteArrayList &list)
{
    if(m_childrenIds != list){
        m_childrenIds = list;

        istd::CChangeNotifier notifier(this);

    }
}

double CCluster::GetZoom() const
{
    return m_zoom;
}

void CCluster::SetZoom(double zoom)
{

    if(zoom != m_zoom){
        m_zoom = zoom;

        istd::CChangeNotifier notifier(this);
    }

}

bool CCluster::Serialize(iser::IArchive &archive)
{
    istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

    bool retVal = BaseClass::Serialize(archive);

    retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_childrenIds, "ObjectIds", "ObjectId");

    iser::CArchiveTag zoomLevelTag("ZoomLevel", "Map zoom level", iser::CArchiveTag::TT_LEAF);
    retVal = archive.BeginTag(zoomLevelTag);
    retVal = retVal && archive.Process(m_zoom);
    retVal = retVal && archive.EndTag(zoomLevelTag);

    return retVal;
}


// public methods

} // namespace imtgeo



