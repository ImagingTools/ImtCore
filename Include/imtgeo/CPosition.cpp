#include <imtgeo/CPosition.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtgeo
{


// statics

const iser::CArchiveTag& s_latTag() {
	static const iser::CArchiveTag tag(QByteArrayLiteral("Latitude"), QByteArrayLiteral("Latitude address"), iser::CArchiveTag::TT_LEAF);

	return tag;
}

const iser::CArchiveTag& s_lonTag() {
	static const iser::CArchiveTag tag(QByteArrayLiteral("Longitude"), QByteArrayLiteral("Longitude address"), iser::CArchiveTag::TT_LEAF);

	return tag;
}


// public methods

CPosition::CPosition():
	m_latitude(0.0),
	m_longitude(0.0) {}

CPosition::~CPosition() {}


// reimplemented (imtgeo::IPosition)

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


// reimplemented (iser::ISerializable)

bool CPosition::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = false;

	retVal = retVal && archive.BeginTag(s_latTag());
	retVal = retVal && archive.Process(m_latitude);
	retVal = retVal && archive.EndTag(s_latTag());

	retVal = retVal && archive.BeginTag(s_lonTag());
	retVal = retVal && archive.Process(m_longitude);
	retVal = retVal && archive.EndTag(s_lonTag());

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

		return true;
	}

	return false;
}


bool CPosition::IsEqual(const IChangeable& object) const
{
	const CPosition* sourcePtr = dynamic_cast<const CPosition*>(&object);
	if (sourcePtr != nullptr){
		return (m_latitude == sourcePtr->m_latitude
				&& m_longitude == sourcePtr->m_longitude);
	}

	return false;
}


istd::IChangeable* CPosition::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::TDelPtr<CPosition> clonePtr(new CPosition());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CPosition::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_latitude = 0.0;
	m_longitude = 0.0;

	return true;
}


} // namespace imtgeo



