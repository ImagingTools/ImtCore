#include "CCluster.h"


using namespace std;

namespace imtgeo
{

CCluster::CCluster()
{

}

CCluster::~CCluster()
{

}

QStringList CCluster::GetChildrenIds() const
{
    return m_childrenIds;
}

void CCluster::SetChildrenIds(QStringList &list)
{
    if(m_childrenIds != list){
        m_childrenIds = list;
    }
}

bool CCluster::Serialize(iser::IArchive &archive)
{
    return false;
}


// public methods

} // namespace imtgeo



