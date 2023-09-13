#include "CPosition.h"
#include "qdebug.h"

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
    bool ok = !m_coordinate.isValid() ? true : m_coordinate.latitude() != lat;
    if(ok){
        m_coordinate.setLatitude(lat);
    }
}

double CPosition::GetLongitude() const
{
    return m_coordinate.longitude();
}

void CPosition::SetLongitude(double lon)
{
    bool ok = !m_coordinate.isValid() ? true : m_coordinate.longitude() != lon;
    if(ok){
        m_coordinate.setLongitude(lon);
    }
}

bool CPosition::Serialize(iser::IArchive &archive)
{
    return false;
}


} // namespace imtgeo



