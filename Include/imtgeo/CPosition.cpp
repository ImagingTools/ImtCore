#include "CPosition.h"
#include "qdebug.h"

// STL includes
#include <cmath>

// ACF includes
#include <istd/CChangeNotifier.h>


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

        istd::CChangeNotifier notifier(this);

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

        istd::CChangeNotifier notifier(this);

    }
}

bool CPosition::Serialize(iser::IArchive &archive)
{
    istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

    bool retVal = false;

    static iser::CArchiveTag latTag("Latitude", "Latitude address", iser::CArchiveTag::TT_LEAF);
    retVal = archive.BeginTag(latTag);
    double lat = m_coordinate.latitude();
    retVal = retVal && archive.Process(lat);
    retVal = retVal && archive.EndTag(latTag);

    static iser::CArchiveTag lonTag("Longitude", "Longitude address", iser::CArchiveTag::TT_LEAF);
    retVal = archive.BeginTag(lonTag);
    double lon = m_coordinate.longitude();
    retVal = retVal && archive.Process(lon);
    retVal = retVal && archive.EndTag(lonTag);

    return retVal;
}


} // namespace imtgeo



