#include <imtbase/CLicenseManager.h>


// ACF includes
#include <istd/TSingleFactory.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/CLicenseInfo.h>


namespace imtbase
{


// public methods

CLicenseManager::CLicenseManager()
{
	m_licenses.RegisterFactory(new istd::TSingleFactory<istd::IChangeable, CLicenseInfo>("LicenseInfo"), true);
}


// reimplemented (imtbase::ILicenseInfoProvider)

const imtbase::ICollectionInfo & CLicenseManager::GetLicenseList() const
{
	return m_licenses;
}


const imtbase::ILicenseInfo* CLicenseManager::GetLicenseInfo(const QByteArray& licenseId) const
{
	return dynamic_cast<const imtbase::ILicenseInfo*>(m_licenses.GetObjectPtr(licenseId));
}


// reimplemented (istd::IChangeable)

bool CLicenseManager::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CLicenseManager* sourcePtr = dynamic_cast<const imtbase::CLicenseManager*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		return m_licenses.CopyFrom(sourcePtr->m_licenses);
	}

	return false;
}


bool CLicenseManager::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	return m_licenses.ResetData();
}


} // namespace imtbase


