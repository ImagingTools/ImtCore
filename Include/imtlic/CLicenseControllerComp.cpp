#include <imtlic/CLicenseControllerComp.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QTimer>

// ACF includes
#include <istd/CCrcCalculator.h>

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

	// Remove all existing license data:
	m_productInstanceCompPtr->ResetData();

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

	QString fingerprintFilePath = GetFingerprintPath();

	int state = m_productInstancePersistenceCompPtr->LoadFromFile(*m_productInstanceCompPtr, licenseFilePath);
	if (state != ifile::IFilePersistence::OS_OK){
		int daysUntilExpire = -1;
		if (LoadFingerprint(fingerprintFilePath, daysUntilExpire)){
			SendWarningMessage(0, QString(QObject::tr("You have no valid license to run this software anymore. You have %1 day(s) to update your system with a valid license")).arg(daysUntilExpire), "License Management");
		}
		else{
			SendErrorMessage(0, QString(QObject::tr("You have no license to run this software. License file: '%1'")).arg(licenseFilePath), "License Management");
		}
	}
	else{
		if (!UpdateFingerprint(fingerprintFilePath)){
			SendErrorMessage(0, QString(QObject::tr("License fingerprint could not be updated")), "License Management");
		}

		SendInfoMessage(0, QString(QObject::tr("License was successfully loaded")), "License Management");
	}

	int oneHour = 1000 * 60 * 60;

	QTimer::singleShot(oneHour, this, &CLicenseControllerComp::OnFingeprintCheckTimer);
}


bool CLicenseControllerComp::UpdateFingerprint(const QString& fingerprintFilePath) const
{
	if (!m_fingerprintInstancePersistenceCompPtr.IsValid()){
		return false;
	}

	return (m_fingerprintInstancePersistenceCompPtr->SaveToFile(*m_productInstanceCompPtr, fingerprintFilePath) == ifile::IFilePersistence::OS_OK);
}


bool CLicenseControllerComp::LoadFingerprint(const QString& filePath, int& daysUntilExpire)
{
	daysUntilExpire = -1;

	if (!m_fingerprintInstancePersistenceCompPtr.IsValid()){
		return false;
	}

	QFileInfo fingerprintInfo(filePath);
	if (!fingerprintInfo.exists()){
		return false;
	}

	if (m_fingerprintExpirationAttrPtr.IsValid()){
		QDateTime currentTime = QDateTime::currentDateTime();
		QDateTime fingerprintTimeStamp = fingerprintInfo.lastModified();

		int days = fingerprintTimeStamp.daysTo(currentTime);
		if (days < *m_fingerprintExpirationAttrPtr){
			daysUntilExpire = *m_fingerprintExpirationAttrPtr - days;

			return (m_fingerprintInstancePersistenceCompPtr->LoadFromFile(*m_productInstanceCompPtr, filePath) == ifile::IFilePersistence::OS_OK);
		}
	}

	return false;
}


QString CLicenseControllerComp::GetFingerprintPath() const
{
	QString licenseFilePath = m_licensePathCompPtr->GetPath();
	if (licenseFilePath.isEmpty()){
		return QString();
	}

	QFileInfo licenseFileInfo(licenseFilePath);
	if (!licenseFileInfo.exists()){
		return QString();
	}

	int checksum = istd::CCrcCalculator::GetCrcFromFile(licenseFilePath);

	QString checkSumString = QString::number(checksum, 16).rightJustified(8, '0').toUpper();

	QString fingerprintFilePath = QDir::tempPath() + "/" + checkSumString + ".xli";

	return fingerprintFilePath;
}


void CLicenseControllerComp::OnLicenseKeysUpdated(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtcrypt::IEncryptionKeysProvider* /*licenseKeysProviderPtr*/)
{
	if (!m_isInitializing){
		SendInfoMessage(0, tr("License key was changed. Checking license fingerprint"), tr("License Controller"));

		ReadLicenseFile();
	}
}


void CLicenseControllerComp::OnFingeprintCheckTimer()
{
	SendVerboseMessage(tr("Checking license fingerprint"), tr("License Controller"));

	ReadLicenseFile();
}


} // namespace imtlic


