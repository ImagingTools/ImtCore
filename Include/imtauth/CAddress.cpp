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

// reimplemented (IAddress)

CAddress::CAddress()
	:m_postalCode(-1)
{
}


// reimplemented (IAddress)

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


// reimplemented (iser::IObject)

QByteArray CAddress::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (iser::ISerializable)

bool CAddress::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag cityTag("City", "City name", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(cityTag);
	retVal = retVal && archive.Process(m_city);
	retVal = retVal && archive.EndTag(cityTag);

	static iser::CArchiveTag countryTag("Country", "Country name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(countryTag);
	retVal = retVal && archive.Process(m_country);
	retVal = retVal && archive.EndTag(countryTag);

	static iser::CArchiveTag postalCodeTag("PostalCode", "Postal code", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(postalCodeTag);
	retVal = retVal && archive.Process(m_postalCode);
	retVal = retVal && archive.EndTag(postalCodeTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CAddress::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CAddress::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IAddress* sourcePtr = dynamic_cast<const IAddress*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_city = sourcePtr->GetCity();
		m_country = sourcePtr->GetCountry();
		m_postalCode = sourcePtr->GetPostalCode();

		return true;
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

	m_city.clear();
	m_country.clear();
	m_postalCode = -1;

	return true;
}


} // namespace imtauth


