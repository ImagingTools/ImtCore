#include "CPosition.h"

// STL includes
#include <cmath>

using namespace std;

namespace imtgeo
{

CPosition::CPosition()
{

}

CPosition::~CPosition()
{

}

double CPosition::GetLatitude() const
{
    return m_coordinate.latitude();
}

void CPosition::SetLatitude(double lat)
{
    if(m_coordinate.latitude() != lat){
        m_coordinate.setLatitude(lat);
    }
}

double CPosition::GetLongitude() const
{
    return m_coordinate.longitude();
}

void CPosition::SetLongitude(double lon)
{
    if(m_coordinate.longitude() != lon){
        m_coordinate.setLongitude(lon);
    }
}

bool CPosition::Serialize(iser::IArchive &archive)
{
    return false;
}


} // namespace imtgeo



