// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CPosition.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtgeo
{


// static tag wrappers

const iser::CArchiveTag& s_latTag(){
	static const iser::CArchiveTag tag(QByteArrayLiteral("Latitude"), QByteArrayLiteral("Latitude"), iser::CArchiveTag::TT_LEAF);

	return tag;
}

const iser::CArchiveTag& s_lonTag(){
	static const iser::CArchiveTag tag(QByteArrayLiteral("Longitude"), QByteArrayLiteral("Longitude"), iser::CArchiveTag::TT_LEAF);

	return tag;
}

const iser::CArchiveTag& s_zoomTag(){
	static const iser::CArchiveTag tag(QByteArrayLiteral("ZoomLevel"), QByteArrayLiteral("Zoom level"), iser::CArchiveTag::TT_LEAF);

	return tag;
}


// public methods

CPosition::CPosition():
	m_latitude(0.0),
	m_longitude(0.0),
	m_zoomLevel(1.0){}

CPosition::~CPosition(){}


// reimplemented (imtgeo::IPosition)

double CPosition::GetLatitude() const
{
	return m_latitude;
}


void CPosition::SetLatitude(const double& lat)
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


void CPosition::SetLongitude(const double& lon)
{
	if(m_longitude != lon){
		m_longitude = lon;
		istd::CChangeNotifier notifier(this);
	}
}


int CPosition::GetZoomLevel() const
{
	return m_zoomLevel;
}


void CPosition::SetZoomLevel(const double& zoom)
{
	if(!qFuzzyCompare(m_zoomLevel, zoom)){
		m_zoomLevel = zoom;
		istd::CChangeNotifier notifier(this);
	}
}


// reimplemented (iser::ISerializable)

bool CPosition::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	retVal = retVal && archive.BeginTag(s_latTag());
	retVal = retVal && archive.Process(m_latitude);
	retVal = retVal && archive.EndTag(s_latTag());

	retVal = retVal && archive.BeginTag(s_lonTag());
	retVal = retVal && archive.Process(m_longitude);
	retVal = retVal && archive.EndTag(s_lonTag());

	retVal = retVal && archive.BeginTag(s_zoomTag());
	retVal = retVal && archive.Process(m_zoomLevel);
	retVal = retVal && archive.EndTag(s_zoomTag());

	return retVal;
}


// reimplemented (istd::IChangeable)

int CPosition::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CPosition::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CPosition* sourcePtr = dynamic_cast<const CPosition*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_latitude	= sourcePtr->m_latitude;
		m_longitude = sourcePtr->m_longitude;
		m_zoomLevel = sourcePtr->m_zoomLevel;

		return true;
	}

	return false;
}


bool CPosition::IsEqual(const IChangeable& object) const
{
	const CPosition* sourcePtr = dynamic_cast<const CPosition*>(&object);
	if (sourcePtr != nullptr){
		bool retVal = m_latitude == sourcePtr->m_latitude;
		retVal = retVal && m_longitude == sourcePtr->m_longitude;
		retVal = retVal && m_zoomLevel == sourcePtr->m_zoomLevel;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CPosition::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CPosition());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CPosition::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_latitude = 0.0;
	m_longitude = 0.0;
	m_zoomLevel = 1.0;

	return true;
}


} // namespace imtgeo



