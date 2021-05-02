#include <imtlic/CLicenseControllerComp.h>


// Qt includes
#include <QtCore/QFileInfo>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/ILicenseInstance.h>


namespace imtlic
{


// public methods

CLicenseControllerComp::CLicenseControllerComp()
	:m_licenseKeysProvider(*this),
	m_isInitializing(false)
{
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CLicenseControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_isInitializing = true;

	m_licenseKeysProvider.RegisterObject(m_licenseKeysProviderCompPtr.GetPtr(), &CLicenseControllerComp::OnLicenseKeysUpdated);

	m_isInitializing = false;
}


void CLicenseControllerComp::OnComponentDestroyed()
{
	m_licenseKeysProvider.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CLicenseControllerComp::ReadLicenseFile()
{
	if (!m_licensePathCompPtr.IsValid()){
		SendCriticalMessage(0, "No license path component was set. Please check component configuration", "CLicenseControllerComp::ReadLicenseFile");

		return;
	}

	if (!m_productInstanceCompPtr.IsValid()){
		SendCriticalMessage(0, "No product instance component was set. Please check component configuration", "CLicenseControllerComp::ReadLicenseFile");

		return;
	}

	if (!m_productInstancePersistenceCompPtr.IsValid()){
		SendCriticalMessage(0, "No product instance persistence component was set. Please check component configuration", "CLicenseControllerComp::ReadLicenseFile");

		return;
	}

	QString licenseFilePath = m_licensePathCompPtr->GetPath();
	if (licenseFilePath.isEmpty()){
		SendCriticalMessage(0, "License file path is empty. Please check component configuration", "CLicenseControllerComp::ReadLicenseFile");

		return;
	}

	QFileInfo licenseFileInfo(licenseFilePath);
	if (!licenseFileInfo.exists()){
		SendErrorMessage(0, QString(QObject::tr("License file '%1' doesn't exist")).arg(licenseFilePath), "License Management");

		return;
	}

	int state = m_productInstancePersistenceCompPtr->LoadFromFile(*m_productInstanceCompPtr, licenseFilePath);
	if (state != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, QString(QObject::tr("You have no license to run this software. License file: '%1'")).arg(licenseFilePath), "License Management");

		return;
	}
}


void CLicenseControllerComp::OnLicenseKeysUpdated(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtcrypt::IEncryptionKeysProvider* /*licenseKeysProviderPtr*/)
{
	if (!m_isInitializing){
		ReadLicenseFile();
	}
}


} // namespace imtlic


