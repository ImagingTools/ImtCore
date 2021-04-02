#include <imtauth/CAddress.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauth
{


// public static methods

QByteArray CAddress::GetTypeId()
{
	return "Address";
}


// public methods

CAddress::CAddress()
	:m_postalCode(-1)
{
	m_country = QObject::tr("Country");
	m_city = QObject::tr("City");
}


// reimplemented (IAddress)

QString CAddress::GetCountry() const
{
	return m_country;
}


void CAddress::SetCountry(const QString & country)
{
	if (m_country != country){
		istd::CChangeNotifier changeNotifier(this);

		m_country = country;
	}
}


QString CAddress::GetCity() const
{
	return m_city;
}


void CAddress::SetCity(const QString& city)
{
	if (m_city != city){
		istd::CChangeNotifier changeNotifier(this);

		m_city = city;
	}
}


int CAddress::GetPostalCode() const
{
	return m_postalCode;
}


void CAddress::SetPostalCode(int postalCode)
{
	if (m_postalCode != postalCode){
		istd::CChangeNotifier changeNotifier(this);

		m_postalCode = postalCode;
	}
}


QString CAddress::GetStreet() const
{
	return m_street;
}


void CAddress::SetStreet(const QString& street)
{
	if (m_street != street){
		istd::CChangeNotifier changeNotifier(this);

		m_street = street;
	}
}


// reimplemented (iser::IObject)

QByteArray CAddress::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (iser::ISerializable)

bool CAddress::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag countryTag("Country", "Country name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(countryTag);
	retVal = retVal && archive.Process(m_country);
	retVal = retVal && archive.EndTag(countryTag);
	
	static iser::CArchiveTag cityTag("City", "City name", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(cityTag);
	retVal = retVal && archive.Process(m_city);
	retVal = retVal && archive.EndTag(cityTag);

	static iser::CArchiveTag postalCodeTag("PostalCode", "Postal code", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(postalCodeTag);
	retVal = retVal && archive.Process(m_postalCode);
	retVal = retVal && archive.EndTag(postalCodeTag);

	static iser::CArchiveTag streetTag("Street", "Street", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(streetTag);
	retVal = retVal && archive.Process(m_street);
	retVal = retVal && archive.EndTag(streetTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CAddress::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CAddress::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CAddress* sourcePtr = dynamic_cast<const CAddress*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_country = sourcePtr->m_country;
		m_city = sourcePtr->m_city;
		m_postalCode = sourcePtr->m_postalCode;
		m_street = sourcePtr->m_street;

		return true;
	}

	return false;
}


bool CAddress::IsEqual(const IChangeable& object) const
{
	const CAddress* sourcePtr = dynamic_cast<const CAddress*>(&object);
	if (sourcePtr != nullptr){
		bool retVal = m_country == sourcePtr->m_country;
		retVal = retVal && m_city == sourcePtr->m_city;
		retVal = retVal && m_postalCode == sourcePtr->m_postalCode;
		retVal = retVal && m_street == sourcePtr->m_street;

		return retVal;
	}

	return false;
}


istd::IChangeable* CAddress::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CAddress> clonePtr(new CAddress);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CAddress::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_country.clear();
	m_city.clear();
	m_postalCode = -1;
	m_street.clear();

	return true;
}


} // namespace imtauth


