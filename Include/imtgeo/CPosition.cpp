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
    return m_latitude;
}

void CPosition::SetLatitude(double lat)
{
    if(m_latitude != lat){
        m_latitude = lat;

        istd::CChangeNotifier notifier(this);

    }
}

double CPosition::GetLongitude() const
{
    return m_longitude;
}

void CPosition::SetLongitude(double lon)
{
    if(m_longitude != lon){
        m_longitude = lon;

        istd::CChangeNotifier notifier(this);

    }
}

bool CPosition::Serialize(iser::IArchive &archive)
{
    istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

    bool retVal = false;

    iser::CArchiveTag latTag("Latitude", "Latitude address", iser::CArchiveTag::TT_LEAF);
    retVal = archive.BeginTag(latTag);
        retVal = retVal && archive.Process(m_latitude);
    retVal = retVal && archive.EndTag(latTag);

    iser::CArchiveTag lonTag("Longitude", "Longitude address", iser::CArchiveTag::TT_LEAF);
    retVal = archive.BeginTag(lonTag);
    retVal = retVal && archive.Process(m_longitude);
    retVal = retVal && archive.EndTag(lonTag);

    return retVal;
}


} // namespace imtgeo



